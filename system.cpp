#include <istream>
#include <fstream>

#include "system.hpp"

namespace gameboy
{

System::System()
{}

void System::run()
{
	for (;;)
	{
		cpu_.dump(std::cout);
		cpu_.step();
	}
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

}