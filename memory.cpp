#include "memory.hpp"

#include <cstdint>

namespace gameboy
{
	
uint8_t Memory::read(uint16_t adr) const
{
	uint8_t b;
	
	if (adr < 0x8000) // ROM bank accessing
	{
		b = cart_.value().read(adr); // MBC on cartridge
	}
	else if (adr < 0xa000) // VRAM accessing
	{
		uint16_t a = adr - 0x8000; // adjusted for placement in memory map
		if (!cart_.value().is_cgb()) // no banking for DMG
			b = vram_.read(0, a);
		else // CGB, check 0xff4f flag for vram bank
			b = vbk_ ? vram_.read(1, a) : vram_.read(0, a);
	}
	else if (adr < 0xc000) // external RAM accessing
	{
		b = cart_.value().read(adr); // MBC on cartridge
	}
	else if (adr < 0xd000) // WRAM bank 0 access
	{
		b = wram_.read(0, adr-0xc000);
	}
	else if (adr < 0xe000) // WRAM bank 1-7 accessing (CGB)
	{
		uint16_t a = adr - 0xd000; // adjust
		if (!cart_.value().is_cgb()) // no WRAM banking for DMG
			b = wram_.read(1, a);
		else // CGB, check 0xff70 (svbk) for bank 1-7
			// bit 0-2 of svbk selects bank 1-7, svbk of 0 is bank 1
			b = svbk_ ? wram_.read(svbk_ & 3, a) : wram_.read(1, a);
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

void Memory::write(uint8_t b, uint16_t adr)
{
	if (adr < 0x8000) // enabling flags (dependant on MBC)
	{
		cart_.value().write(b, adr);
	}
	else if (adr < 0xa000) // VRAM accessing
	{
		uint16_t a = adr - 0x8000; // adjusted for placement in memory map
		if (!cart_.value().is_cgb()) // no banking for DMG
			b = vram_.read(0, a);
		else // CGB, check 0xff4f flag for vram bank
			b = vbk_ ? vram_.read(1, a) : vram_.read(1, a);
	}
	else if (adr < 0xc000) // RAM bank access
		cart_.value().write(b, adr); // MBC on cartridge
}


void Memory::load_cartridge(std::istream &is)
{
	cart_ = Cartridge {is};
}

void Memory::set_ram_size()
{
	bool is_cgb {cart_.value().is_cgb()};
	vram_.resize(is_cgb ? 2 : 1); // CGB has 2 banks of VRAM
	wram_.resize(is_cgb ? 8 : 2); // CGB has 8 banks of WRAM
}

}