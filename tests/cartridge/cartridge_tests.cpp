#include <iostream>
#include <fstream>

#include "cartridge.hpp"

int main()
{
	std::ifstream is {"../roms/tetris.gb", std::ios::binary};
	if (!is.is_open())
	{
		std::cerr << "Could not open ROM!\n";
		return -1;
	}
	gameboy::Cartridge cart {is};
	cart.dump(std::cout);
	std::cout << std::hex << static_cast<int>(cart.read(0x3fbf)) << '\n'; // should be 52
	cart.write(0xFF, 0x3fbf); // should be an error, TETRIS cartridge has no RAM
	return 0;
}