#include "qt_renderer.h"

#include <cmath>

Qt_renderer::Qt_renderer(int w, int h, QObject *parent)
    : QObject {parent},
      img_ {w, h, QImage::Format_ARGB32}
{}

void Qt_renderer::draw_tile(gameboy::Tile_data td, size_t x, size_t y)
{
    for (uint16_t p {0}; p < td.pixels.size(); ++p)
        img_.setPixel(x+(p%8), y+(p >> 3), td.pixels[p]);
}

void Qt_renderer::draw_scanline(gameboy::Line_data ld)
{
    for (uint16_t p {0}; p < ld.pixels.size(); ++p)
        img_.setPixel(p, ld.line, ld.pixels[p]);
}

void Qt_renderer::draw_framebuffer(gameboy::Frame_data fd)
{
    for (size_t p {0}; p < fd.pixels.size(); ++p)
        img_.setPixel(p%256, p>>8, fd.pixels[p]);
}
