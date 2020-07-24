#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "ppu.hpp"

namespace gameboy
{

class Texture;

class Renderer
{
    public:
    virtual void draw_texture(const Texture &t, unsigned x, unsigned y) = 0;
    virtual void present_screen() = 0;
};

}

#endif // RENDERER_HPP
