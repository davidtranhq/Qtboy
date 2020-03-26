#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "ppu.hpp"

namespace gameboy
{

class Texture;

class Renderer
{
    public:
    virtual void draw_texture(const Texture &t, int x, int y) = 0;
    virtual void present_screen() = 0;
};

}

#endif // RENDERER_HPP
