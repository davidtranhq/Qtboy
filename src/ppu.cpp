#include "ppu.hpp"
#include "renderer.hpp"
#include "exception.hpp"

#include <iostream>

using namespace gameboy;

Ppu::Ppu(std::function<uint8_t(uint16_t)> rd,
         std::function<void(uint8_t, uint16_t)> wr,
         Renderer *r)
    : read {std::move(rd)},
      write {std::move(wr)},
      renderer_ {r}
{}

void Ppu::reset()
{
    mode_ = 0;
    clock_ = 0;
    line_ = 0;
    write(line_, 0xff44);
}

void Ppu::step(size_t cycles)
{
    lcdc_ = read(0xff40);
    if (!lcdc_[7])
    {
        reset();
        return; // don't execute if master bit is off
    }
    // PPU operates on 4.194 MHz clock, 1 clock = 1/4 cycle
    clock_ += cycles;
    switch (mode_)
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

void Ppu::draw_framebuffer()
{
    if (!renderer_)
        return;
    uint8_t lcdc {read(0xff40)};
    // bit 4 of LCDC:
    // 0 uses tileset from 8800 to 97FF with signed addressing,
    // 1 uses tileset from 8000 to 8fff with unsigned addressing
    uint16_t tile_base = (lcdc & (1 << 4) ? 0x8000 : 0x8800);
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
        if (tile_base == 0x8800) // signed addressing
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

void Ppu::draw_scanline()
{

}

void Ppu::set_renderer(Renderer *r)
{
    renderer_ = r;
}

int Ppu::mode() const
{
    return mode_;
}

int Ppu::clock() const
{
    return clock_;
}

void Ppu::oam_scan()
{
    if (clock_ >= 80)
    {
        clock_ -= 80;
        mode_ = 3;
    }
}

void Ppu::vram_read()
{
    if (clock_ >= 172)
    {
        clock_ -= 172;
        mode_ = 0;
        if (!renderer_)
            return;
        renderer_->draw_scanline();
    }
}

void Ppu::hblank()
{
    if (clock_ >= 204)
    {
        clock_ -= 204;
        ++line_;
        if (line_ == 144)
        {
            mode_ = 1;
            write(line_, 0xff44);
            if (!renderer_)
                return;
            renderer_->present_screen();
        }
        else
        {
            mode_ = 2;
        }
        write(line_, 0xff44); // update LY register, indicating current
                              // horizontal scanline
    }
}

void Ppu::vblank()
{
    if (clock_ >= 456)
    {
        clock_ -= 456;
        ++line_;
        if (line_ > 153)
        {
            // restart scanning modes
            mode_ = 2;
            line_ = 0;
            write(line_, 0xff44);
        }
        write(line_, 0xff44); // update LY register, indicating current
                              // horizontal scanline
    }
}
