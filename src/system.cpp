#include <istream>
#include <fstream>
#include <SDL.h>

#include "system.hpp"
#include "exception.hpp"

namespace gameboy
{

System::System()
{
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		throw Bad_SDL {SDL_GetError(), __FILE__, __LINE__};
}

void System::run()
{
	SDL_Event e;
	while (!quit_)
	{
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
				quit_ = true;
		}
		cpu_.dump(std::cout);
		cpu_.step();
		display_.draw_frame();
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

uint8_t System::memory_read(uint16_t adr)
{
	return memory_.read(adr);
}

void System::memory_write(uint8_t b, uint16_t adr)
{
	memory_.write(b, adr);
}

}