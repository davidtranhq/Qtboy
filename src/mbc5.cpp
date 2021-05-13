#include "memory_bank_controller.hpp"

#include "rom.hpp"
#include "ram.hpp"
#include "exception.hpp"

namespace qtboy
{

Mbc5::Mbc5(Rom *rom, std::optional<External_ram> *ram)
    : rom_ {rom}, ram_ {ram}
{}

uint8_t Mbc5::read(uint16_t adr) const
{
    uint8_t b {0xff};
    if (adr < 0x4000) // ROM Bank 00
    {
        b = rom_->read(0, adr);
    }
    else if (adr < 0x8000) // ROM bank 00-1ff
    {
        b = rom_->read(rom_bank_, adr - 0x4000);
    }
    else if (adr < 0xa000)
    {
        // nothing defined here
    }
    else if (adr < 0xc000) // RAM Bank 00-0f
    {
        if (ram_ && ram_enable_)
            b = ram_->value().read(ram_bank_, adr - 0xa000);
        else
            b = 0xff;
    }
    return b;
}

void Mbc5::write(uint8_t b, uint16_t adr)
{
    if (adr < 0x2000) // RAM enable
    {
        // 00h = disable RAM, 0ah == enable RAM
        ram_enable_ = ((b & 0xa) == 0xa);
    }
    else if (adr < 0x3000) // Low 8 bits of ROM bank # select
    {
        rom_bank_ &= ~0xff;
        rom_bank_ |= b;
    }
    else if (adr < 0x4000) // hi bit of ROM bank # select
    {
        rom_bank_ &= (b ? 0x100 : ~0x100);
    }
    else if (adr < 0x6000) // high bit
    {
        ram_bank_ = (b & 0x0f);
    }
    else if (adr < 0x8000)
    {
        // nothing defined here
    }
    else if (adr >= 0xa000 && adr < 0xc000)
    {
        if (ram_ && ram_enable_)
            ram_->value().write(b, ram_bank_, adr - 0xa000);
    }
}

void Mbc5::load_sram(const std::vector<uint8_t> &sram)
{
    ram_->value().load(sram);
}

std::vector<uint8_t> Mbc5::dump_ram() const
{
    return ram_->value().dump();
}


}
