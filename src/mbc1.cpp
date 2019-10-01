#include "Memory_bank_controller.hpp"

#include "Rom.hpp"
#include "Ram.hpp"

namespace gameboy
{

Mbc1::Mbc1(Rom &rom, Ram &ram)
	: rom_ {rom}, ram_ {ram}
{}

uint8_t Mbc1::read(uint16_t adr) const
{
	uint8_t b {0xff};
	if (adr < 0x4000)
	{
		b = rom_.read(0, adr);
	}
	else if (adr < 0x8000)
	{
		b = rom_.read(rom_bank_, adr - 0x8000);
	}
	else if (adr < 0xc000)
	{
		b = ram_.read(ram_bank_, adr - 0xc000);
	}
	return b;
}
	
void Mbc1::write(uint8_t b, uint16_t adr)
{
	if (adr < 0x2000)
	{
		ram_enable_ = ((b & 0x0011) == 0x000a);
	}
	else if (adr < 0x4000)
	{
		rom_bank_ &= ~0x1f; // clear low 5 bits
		rom_bank_ |= (b & 0x1f); // set low 5 bits with b
	}
	else if (adr < 0x6000)
	{
		rom_bank_ &= ~(3 << 6); // clear hi 2 bits
		rom_bank_ &= (b & (3 << 6)); // set hi 2 bits
	}
	else if (adr < 0x8000)
	{
		ram_mode_select_ = b;
	}
	else if (adr >= 0xa000 && adr < 0xc000)
	{
		ram_.write(b, ram_bank_, adr - 0xa000);
	}
}
	
}