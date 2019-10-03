#include <istream>
#include <fstream>
#include <SDL.h>

#include "system.hpp"
#include "exception.hpp"

namespace gameboy
{

System::System()
{
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
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
					case SDLK_t:
						cpu_.dump(std::cout);
						break;
				}
			}
		}
		#ifdef DEBUG_BULID
			cpu_.dump(std::cout);
		#endif
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