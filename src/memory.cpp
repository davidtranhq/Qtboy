#include "memory.hpp"
#include "timer.hpp"
#include "ppu.hpp"
#include "joypad.h"
#include "exception.hpp"

#include <cstdint>
// #include <QDebug>

namespace gameboy
{

Memory::Memory(Ppu &p, Timer &t, Joypad &j)
    : ppu_ {p},
      timer_ {t},
      joypad_ {j}
{
    init_io();
}

uint8_t Memory::read(uint16_t adr) const
{
    if (!cart_) // no cartridge inserted
        return 0xff;

    uint8_t b;

    if (adr < 0x8000) // ROM bank accessing
    {
        b = cart_->read(adr); // MBC on cartridge
    }
    else if (adr < 0xa000) // VRAM accessing
    {
        uint16_t a = adr - 0x8000; // adjusted for placement in memory map
        b = vram_[a];
    }
    else if (adr < 0xc000) // external RAM accessing
    {
        b = cart_->read(adr); // MBC on cartridge
    }
    else if (adr < 0xd000) // WRAM bank 0 access
    {
        b = wram0_[adr-0xc000];
    }
    else if (adr < 0xe000) // WRAM bank 1-7 accessing (CGB)
    {
        uint16_t a = adr - 0xd000; // adjust
        b = wram1_[a];
    }
    else if (adr < 0xfe00) // echo RAM
    {
        return read(adr - 0x2000);  // echo ram of 0xc000-0xddff
    }
    else if (adr < 0xfea0) // OAM accessing
    {
        b = oam_[adr - 0xfe00];
    }
    else if (adr < 0xff00) // undefined
    {
        // on a real gameboy these addresses don't point to any memory so
        // reading from it is undefined!
        b = 0xff;
    }
    else if (adr < 0xff80) // IO accessing
    {
        if (adr == 0xff00)
            b = joypad_.read_reg();
        else if (adr > 0xff03 && adr < 0xff08) // timer registers
            b = timer_.read(adr);
        else if (adr > 0xff39 && adr < 0xff4c && adr != 0xff46) // ppu registers
            b = ppu_.read_reg(adr);
        else
            b = io_[adr - 0xff00];
    }
    else if (adr < 0xffff) // High RAM accessing
    {
        b = hram_[adr - 0xff80];
    }
    else // adr == 0xffff, interrupt enable register
    {
        b = ie_;
    }
    return b;
}

//QTextStream &qStdOut()
//{
//    static QTextStream ts (stdout);
//    ts.flush();
//    return ts;
//}

void Memory::write(uint8_t b, uint16_t adr)
{
    if (!cart_) // no cartridge inserted
        return;
    was_written_ = true;
    if (adr == 0xff02 && b == 0x81)
        // qStdOut() << static_cast<char>(read(0xff01));
        std::cout << static_cast<char>(read(0xff01));
    if (adr < 0x8000) // enabling flags (dependant on MBC)
    {
        cart_->write(b, adr);
    }
    else if (adr < 0xa000) // VRAM accessing
    {
        uint16_t a = adr - 0x8000; // adjusted for placement in memory map
        vram_[a] = b;
    }
    else if (adr < 0xc000) // RAM bank access
    {
        cart_->write(b, adr); // MBC on cartridge
    }
    else if (adr < 0xd000) // write to WRAM bank 0
    {
        wram0_[adr-0xc000] = b;
    }
    else if (adr < 0xe000) // write to WRAM bank 1
    {
        wram1_[adr-0xd000] = b;
    }
    else if (adr < 0xfe00) // echo RAM
    {
        write(b, adr - 0x2000);  // echo ram of 0xc000-0xddff
    }
    else if (adr < 0xfea0) // OAM accessing
    {
        oam_[adr - 0xfe00] = b;
    }
    else if (adr < 0xff00) // undefined
    {
        // on a real gameboy these addresses don't point to any memory so
        // writing to it is undefined!
    }
    else if (adr < 0xff80) // IO accessing
    {
        if (adr == 0xff00)
            joypad_.write_reg(b);
        else if (adr > 0xff03 && adr < 0xff08) // timer registers
            timer_.write(b, adr);
        else if (adr > 0xff3f && adr < 0xff4c && adr != 0xff46) // PPU registers
            ppu_.write_reg(b, adr);
        else if (adr == 0xff46) // DMA
            dma_transfer(b);
        io_[adr - 0xff00] = b;
    }
    else if (adr < 0xffff) // High RAM accessing
    {
        hram_[adr - 0xff80] = b;
    }
    else // adr == 0xffff, interrupt enable register
    {
        ie_ = b;
    }
}


void Memory::load_cartridge(std::istream &is)
{
    cart_ = std::make_unique<Cartridge>(is);
    set_ram_size();
}

void Memory::reset()
{
    vram_ = {};
    wram0_ = {};
    wram1_ = {};
    oam_ = {};
    io_ = {};
    hram_ = {};
    ie_ = 0;
    init_io();
}

std::vector<uint8_t> Memory::dump_rom() const
{
    return (cart_ ? cart_->dump_rom() : std::vector<uint8_t> {});
}

std::unordered_map<std::string, Memory_range> Memory::dump() const
{
    std::unordered_map<std::string, Memory_range> out;
    if (!cart_) // no cartridge loaded
    {
        out["ROM0"] = Memory_range {"ROM0", std::vector<uint8_t>(0x4000)};
        out["ROMX"] = Memory_range {"ROM1", std::vector<uint8_t>(0x4000)};
    }
    else // cartridge dependant memory locations
    {
        std::map<std::string, Memory_range>
                cart {cart_->dump()};
        // concatenate memory map recieved from cartridge to output
        out.insert(cart.begin(), cart.end());
    }
    std::vector<uint8_t> wrm0 {wram0_.begin(), wram0_.end()};
    std::vector<uint8_t> wrm1 {wram1_.begin(), wram1_.end()};
    std::vector<uint8_t> vrm0 {vram_.begin(), vram_.end()};
    out["WRM0"] = {"WRM0", wrm0};
    out["VRMX"] = {"VRM0", vrm0};
    out["WRMX"] = {"WRM1", wrm1};
    std::vector<uint8_t> oam(oam_.begin(), oam_.end());
    std::vector<uint8_t> io(io_.begin(), io_.end());
    std::vector<uint8_t> hram(hram_.begin(), hram_.end());
    out["OAM"] = {"OAM", oam};
    out["IO"] = {"IO", io};
    out["HRAM"] = {"HRAM", hram};
    return out;
}

bool Memory::was_written()
{
    bool out = was_written_;
    was_written_ = false;
    return out;
}

void Memory::set_ram_size()
{}

void Memory::init_io()
{
    io_[0x00] = 0xff;
    io_[0x05] = 0x00;   // TIMA
    io_[0x06] = 0x00;   // TMA
    io_[0x07] = 0x00;   // TAC
    io_[0x10] = 0x80;   // NR10
    io_[0x11] = 0xBF;   // NR11
    io_[0x12] = 0xF3;   // NR12
    io_[0x14] = 0xBF;   // NR14
    io_[0x16] = 0x3F;   // NR21
    io_[0x17] = 0x00;   // NR22
    io_[0x19] = 0xBF;   // NR24
    io_[0x1A] = 0x7F;   // NR30
    io_[0x1B] = 0xFF;   // NR31
    io_[0x1C] = 0x9F;   // NR32
    io_[0x1E] = 0xBF;   // NR33
    io_[0x20] = 0xFF;   // NR41
    io_[0x21] = 0x00;   // NR42
    io_[0x22] = 0x00;   // NR43
    io_[0x23] = 0xBF;   // NR44
    io_[0x24] = 0x77;   // NR50
    io_[0x25] = 0xF3;   // NR51
    io_[0x26] = 0xF1;   // NR52 (0xf1-GB, 0xF0-SGB)
    io_[0x40] = 0x91;   // LCDC
    io_[0x42] = 0x00;   // SCY
    io_[0x43] = 0x00;   // SCX
    io_[0x44] = 0x91;   // LY
    io_[0x45] = 0x00;   // LYC
    io_[0x47] = 0xFC;   // BGP
    io_[0x48] = 0xFF;   // OBP0
    io_[0x49] = 0xFF;   // OBP1
    io_[0x4A] = 0x00;   // WY
    io_[0x4B] = 0x00;   // WX
    io_[0xFF] = 0x00;   // IE
}

void Memory::dma_transfer(uint8_t b)
{
    if (b > 0xf1) // only defined for range between 00-f1
        throw Bad_memory("Attempted to write value outside 00-f1"
                         "to DMA register (ff46)\n", __FILE__, __LINE__);
    // copy from XX00-XX9f to oam (fe00-fe9f), where XXh = b
    for (uint8_t i {0}; i < oam_.size(); ++i)
        oam_[i] = read(b << 8 | i);
}

}


