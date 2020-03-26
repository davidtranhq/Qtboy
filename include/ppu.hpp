#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>
#include <functional>
#include <bitset>
#include <vector>

#include "graphics.hpp"

namespace gameboy
{

class Renderer;
class Processor;

class Ppu
{
    public:

    enum class Layer {Background, Window, Sprite};

    Ppu(std::function<uint8_t(uint16_t)> rd,
        std::function<void(uint8_t, uint16_t)> wr,
        Processor &p,
        Renderer *r = nullptr);
    void reset();
    void step(size_t cycles);
    int mode() const;
    int clock() const;
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);

    void set_renderer(Renderer *r);
    void render_framebuffer();
    Texture get_tile(uint16_t i);
    std::array<Sprite, 40> get_sprites();
    Texture get_layer(Layer);
    Texture get_sprite_layer();
    std::array<uint8_t, 32*32> get_raw_background();
    void render_sprites();

    private:
    std::function<uint8_t(uint16_t)> read;
    std::function<void(uint8_t, uint16_t)> write;
    void render_scanline();
    void render_layer_line(Texture &ld, Layer l);
    void render_sprite_line(Texture &ld);
    Palette get_bg_palette();
    Palette get_sprite_palette(bool alt_pal);
    void load_sprites();
    void oam_scan(); // mode 2
    void vram_read(); // mode 3
    void hblank(); // mode 0
    void vblank(); // mode 1

    void increment_ly();

    private:
    Processor &cpu_;
    Renderer *renderer_;
    int clock_ {0};
    uint8_t lcdc_ {0x91}, stat_ {0x85}; // ff40, ff41
    uint8_t scy_ {0}, scx_ {0}; // ff42, ff43
    uint8_t ly_ {0}, lyc_ {0}; // ff44, ff45
    uint8_t bgp_ {0xfc}, obp0_ {0xff}, obp1_ {0xff}; // ff47-ff49
    uint8_t wy_ {0}, wx_ {0}; // ff4a, ff4b
    std::array<Sprite, 40> sprites_ {};

};

}

#endif // PPU_HPP
