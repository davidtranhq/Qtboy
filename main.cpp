#include "processor.hpp"
#include "system.hpp"

#include <istream>

using namespace gameboy;

int main()
{	
	System sys;
	sys.load_cartridge("roms/tetris.gb");
	sys.run();
	return 0;
}