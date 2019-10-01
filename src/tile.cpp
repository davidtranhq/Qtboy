#include "tile.hpp"

#include <SDL.h>
#include <cstdint>
#include <cmath>
#include <array>

namespace gameboy
{
	
Tile::Tile(const std::array<uint8_t, 16> &bmp, SDL_Renderer *r)
	: texture_ {SDL_CreateTexture(r, SDL_PIXELFORMAT_RGB888, 
		SDL_TEXTUREACCESS_TARGET, 8, 8)}
{
	std::array<uint8_t, 64> pixels {};
	// tile stored with 16 bytes, 2 bytes per line
	uint8_t i {0};
	for (uint8_t byte {0}; byte < 16; byte += 2)
	{
		// 2-bit value for each pixel (grayscale)
		for (uint8_t bit {0}; bit < 8; ++bit)
		{
			bool b1 {bmp[byte] & (1 << (7-bit))};
			bool b2 {bmp[byte+1] & (1 << (7-bit))};
			pixels[i++] = b2 << 1 | b1;
		}
	}
	SDL_SetRenderTarget(r, texture_);
	for (uint8_t p {0}; p < pixels.size(); ++p)
	{
		switch (pixels[p])
		{
			case 0:
			SDL_SetRenderDrawColor(r, 0, 0, 0, SDL_ALPHA_OPAQUE);
			break;
			case 1:
			SDL_SetRenderDrawColor(r, 85, 85, 85, SDL_ALPHA_OPAQUE);
			break;
			case 2:
			SDL_SetRenderDrawColor(r, 170, 170, 170, SDL_ALPHA_OPAQUE);
			break;
			case 3:
			SDL_SetRenderDrawColor(r, 255, 255, 255, SDL_ALPHA_OPAQUE);
		}
		SDL_RenderDrawPoint(r, p%8, std::floor(p/8));
	}
	SDL_SetRenderTarget(r, nullptr);
}

Tile::~Tile()
{
	SDL_DestroyTexture(texture_);
}
	
}