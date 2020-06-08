#include "memory_bank_controller.hpp"

#include "rom.hpp"
#include "ram.hpp"
#include "exception.hpp"

namespace gameboy
{

Mbc2::Mbc2(Rom *rom)
    : rom_ {rom},
      ram_(512)
{}

uint8_t Mbc2::read(uint16_t adr) const
{
    uint8_t b {0xff};
    if (adr < 0x4000) // ROM Bank 00
    {
        b = rom_->read(0, adr);
    }
    else if (adr < 0x8000) // ROM bank 01-0f
    {
        b = rom_->read(rom_bank_, adr - 0x4000);
    }
    else if (adr < 0xa000)
    {
        // nothing defined here
    }
    else if (adr < 0xb000) // 512x4 bits built-in RAM
    {
        if (ram_enable_)
            // only the lower 4-bits of each byte is usable
            b = ram_[adr-0xa000] & 0x0f;
    }
    return b;
}

void Mbc2::write(uint8_t b, uint16_t adr)
{
    if (adr < 0x2000) // RAM enable
    {
        // lsb of the upper address byte must be 0 to toggle RAM (bit 8)
        if ((adr & 0x100) == 0)
            ram_enable_ = b;
    }
    else if (adr < 0x4000) // ROM bank #
    {
        // lsb of the upper address byte must be 1 to select ROM bank
        if (adr & 0x100)
        {
            // only banks 01-0f are selectable
            rom_bank_ = b ? (b & 0x0f) : 1;
        }

    }
    else if (adr < 0xa000)
    {
        // nothing defined here
    }
    else if (adr < 0xb000) // 512x4 bits built-in RAM
    {
        if (ram_enable_)
            // only the lower 4-bits of each byte is usable
            ram_[adr-0xa000] = b & 0x0f;
    }
}

void Mbc2::load_sram(const std::vector<uint8_t> &sram)
{
    ram_ = sram;
    ram_.resize(512); // crop to 512 bytes, size of RAM on MBC2 carts
}

std::vector<uint8_t> Mbc2::dump_ram() const
{
    return ram_;
}

}
