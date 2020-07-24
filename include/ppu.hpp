#ifndef PPU_HPP
#define PPU_HPP

#include <cstdint>
#include <functional>
#include <bitset>
#include <vector>

#include "graphic_types.hpp"

namespace gameboy
{

class Renderer;
class Processor;
class Memory;

class Ppu
{
    public:

    struct Dump
    {
        uint8_t lcdc {0x90}, stat {0x00}; // ff40, ff41
        uint8_t scy {0}, scx {0}; // ff42, ff43
        uint8_t ly {0}, lyc {0}; // ff44, ff45
        uint8_t bgp {0xfc}, obp0 {0xff}, obp1 {0xff}; // ff47-ff49
        uint8_t wy {0}, wx {0}; // ff4a, ff4b
        uint8_t bgpi {0}; // ff68
        std::array<uint8_t, 0x40> bgpd {}; // background palette data
        uint8_t obpi {0}; // ff6a
        std::array<uint8_t, 0x40> obpd {}; // object palette data
    };

    enum class Layer { Background, Window, Sprite};
    enum class Color_correction { None, Fast, Proper };

    Ppu(Memory &m,
        Processor &p,
        Renderer *r = nullptr);
    void reset();
    void enable_cgb(bool is_cgb);
    void step(size_t cycles);
    int mode() const;
    int clock() const;
    bool enabled() const;
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    void set_renderer(Renderer *r);

    // debug
    Palette get_bg_palette(uint8_t idx) const;
    Palette get_sprite_palette(uint8_t idx) const;
    Color color_correct(const Color &c) const;
    Texture get_framebuffer(bool bg = true, bool window = true,
                            bool sprites = true) const;
    Texture get_tile(uint8_t bank, uint16_t i) const;
    std::array<Texture, 40> get_sprites() const;
    Texture get_layer(Layer) const;
    Texture get_sprite_layer() const;
    std::array<uint8_t, 32*32> get_raw_background();
    Dump dump_values() const;

    private:
    uint8_t read(uint16_t adr) const;
    void write(uint8_t b, uint16_t adr);
    uint8_t read_vram(uint8_t bank, uint16_t adr) const;
    void write_vram(uint8_t b, uint8_t bank, uint16_t adr);
    void render_scanline();
    void render_layer_line(Texture &tex, Layer l);
    // (x,y): coordinate in VRAM tilemap to get pixel from
    // (tex_x, tex_y): pixel to draw in Texture
    void render_layer_pixel(Texture &tex, Layer l, uint8_t x, uint8_t y,
                            uint8_t tex_x, uint8_t tex_y) const;
    void render_sprite_line(Texture &tex);
    void order_sprites(std::array<Sprite, 10> &s) const;
    void load_sprites();
    void oam_scan(); // mode 2
    void vram_read(); // mode 3
    void hblank(); // mode 0
    void vblank(); // mode 1
    void check_stat();

    public: // options
    Color_correction color_correction {Color_correction::Fast};
    float brightness;

    private:
    Memory &memory_;
    Processor &cpu_;
    Renderer *renderer_;
    int clock_ {0};
    uint8_t window_line_ {0}; // keep track of how many window lines were drawn
    uint8_t lcdc_ {0x90}, stat_ {0x00}; // ff40, ff41
    uint8_t scy_ {0}, scx_ {0}; // ff42, ff43
    uint8_t ly_ {0}, lyc_ {0}; // ff44, ff45
    uint8_t bgp_ {0xfc}, obp0_ {0xff}, obp1_ {0xff}; // ff47-ff49
    uint8_t wy_ {0}, wx_ {0}; // ff4a, ff4b
    std::array<Sprite, 40> sprites_ {};
    bool stat_signal_ {false}; // for activating STAT interrupt
    static Palette dmg_palette;

    // CGB registers
    bool cgb_mode_ {false};
    std::array<uint8_t, 0x40> bgpd_ {}; // background palette data
    std::array<uint8_t, 0x40> obpd_ {}; // object palette data
    uint8_t bgpi_ {0}; // ff68
    uint8_t obpi_ {0}; // ff6a



};

}

#endif // PPU_HPP
