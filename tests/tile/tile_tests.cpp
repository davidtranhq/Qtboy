#include <SDL.h>

#include "tile.hpp"

using namespace gameboy;

int main(int argc, char *argv[])
{
	std::array<uint8_t, 16> bmp
	{
		0x7c, 0x7c, 0x00, 0xc6, 0xc6, 0x00, 0x00, 0xfe, 0xc6, 0xc6, 0x00,
		0xc6, 0xc6, 0x00, 0x00, 0x00
	};
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return 1;
	}
	SDL_Renderer *r {nullptr};
	SDL_Window *w {nullptr};
	if (SDL_CreateWindowAndRenderer(480, 480, SDL_WINDOW_RESIZABLE, &w, &r) != 0)
	{
		SDL_Log("Unable to create window and renderer! SDL: %s", SDL_GetError());
		return 2;
	}
	Tile t {bmp, r};
	SDL_RenderCopy(r, t.texture(), nullptr, nullptr);
	SDL_RenderPresent(r);
	SDL_Delay(10000);
	SDL_Quit();
	return 0;
}