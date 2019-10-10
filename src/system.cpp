#include <istream>
#include <fstream>
#include <vector>

#include "system.hpp"
#include "exception.hpp"
#include "disassembler.hpp"

namespace gameboy
{

System::System()
{
}

void System::run()
{
    #ifdef DEBUG_BULID
        cpu_.dump(std::cout);
    #endif
    cpu_.step();
}


void System::load_cartridge(std::istream &is)
{
	memory_.load_cartridge(is);
}

void System::load_cartridge(const std::string &path)
{
	std::ifstream rom {path, std::ios::binary};
	if (!rom.good())
		throw std::runtime_error {"Invalid ROM path!\n"};
	memory_.load_cartridge(rom);
}

std::string System::disassemble() const
{
    return disassembler_.disassemble(memory_.dump_rom());
}

uint8_t System::memory_read(uint16_t adr)
{
	return memory_.read(adr);
}

void System::memory_write(uint8_t b, uint16_t adr)
{
	memory_.write(b, adr);
}

}
