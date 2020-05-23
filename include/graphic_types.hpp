#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <array>
#include <vector>

namespace gameboy
{

struct Color
{
    uint8_t r, g, b;
    uint32_t argb() const { return 0xff000000 | r << 16 | g << 8 | b; }
};

using Palette = std::array<Color, 4>;

class Texture
{
    public:
    Texture(unsigned w = 1, unsigned h = 1);

    void set_pixel(size_t i, Color c);
    Color pixel(size_t i) const;
    void set_pixel_index(size_t i, int x);
    int pixel_index(size_t i) const;
    unsigned width() const;
    unsigned height() const;

    private:
    unsigned w_, h_;
    std::vector<Color> data_;
    std::vector<int> indices_;
};


struct Sprite
{
    uint8_t y, x, tile, attr, line;
};

} // namespace gameboy
#endif // GRAPHICS_H
