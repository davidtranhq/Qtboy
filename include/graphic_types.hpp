#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <array>
#include <vector>

namespace gameboy
{

using Color = uint16_t;

using Palette = std::array<Color, 4>;

class Texture
{
    public:
    Texture(unsigned w = 1, unsigned h = 1);

    void fill(const Color &c);
    void set_pixel(size_t i, const Color &c);
    Color pixel(size_t i) const;
    void set_pixel_priority(size_t i, uint8_t x);
    uint8_t pixel_priority(size_t i) const;
    unsigned width() const;
    unsigned height() const;

    private:
    unsigned w_, h_;
    std::vector<Color> data_;
    std::vector<uint8_t> indices_;
};


struct Sprite
{
    uint8_t y, x, tile, attr, line, id;
};

} // namespace gameboy
#endif // GRAPHICS_H
