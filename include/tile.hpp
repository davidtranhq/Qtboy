#pragma once

#include <SDL.h>
#include <array>
#include <cstdint>

namespace gameboy
{
	
class Tile
{
	public:
	explicit Tile(const std::array<uint8_t, 16> &bmp, SDL_Renderer *r);
	~Tile();
	
	SDL_Texture *texture() const noexcept;
	
	private:
	SDL_Texture *texture_ {nullptr};
};

inline SDL_Texture *Tile::texture() const noexcept
{
	return texture_;
}

}