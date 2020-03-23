#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "ppu.hpp"

namespace gameboy
{

class Renderer
{
    public:
    virtual void draw_tile(Tile_data, size_t x, size_t y) = 0;
    virtual void draw_scanline(Line_data) = 0;
    virtual void draw_framebuffer(Frame_data) = 0;
    virtual void present_screen() = 0;
};

}

#endif // RENDERER_HPP
