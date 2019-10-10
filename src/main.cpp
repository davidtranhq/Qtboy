#include "system.hpp"

#include <iostream>

using namespace gameboy;

int main(int argc, char *argv[])
{
	System system;
	std::cout << system.disassemble("roms/tetris.gb");
	return 0;
}