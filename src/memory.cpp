#include "processor.hpp"
#include "memory.hpp"
#include "timer.hpp"
#include "ppu.hpp"
#include "joypad.hpp"
#include "exception.hpp"
#include "apu.hpp"

#include <cstdint>
// #include <QDebug>

namespace gameboy
{

Memory::Memory(Processor &c, Ppu &p, Timer &t, Joypad &j, Apu &a)
    : cpu_ {c},
      ppu_ {p},
      timer_ {t},
      joypad_ {j},
      apu_ {a}
{
    init_io();
}

void Memory::enable_cgb(bool is_cgb)
{
    cgb_mode_ = is_cgb;
}

uint8_t Memory::read(uint16_t adr) const
{
    if (!cart_) // no cartridge inserted
        return 0xff;

    uint8_t b {0xff};

    if (adr < 0x8000) // ROM bank accessing
    {
        b = cart_->read(adr); // MBC on cartridge
    }
    else if (adr < 0xa000) // VRAM accessing
    {
        // VRAM accesible when PPU disabled or mode isn't 3
        if (!ppu_.enabled() || ppu_.mode() != 3)
        {
            uint16_t a = adr - 0x8000; // adjusted for placement in memory map
            // CGB can access banks 0-1
            if (cgb_mode_)
            {
                // get VRAM bank specified in bit 0 of 0xff4f
                uint8_t bank = io_[0x4f] & 1;
                b = vram_.read(bank, a);
            }
            // DMG can only access bank 0
            else
            {
                b = vram_.read(0, a);
            }
        }
    }
    else if (adr < 0xc000) // external RAM accessing
    {
        b = cart_->read(adr); // MBC on cartridge
    }
    else if (adr < 0xd000) // WRAM bank 0 access
    {
        b = wram_.read(0, adr-0xc000);
    }
    else if (adr < 0xe000) // WRAM bank 1-7 accessing (CGB)
    {
        uint16_t a = adr - 0xd000; // adjust
        if (cgb_mode_) // WRAM banks 1-7 are accessible in CGB mode
        {
            uint8_t bank = io_[0x70] & 7;
            if (bank == 0)
                bank = 1;
            b = wram_.read(bank, a);
        }
        else // only bank 1 is accessible in DMG
        {
            b = wram_.read(1, a);
        }
    }
    else if (adr < 0xfe00) // echo RAM
    {
        return read(adr - 0x2000);  // echo ram of 0xc000-0xddff
    }
    else if (adr < 0xfea0) // OAM accessing
    {
        //if (!ppu_.enabled() || ppu_.mode() < 2)
            b = oam_[adr - 0xfe00];
       // else
            //b = 0xff;
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
        else if (adr > 0xff0f && adr < 0xff40) // APU registers
            b = apu_.read_reg(adr);
        else if (adr > 0xff3f && adr < 0xff4c && adr != 0xff46) // ppu registers
            b = ppu_.read_reg(adr);
        else if (cgb_mode_ && adr >= 0xff68 && adr <= 0xff6b) // CGB PPU regs
            b = ppu_.read_reg(adr);
        else // misc. IO register
        {
            b = io_[adr - 0xff00];
            // special cases when reading from IO registers
            switch (adr)
            {
                case 0xff4d: // key1 (GBC only: sped switch)
                {
                    if (cgb_mode_)
                        b |= 0x7e; // only bits 0 and 7 are read, other bits=1
                    else
                        b = 0xff;
                } break;
                case 0xff4f: // vbk
                {
                    if (cgb_mode_)
                    {
                        b &= 1;
                        b |= 0xfe;
                    }
                    else
                    {
                        b = 0xfe;
                    }
                } break;
                // HDMA addresses always return 0xff when read
                case 0xff51: b = hdma_src_ >> 8; break;  // hdma1
                case 0xff52: b = hdma_src_ & 0xff; break; // hdma2
                case 0xff53: b = hdma_dest_ >> 8; break; // hdma3
                case 0xff54: b = hdma_dest_ & 0xff; break; // hdma4
                case 0xff55:// hdma5
                {
                    if (!cgb_mode_)
                        b = 0xff;
                    else
                        b = static_cast<uint8_t>(
                                    (hdma_len_ & 0x7f)
                                | ((hdma_active_ ? 0 : 1) << 7));
                } break;
                case 0xff70: // svbk
                {
                    if (cgb_mode_) // upper 5 bits always read 1 in CGB
                        b |= 0xf8;
                    else // always FF in DMG
                        b = 0xff;
                } break;
            }
        }
    }
    else if (adr < 0xffff) // High RAM accessing
    {
        b = hram_[adr - 0xff80];
    }
    else if (adr == 0xffff)// adr == 0xffff, interrupt enable register
    {
        b = ie_;
    }
    return b;
}

void Memory::write(uint8_t b, uint16_t adr)
{
    if (!cart_) // no cartridge inserted
    {
        return;
    }

    if (adr == 0xff02 && b == 0x81)
            // qStdOut() << static_cast<char>(read(0xff01));
            // std::cout << static_cast<char>(read(0xff01));
    // don't include ROM in memory logging b/c it shouldn't change
    // don't include echo RAM (WRAM log will handle it)
    // don't include unused portion
    if (logging_ &&
        ! ((adr < 0x8000) // ROM
        || (adr > 0xdfff && adr < 0xfe00) // echo RAM
        || (adr > 0xfe9f && adr < 0xff00)))
    {
        Memory_byte mb {};
        mb.adr = adr;
        mb.val = b;
        log_.push_back(mb);
    }

    if (adr < 0x8000) // enabling flags (dependant on MBC)
    {
        cart_->write(b, adr);
    }
    else if (adr < 0xa000) // VRAM accessing
    {
        // VRAM only accessible if PPU is enabled and PPU mode isn't 3

        if (!ppu_.enabled() || ppu_.mode() != 3)
        {
            uint16_t a = adr - 0x8000; // adjusted for placement in memory map
            // CGB can access banks 0-1
            if (cgb_mode_)
            {
                // get VRAM bank specified in bit 0 of 0xff4f
                uint8_t bank = io_[0x4f] & 1;
                vram_.write(b, bank, a);
            }
            // DMG can only access bank 0
            else
            {
                vram_.write(b, 0, a);
            }
        }
    }
    else if (adr < 0xc000) // RAM bank access
    {
        cart_->write(b, adr); // MBC on cartridge
        sram_written_ = true;
    }
    else if (adr < 0xd000) // write to WRAM bank 0
    {
        wram_.write(b, 0, adr-0xc000);
    }
    else if (adr < 0xe000) // write to WRAM bank 1-n
    {
        uint16_t a = adr - 0xd000; // adjust
        if (cgb_mode_) // WRAM banks 1-7 are accessible in CGB mode
        {
            uint8_t bank = io_[0x70] & 7;
            if (bank == 0)
                bank = 1;
            wram_.write(b, bank, a);
        }
        else // only bank 1 is accessible in DMG
        {
            wram_.write(b, 1, a);
        }
    }
    else if (adr < 0xfe00) // echo RAM
    {
        write(b, adr - 0x2000);  // echo ram of 0xc000-0xddff
    }
    else if (adr < 0xfea0) // OAM accessing
    {
        // OAM only accessible if PPU is enabled and PPU mode is 0 or 1
        if (!ppu_.enabled() || ppu_.mode() < 2)
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
        else if (adr > 0xff0f && adr < 0xff40) // APU registers
            apu_.write_reg(b, adr);
        else if (adr > 0xff3f && adr < 0xff4c && adr != 0xff46) // PPU registers
            ppu_.write_reg(b, adr);
        else if (cgb_mode_ && adr >= 0xff68 && adr <= 0xff6b) // CGB PPU regs
            ppu_.write_reg(b, adr);
        else if (adr == 0xff46) // DMA
            oam_dma_transfer(b);
        // misc. IO registers
        if (cgb_mode_)
        {
            switch (adr)
            {
                case 0xff4d:// CGB speed switch
                {
                    // bit 0 prepares speed switch
                    if (b & 1)
                    {
                        cpu_.toggle_double_speed();
                        // bit 0 cleared immediately after switching
                    }
                    // only bit 0 is writable
                    b &= ~1;
                    // bit 7 tells the current mode (1=double)
                    b &= ~0x80;
                    b |= static_cast<uint8_t>(cpu_.double_speed()) << 7;
                } break;
                case 0xff4f:
                {
                    // VRAM bank select: only bit 1 is writable
                    b &= 1;
                    b |= 0xfe;
                } break;
                case 0xff51: // HDMA src hi
                {
                    hdma_src_ = static_cast<uint16_t>(
                                (hdma_src_ & 0xff) | (b << 8));
                } break;
                case 0xff52: // HDMA src lo (ignore lo 4 bits)
                {
                    b &= 0xf0;
                    hdma_src_ = (hdma_src_ & 0xff00) | b;
                } break;
                case 0xff53: // HDMA dst hi (upper 3 bits ignored), bit 7 always 1
                {
                    b &= 0x1f;
                    hdma_dest_ = static_cast<uint16_t>(
                                (hdma_dest_ & 0xff) | ((b | 0x80) << 8));
                } break;
                case 0xff54: // HDMA dst lo (lower 4 bits ignored)
                {
                    b &= 0xf0;
                    hdma_dest_ = (hdma_dest_ & 0xff00) | b;
                } break;
                case 0xff55: vram_dma_transfer(b); return; // hdma transfer
                case 0xff70:
                {
                     // WRAM bank select: only banks 01-07 are adressable
                    b &= 7;
                    b |= 0xf8;
                } break;
            }
        }
        io_[adr - 0xff00] = b;
    }
    else if (adr < 0xffff) // High RAM accessing
    {
        hram_[adr - 0xff80] = b;
    }
    else if (adr == 0xffff) // adr == 0xffff, interrupt enable register
    {
        ie_ = b;
    }
}


uint8_t Memory::vram_read(uint8_t bank, uint16_t a) const
{
    if (a < 0x8000 || a > 0x9fff)
        return 0xff; // not in range of VRAM
    return vram_.read(bank, a-0x8000);
}

void Memory::vram_write(uint8_t b, uint8_t bank, uint16_t a)
{
    if (a < 0x8000 || a > 0x9fff)
        return; // not in range of VRAM
    vram_.write(b, bank, a-0x8000);
}


Cartridge *Memory::load_cartridge(std::istream &is)
{
    cart_ = std::make_unique<Cartridge>(is);
    set_ram_size();
    return cart_.get();
}

void Memory::reset()
{
    vram_.reset();
    wram_.reset();
    oam_ = {};
    io_ = {};
    hram_ = {};
    ie_ = 0;
    init_io();
    logging_ = false;
    log_.clear();
    cgb_mode_ = false;
    hdma_active_ = false;
    hdma_src_ = 0;
    hdma_dest_ = 0;
    hdma_len_ = 0xff;
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
        // create dummy memory regions of cartridge ROM and RAM
        out["ROM0"] = Memory_range {"ROM0", std::vector<uint8_t>(0x4000)};
        out["ROMX"] = Memory_range {"ROM1", std::vector<uint8_t>(0x4000)};
        out["RAMX"] = Memory_range {"RAM0", std::vector<uint8_t>(0x2000)};
    }
    else // cartridge dependant memory locations
    {
        std::unordered_map<std::string, Memory_range>
                cart {cart_->dump()};
        // concatenate memory map recieved from cartridge to output
        out.insert(cart.begin(), cart.end());
    }
    uint8_t wram_bank = io_[0x70] & 7,
            vram_bank = io_[0x4f] & 1;
    if (wram_bank == 0)
        wram_bank = 1;
    std::vector<uint8_t> wrm0 {wram_.dump(0)};
    std::vector<uint8_t> wrm1 {wram_.dump(wram_bank)};
    std::vector<uint8_t> vrm0 {vram_.dump(vram_bank)};
    out["WRM0"] = {"WRM0", wrm0};
    out["VRMX"] = {"VRM" + std::to_string(vram_bank), vrm0};
    out["WRMX"] = {"WRM" + std::to_string(wram_bank), wrm1};
    std::vector<uint8_t> oam(oam_.begin(), oam_.end());
    std::vector<uint8_t> io(io_.begin(), io_.end());
    std::vector<uint8_t> hram(hram_.begin(), hram_.end());
    hram.push_back(ie_);
    out["OAM"] = {"OAM", oam};
    out["IO"] = {"IO", io};
    out["HRAM"] = {"HRAM", hram};

    // create echo ram of 0xc00-0xdff  (WRM0 and WRMX)
    std::vector<uint8_t> echo_ram = wrm0;
    echo_ram.insert(echo_ram.end(), wrm1.begin(),
                    wrm1.begin() + 0x0e00);

    // add extra areas to the memory map
    out["ECHO"] = {"ECHO", echo_ram};
    out["XXXX"] = {"UNUS", std::vector<uint8_t>(0x60)};


    return out;
}

void Memory::enable_logging(bool b)
{
    logging_ = b;
}

std::vector<Memory_byte> Memory::log()
{
    // clear after returning because log represents only the latest changes
    auto out {std::move(log_)};
    log_.clear();
    return out;
}

std::vector<uint8_t> Memory::dump_sram() const
{
    sram_written_ = false;
    if (cart_)
        return cart_->dump_ram();
    else
        throw std::runtime_error("Unable to dump SRAM: no cartridge is inserted");
}

bool Memory::sram_changed() const
{
    return sram_written_;
}

bool Memory::load_save(const std::string &path)
{
    if (cart_)
        return cart_->load_save(path);
    else
        throw std::runtime_error("Unable to load save: no cartridge is inserted");
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

void Memory::oam_dma_transfer(uint8_t b)
{
    if (b > 0xf1) // only defined for range between 00-f1
        throw Bad_memory("Attempted to write value outside 00-f1"
                         "to DMA register (ff46)\n", __FILE__, __LINE__);
    // copy from XX00-XX9f to oam (fe00-fe9f), where XXh = b
    for (uint8_t i {0}; i < oam_.size(); ++i)
        oam_[i] = read(static_cast<uint16_t>(b << 8 | i));
}

void Memory::vram_dma_transfer(uint8_t b)
{
    hdma_len_ = b & 0x7f;
    // bit 7 is 0: GDMA (instant)
    if ((b & 0x80) == 0)
    {
        // if HDMA isn't active, do a GDMA
        if (!hdma_active_)
            general_dma();
        // if HDMA is active, stop it
        else
        {
            hdma_active_ = false;
        }
    }
    // bit 7 is 1: HDMA
    else
    {
        hdma_active_ = true;
    }
}

void Memory::general_dma()
{
    // copy everything all at once
    for (uint16_t i = 0; i < (hdma_len_ & 0x7f); ++i)
        dma_copy();
    // ff55 reads 0xff when finished
    hdma_len_ = 0xff;
    hdma_active_ = false;
}

// called by PPU during HBLANK, transfer 10h bytes of data
void Memory::hblank_dma()
{
    // only run if active and cpu isn't halted
    if (hdma_active_ && !cpu_.halted())
    {
        // end if no more  bytes to copy
        if ((hdma_len_ & 0x7f) == 0)
            hdma_active_ = false;
        dma_copy();
        --hdma_len_;
    }

}

void Memory::dma_copy()
{
    // HDMA src in e000-ffff maps to a000-bfff
    uint16_t src = (hdma_src_ >= 0xe000) ? hdma_src_-0x4000 : hdma_src_;
    uint16_t dst = hdma_dest_;
    for (uint8_t i = 0; i < 0x10; ++i)
    {
        if (src < 0x8000 || src > 0x9fff)
            write( read(src), dst );
        ++src;
        ++dst;
    }
    hdma_src_ += 0x10;
    hdma_dest_ += 0x10;
}

}


