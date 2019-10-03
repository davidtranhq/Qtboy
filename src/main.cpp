#include "system.hpp"
#include "exception.hpp"

#include <iostream>
#include <sstream>

using namespace gameboy;

int main(int argc, char *argv[])
{	
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		SDL_Log("Could not initialize SDL! SDL: %s", SDL_GetError());
		return -1;
	}
	std::cout << "Path of ROM to load: \n";
	std::string rom;
	std::cin >> rom;
	try
	{
		System sys;
		sys.load_cartridge(rom);
		sys.run();
	}
	catch (const gameboy::Exception &e)
	{
		std::ostringstream err {};
		std::ofstream log {"log.txt"};
		err << argv[0] << ": error in file " << e.file() << ':' << e.line() << ":\n"
			<< e.what() << '\n';
		log << err.str();
		std::cout << err.str();
		return -1;
	}
	catch (const std::exception &e)
	{
		std::ostringstream err {};
		std::ofstream log {"log.txt"};
		err << argv[0] << ": error:\n"
			<< e.what() << '\n';
		log << err.str();
		std::cout << err.str();
		return -1;
	}
	return 0;
}