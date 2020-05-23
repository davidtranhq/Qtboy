#include "memory_bank_controller.hpp"

#include "rom.hpp"
#include "ram.hpp"
#include "exception.hpp"

namespace gameboy
{

Mbc1::Mbc1(Rom *rom, std::optional<External_ram> *ram)
    : rom_ {rom}, ram_ {ram}
{}

uint8_t Mbc1::read(uint16_t adr) const
{
	uint8_t b {0xff};
	if (adr < 0x4000)
	{
        b = rom_->read(0, adr);
	}
	else if (adr < 0x8000)
	{
        b = rom_->read(rom_bank_, adr - 0x4000);
	}
	else if (adr < 0xc000)
    {
        if (ram_ && ram_enable_)
            b = ram_->value().read(ram_bank_, adr - 0xc000);
        else
            b = 0xff;
    }
	return b;
}
	
void Mbc1::write(uint8_t b, uint16_t adr)
{
    if (adr < 0x2000)
    {
        // 00h = disable RAM, 0ah == enable RAM
        ram_enable_ = ((b & 0xa) == 0xa);
    }
    else if (adr < 0x4000)
    {
        rom_bank_ = (b & 0x1f); // set low 5 bits with b
        adjust_rom_bank(); // ROM bank can't be 00h, 20h, 40h or 60h
    }
    else if (adr < 0x6000)
    {
        if (ram_mode_select_)
        {
            // if in RAM banking mode, use b as the ram bank number
            ram_bank_ = (b & 3); // only banks 00-03 are addressable
            // only ROM banks 00-1f can be selected (hi 3 bits cleared)
            rom_bank_ &= ~0xe0;
            adjust_rom_bank();
        }
        else
        {
            // if in ROM banking mode, use b as the hi 2 bits of the rom bank
            // number
            rom_bank_ &= ~(3 << 6); // clear hi 2 bits
            rom_bank_ &= (b & (3 << 6)); // set hi 2 bits
            adjust_rom_bank(); // ROM bank can't be 00h, 20h, 40h or 60h
            // only RAM bank 00 can be selected
            ram_bank_ = 0;
        }
    }
    else if (adr < 0x8000)
    {
        ram_mode_select_ = b;
    }
    else if (adr >= 0xa000 && adr < 0xc000)
    {
        if (ram_ && ram_enable_)
            ram_->value().write(b, ram_bank_, adr - 0xa000);
    }
}

void Mbc1::adjust_rom_bank()
{
    // ROM banks 00, 20h, 40h, and 60h are not accessible. They convert
    // to 01h, 21h, 41h, and 61h instead
    switch (rom_bank_)
    {
        case 0x00:
        case 0x20:
        case 0x40:
        case 0x60:
            ++rom_bank_;
    }
}

}
