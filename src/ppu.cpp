#include "ppu.hpp"
#include "renderer.hpp"
#include "exception.hpp"
#include "processor.hpp"
#include "exception.hpp"

#include <iostream>
#include <string>

#define CHANGE_BIT(b, n, x) b ^= (-x ^ b) & (1UL << n)
#define CLEAR_BIT(b, n) b &= ~(1UL << n)
#define SET_BIT(b, n) b |= (1UL << n)

using namespace gameboy;

Ppu::Ppu(std::function<uint8_t(uint16_t)> rd,
         std::function<void(uint8_t, uint16_t)> wr,
         Processor &p,
         Renderer *r)
    : read {std::move(rd)},
      write {std::move(wr)},
      cpu_ {p},
      renderer_ {r}
{}

void Ppu::reset()
{
    clock_ = 0;
    lcdc_ = 0x91;
    stat_ = 0x85;
    scy_ = 0;
    scx_ = 0;
    ly_ = 0;
    lyc_ = 0;
    bgp_ = 0xfc;
    obp0_ = 0xff;
    obp1_ = 0xff;
    wy_ = 0;
    wx_ = 0;
    sprites_ = {};
}

void Ppu::step(size_t cycles)
{
    if (!(lcdc_ & 0x80)) // bit 7
    {
        ly_ = 0;
        return; // don't execute if master bit is off
    }
    // PPU operates on 4.194 MHz clock, 1 clock = 1/4 cycle
    clock_ += cycles;
    switch (stat_ & 3) // bit 0-1
    {
        // mode 2: scan for OAM sprites
        case 2:
            oam_scan();
            break;
        // OAM/VRAM read
        // end of mode 3 = end of scan line
        case 3:
            vram_read();
            break;
        // HBLANK
        case 0:
            hblank();
            break;
        // VBLANK
        case 1:
            vblank();
            break;
    }
}

int Ppu::mode() const
{
    return stat_ & 3;
}

int Ppu::clock() const
{
    return clock_;
}

uint8_t Ppu::read_reg(uint16_t adr)
{
    std::array<uint8_t, 12> regs
    {{
        lcdc_, stat_, scy_, scx_, ly_, lyc_, 0x00, // ff46 is DMA transfer
        bgp_, obp0_, obp1_, wy_, wx_
    }};
    if (!(lcdc_ & 0x80))
        return 0; // always return 0 when LCD is off
    if (adr > 0xff39 && adr < 0xff4c && adr != 0xff46)
        return regs[adr-0xff40];
    else
        throw gameboy::Exception("Attempted to read invalid PPU register.",
                                 __FILE__, __LINE__);
}

void Ppu::write_reg(uint8_t b, uint16_t adr)
{
    std::array<uint8_t *, 12> regs
    {{
        &lcdc_, &stat_, &scy_, &scx_, &ly_, &lyc_, nullptr, // ff46 is DMA transfer
        &bgp_, &obp0_, &obp1_, &wy_, &wx_
    }};
    if (adr > 0xff3f && adr < 0xff4c && adr != 0xff46)
        *regs[adr-0xff40] = b;
    else
        throw gameboy::Exception("Attempted to write invalid PPU register",
                                  __FILE__, __LINE__);
}


void Ppu::set_renderer(Renderer *r)
{
    renderer_ = r;
}

/*
void Ppu::render_framebuffer()
{
    if (!renderer_)
        return;
    uint8_t lcdc {read(0xff40)};
    // bit 4 of LCDC:
    // 0 uses tileset from 8800 to 97FF with signed addressing,
    // 1 uses tileset from 8000 to 8fff with unsigned addressing
    uint16_t tile_base = (lcdc & (1 << 4) ? 0x8000 : 0x9000);
    // bit 3 and 6 of LCDC
    // 0: tile map used for bg/window starts at 0x9800
    // 1: tile map used for bg/window starts at 0x9c00
    uint16_t bg_map = ((lcdc & (1 << 3)) ? 0x9c00 : 0x9800);
    uint16_t window_map = (lcdc & (1 << 6) ? 0x9c00 : 0x9800);

    Frame_data fd {};
    const uint16_t MAP_SIZE {0x400};
    for (uint16_t p {0}; p < MAP_SIZE; ++p)
    {
        uint8_t i {read(bg_map+p)};
        if (tile_base == 0x9000) // signed addressing
        {
            int8_t is {static_cast<int8_t>(i)};
            int16_t base {static_cast<int16_t>(tile_base)};
            uint16_t index {static_cast<uint16_t>(base+is)};
            fd.pixels[p] = read(index);
        }
        else // unsigned addressing
        {
            fd.pixels[p] = read(tile_base+i);
        }
    }

    uint8_t pal {read(0xff47)};
    std::array<Color, 4> shades
    {{
        {255, 255, 255},
        {192, 102, 192},
        {96, 96, 96},
        {0, 0, 0}
    }};
    for (uint8_t i {0}; i < 4; ++i)
        fd.palette[i] = shades[((pal >> i*2) & 3)];
    renderer_->draw_framebuffer(fd);
}
*/

Texture Ppu::get_tile(uint16_t i)
{
    Texture tex {8,8};
    uint16_t tile_base = 0x8000;
    std::array<Color, 4> pal
    {{
        {255, 255, 255},
        {192, 102, 192},
        {96, 96, 96},
        {0, 0, 0}
    }};
    for (uint8_t byte = 0; byte < 16; byte += 2)
    {
        uint8_t lo_byte = read(byte+tile_base+i*16);
        uint8_t hi_byte = read(byte+tile_base+i*16+1);
        for (uint8_t px = 0; px < 8; ++px)
        {
            bool hi_bit = hi_byte & 1 << (7-px);
            bool lo_bit = lo_byte & 1 << (7-px);
            Color c = pal[static_cast<uint8_t>(hi_bit << 1 | lo_bit)];
            tex.set_pixel(byte/2+px, c);
        }
    }
    return tex;
}

Texture Ppu::get_layer(Layer l)
{
    uint16_t tilemap;
    if (l == Layer::Background)
        tilemap = ((lcdc_ & (1 << 3)) ? 0x9c00 : 0x9800);
    else if (l == Layer::Window)
        tilemap = ((lcdc_ & (1 << 6)) ? 0x9c00 : 0x9800);
    else if (l == Layer::Sprite)
        return get_sprite_layer();
    Texture tex {256, 256};
    Palette pal {get_bg_palette()};
    // tile data offset in VRAM
    uint16_t tile_base = (lcdc_ & (1 << 4) ? 0x8000 : 0x9000);
    uint16_t i = 0;
    for (uint16_t y_px = 0; y_px < 256; ++y_px)
    {
        uint8_t tile_y = static_cast<uint8_t>(y_px >> 3); // y-index of tile
        for (uint16_t x_px = 0; x_px < 256; x_px += 8)
        {
            uint8_t tile_x = static_cast<uint8_t>(x_px >> 3); // x-index of tile
            uint16_t tile_i = tile_x+(tile_y*32);
            uint16_t adr = 0;
            if (tile_base == 0x9000) // 0x9000 base uses signed addressing
            {
                int8_t tile_data_i = static_cast<int8_t>(read(tilemap+tile_i));
                adr = static_cast<uint16_t>(tile_base+tile_data_i*16); // tile is 16 bytes
            }
            else // unsigned addressing
            {
                uint8_t tile_data_i = read(tilemap+tile_i); // index into tile data
                adr = tile_base+tile_data_i*16;
            }
            adr += (y_px%8)*2; // select the right byte pair in the tile
            uint8_t lo_byte = read(adr);
            uint8_t hi_byte = read(adr+1);
            for (uint8_t p = 0; p < 8; ++p)
            {
                bool hi_bit = hi_byte & 1 << (7-p);
                bool lo_bit = lo_byte & 1 << (7-p);
                Color px = pal[static_cast<uint8_t>(hi_bit << 1 | lo_bit)];
                tex.set_pixel(i++, px);
            }
        }
    }
    return tex;
}

Texture Ppu::get_sprite_layer()
{
    return Texture {256,256};
}

std::array<uint8_t, 32*32> Ppu::get_raw_background()
{
    uint16_t bg_map = ((lcdc_ & (1 << 3)) ? 0x9c00 : 0x9800);
    std::array<uint8_t, 32*32> raw {};
    for (int i = 0; i < 32*32; ++i)
        raw[i] = read(bg_map+i);
    return raw;
}

void Ppu::render_scanline()
{
    Texture tex {160, 1};
    if (lcdc_ & 1) // bg/window enable
    {
        render_layer_line(tex, Layer::Background);
        if (lcdc_ & 1 << 5) // window display enable
            render_layer_line(tex, Layer::Window);
    }
    if (lcdc_ & 1 << 1) // OBJ display enable
        render_layer_line(tex, Layer::Sprite);
    renderer_->draw_texture(tex, 0, ly_);
}

void Ppu::render_layer_line(Texture &tex, Layer l)
{
    // tile maps used for bg/window
    uint16_t tile_map = 0;
    uint8_t shift_x = 0, shift_y = 0;
    if (l == Layer::Background)
    {
        tile_map = ((lcdc_ & (1 << 3)) ? 0x9c00 : 0x9800);
        shift_x = scx_;
        shift_y = scy_;
    }
    else if (l == Layer::Window)
    {
        tile_map = ((lcdc_ & (1 << 6)) ? 0x9c00 : 0x9800);
        shift_x = wx_-7;
        shift_y = wy_;
    }
    else if (l == Layer::Sprite)
    {
        render_sprite_line(tex);
        return;
    }
    Palette pal {get_bg_palette()};
    // tile data offset in VRAM
    uint16_t tile_base = (lcdc_ & (1 << 4) ? 0x8000 : 0x9000);
    uint8_t y = ly_ + shift_y; // y -coord in BG
    uint8_t tile_y = y >> 3; // y-index in tile BG map (tiles are 8x8 px)
    // background
    for (uint8_t x_px = 0; x_px < 160; ++x_px)
    {
        uint8_t x = x_px + shift_x; // x-coord in BG map
        uint8_t tile_x = x >> 3; // x-index in BG map (tiles are 8x8 px)
        uint16_t tile_i = (tile_y*32) + tile_x; // index into BG tile map
        uint16_t adr = 0; // location to read tile data
        if (tile_base == 0x9000) // 0x9000 base uses signed addressing
        {
            int8_t tile_data_i = static_cast<int8_t>(read(tile_map+tile_i));
            adr = static_cast<uint16_t>(tile_base+tile_data_i*16); // tile is 16 bytes
        }
        else // unsigned addressing
        {
            uint8_t tile_data_i = read(tile_map+tile_i); // index into tile data
            adr = tile_base+tile_data_i*16;
        }
        adr += (y%8)*2; // select the right byte pair in the tile based on current line and SCY
        // get two bytes (one row of pixels) from tile data
        uint8_t low_byte = read(adr); // tile is 16 bytes
        uint8_t high_byte = read(adr+1);
        uint8_t px_offset = (x % 8); // pixel offset of tile (tiles are 8x8)
        // most significant bits of both bytes represent color of first pixel
        bool hi_bit = (high_byte & 1 << (7-px_offset));
        bool lo_bit = (low_byte & 1 << (7-px_offset));
        uint8_t px_i = static_cast<uint8_t>(hi_bit << 1 | lo_bit);
        tex.set_pixel(x_px, pal[px_i]);
        tex.set_pixel_index(x_px, px_i);
    }
    renderer_->draw_texture(tex, 0, ly_);
}

void Ppu::render_sprite_line(Texture &tex)
{
    const uint16_t tile_data = 0x8000;
    // 0=8x8, 1=8x16
    const uint8_t sprite_h = lcdc_ & 1 << 2 ? 16 : 8;
    uint8_t sprites_drawn = 0;
    for (Sprite s : sprites_)
    {
        if (sprites_drawn == 10)
            break; // can't draw more than 10 sprites per scanline
        uint8_t ln = ly_ - (s.y-16); // line number relative to start of sprite
        if (ln > sprite_h)
            continue; // ignore sprite if no part of it falls on this line
        uint8_t tile_i;
        if (ln > 8) // lower half of an 8x16 sprite
        {
            tile_i = s.tile | 0x01; // ignore lower bit
        }
        else
        {
            if (sprite_h == 16) // upper half of an 8x16 sprite
                tile_i = s.tile & 0xfe;
            else // 8x8 sprite
                tile_i = s.tile;
        }
        bool y_flip = s.attr & 1 << 6;
        uint16_t adr = y_flip
                ? tile_data+(tile_i+1)*16-(ln%8)*2
                : tile_data+tile_i*16+(ln%8)*2; // tile is 16 bytes, each line is 2
        uint8_t low_byte = read(adr);
        uint8_t high_byte = read(adr+1);
        Palette pal = get_sprite_palette(s.attr & 1 << 4);
        bool priority = s.attr & 1 << 7;
        for (uint8_t px = 0; px < 8; ++px)
        {
            uint8_t x = s.x-8 + px;
            bool on_screen = (x < 160 && x > 0);
            // only draw pixel if on screen and if priority is 0 or priority is 1 and bg pixel is 0
            if (on_screen && (!priority || (priority && tex.pixel_index(x) == 0)))
            {
                bool x_flip = s.attr & 1 << 5;
                bool hi_bit = (high_byte & 1 << (x_flip ? px : 7-px));
                bool lo_bit = (low_byte & 1 << (x_flip ? px : 7-px));
                uint8_t p = static_cast<uint8_t>(hi_bit << 1 | lo_bit);
                if (p != 0) // only draw if not transparent
                    tex.set_pixel(x, pal[p]);
            }
        }
        ++sprites_drawn;
    }
}

std::array<Sprite, 40> Ppu::get_sprites()
{
    return sprites_;
}

Palette Ppu::get_bg_palette()
{
    Palette pal {};
    std::array<Color, 4> shades
    {{
        {255, 255, 255},
        {192, 192, 192},
        {96, 96, 96},
        {0, 0, 0}
    }};
    for (uint8_t i {0}; i < 4; ++i)
        pal[i] = shades[((bgp_ >> i*2) & 3)];
    return pal;
}

Palette Ppu::get_sprite_palette(bool alt_pal)
{
    Palette pal {};
    std::array<Color, 4> shades
    {{
        {255, 255, 255},
        {192, 192, 192},
        {96, 96, 96},
        {0, 0, 0}
    }};
    uint8_t obp = alt_pal ? obp1_ : obp0_;
    for (uint8_t i {1}; i < 4; ++i)
        pal[i] = shades[((obp >> i*2) & 3)];
    pal[0] = shades[0]; // 00 in sprite palette is always transparent
    return pal;
}

void Ppu::load_sprites()
{
    uint8_t i = 0;
    for (uint16_t adr = 0xfe00; adr < 0xfea0; adr += 4) // sprites are 4 bytes
    {
        sprites_[i].y = read(adr);
        sprites_[i].x = read(adr+1);
        sprites_[i].tile = read(adr+2);
        sprites_[i].attr = read(adr+3);
        ++i;
    }
}

// OAM_SCAN mode 2
void Ppu::oam_scan()
{
    if (clock_ >= 80)
    {
        clock_ -= 80;
        load_sprites();
        SET_BIT(stat_, 1);
        SET_BIT(stat_, 0); // mode 3
    }
}

// VRAM_READ mode 3
void Ppu::vram_read()
{
    if (clock_ >= 172)
    {
        clock_ -= 172;
        // enter hblank
        CLEAR_BIT(stat_, 1);
        CLEAR_BIT(stat_, 0); // mode 0
        // LCD_STAT if HBL is enabled
        if (stat_ & (1 << 3))
            cpu_.request_interrupt(Processor::Interrupt::LCD_STAT);
        if (!renderer_)
            return;
        render_scanline();
    }
}

// HBLANK mode 0
void Ppu::hblank()
{
    if (clock_ >= 204)
    {
        clock_ -= 204;
        increment_ly();
        if (ly_ == 144)
        {
            // enter vblank
            CLEAR_BIT(stat_, 1); // mode 1
            SET_BIT(stat_, 0);
            cpu_.request_interrupt(Processor::Interrupt::VBLANK);
            if (!renderer_)
                return;
            renderer_->present_screen();
        }
        else
        {
            // enter oam_scan
            SET_BIT(stat_, 1); // mode 2
            CLEAR_BIT(stat_, 0);
            // check LCD_STAT if OAM IRQ is enabled
            // checks bit 5 (OAM) and 4 (VBL) for some reason
            if (stat_ & (1 << 5) || stat_ & (1 << 4))
                cpu_.request_interrupt(Processor::Interrupt::LCD_STAT);
        }
    }
}

// VBLANK mode 1
void Ppu::vblank()
{
    if (clock_ >= 456)
    {
        clock_ -= 456;
        increment_ly();
        if (ly_ > 153)
        {
            // restart scanning modes
            SET_BIT(stat_, 1); // mode 2
            CLEAR_BIT(stat_, 0);
            ly_ = 0;
        }
    }
}

void Ppu::increment_ly()
{
    ++ly_;
    if (ly_ == lyc_ && stat_ & (1 << 6))
        cpu_.request_interrupt(Processor::Interrupt::LCD_STAT);
}
