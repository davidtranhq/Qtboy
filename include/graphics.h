#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <cstdint>
#include <array>

namespace gameboy
{

struct Color
{
    uint8_t a, r, g, b;
    uint32_t argb() const;
};

using Palette = std::array<Color, 4>;

struct Frame_data
{
    std::array<uint32_t, 256*256> pixels;
};

struct Line_data
{
    std::array<uint32_t, 160> pixels;
    std::array<uint8_t, 160> bg_px_index;
    uint8_t line;
};

struct Tile_data
{
    std::array<uint32_t, 64> pixels;
};

struct Sprite
{
    uint8_t y, x, tile, attr, line;
};

} // namespace gameboy
#endif // GRAPHICS_H
