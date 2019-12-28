#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "ppu.hpp"

namespace gameboy
{

class Renderer
{
    public:
    virtual void draw_scanline() = 0;
    virtual void draw_framebuffer(Ppu::Frame_data) = 0;
    virtual void present_screen() = 0;
};

}

#endif // RENDERER_HPP
