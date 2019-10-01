#include "display.hpp"
#include "exception.hpp"
#include "tile.hpp"

#include <functional>
#include <array>
#include <SDL.h>

namespace gameboy
{
	
Display::Display(std::function<uint8_t(uint16_t)> rd, 
		std::function<void(uint8_t, uint16_t)> wr)
	: read {std::move(rd)}, write {std::move(wr)}
{
	SDL_CreateWindowAndRenderer(256, 256, SDL_WINDOW_RESIZABLE, &window_, &renderer_);
	if (window_ == nullptr)
		throw Bad_display {"Could not create SDL_window!", __FILE__, __LINE__};
	if (renderer_ == nullptr)
		throw Bad_display {"Could not create SDL_renderer!", __FILE__, __LINE__};
}

void Display::draw_frame()
{
	// draw background 32x32 tiles
	for (uint8_t col {0}; col < 32; ++col)
	{
		for (uint8_t row {0}; row < 32; ++row)
		{
			uint8_t tile_n = read(0x9800+col*32+row); // 0x9800: start of Background Tile Table
			std::array<uint8_t, 16> bmp {};
			for (uint8_t i {0}; i < bmp.size(); ++i)
			{
				// 0x8000: start of tile pattern table
				bmp[i] = read(0x8000+tile_n*16+i);
			}
			Tile t {bmp, renderer_};
			const SDL_Rect dst {row*16, col*16, 8, 8};
			SDL_RenderCopy(renderer_, t.texture(), nullptr, &dst);
		}
	}
	SDL_RenderPresent(renderer_);
}

}