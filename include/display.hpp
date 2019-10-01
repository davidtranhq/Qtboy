#pragma once

#include <functional>
#include <SDL.h>

namespace gameboy
{

class Display
{
	public:
	explicit Display(std::function<uint8_t(uint16_t)> rd, 
		std::function<void(uint8_t, uint16_t)> wr);
	
	void draw_frame();
	
	private:
	std::function<uint8_t(uint16_t)> read;
	std::function<void(uint8_t, uint16_t)> write;
	
	SDL_Window *window_ {nullptr};
	SDL_Renderer *renderer_ {nullptr};
	
};
	
}