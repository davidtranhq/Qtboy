#include "system.hpp"

#include <istream>

using namespace gameboy;

int main(int argc, char *argv[])
{	
	System sys;
	sys.load_cartridge("roms/tetris.gb");
	sys.run();
	return 0;
}