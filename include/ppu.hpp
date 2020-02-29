#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>
#include <memory>
#include <functional>
#include <bitset>

namespace gameboy
{

class Renderer;

class Ppu
{
    public:

    struct Color { uint8_t r, g, b; };
    struct Frame_data
    {
        std::array<uint8_t, 256> pixels;
        std::array<Color, 4> palette;
    };

    Ppu(std::function<uint8_t(uint16_t)> rd,
        std::function<void(uint8_t, uint16_t)> wr,
        Renderer *r = nullptr);
    void reset();
    void step(size_t cycles);
    void draw_framebuffer();
    void draw_scanline();
    void set_renderer(Renderer *r);
    int mode() const;

    private:
    std::function<uint8_t(uint16_t)> read;
    std::function<void(uint8_t, uint16_t)> write;
    void oam_scan(); // mode 2
    void vram_read(); // mode 3
    void hblank(); // mode 0
    void vblank(); // mode 1

    uint8_t mode_ {2};
    int clock_ {0};
    uint8_t line_ {0};
    std::bitset<8> lcdc_ {0};
    Renderer *renderer_;
};

}

#endif // PPU_HPP
