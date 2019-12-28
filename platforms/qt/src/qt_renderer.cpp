#include "qt_renderer.h"

#include <cmath>

Qt_renderer::Qt_renderer(QObject *parent)
    : QObject {parent},
      img_ {256, 256, QImage::Format_Indexed8}
{
}

void Qt_renderer::draw_scanline()
{}

void Qt_renderer::draw_framebuffer(gameboy::Ppu::Frame_data fd)
{
    for (uint8_t c {0}; c < 4; ++c)
    {
        QColor color
        {
            fd.palette[c].r,
            fd.palette[c].g,
            fd.palette[c].b
        };
        img_.setColor(c, color.rgb());
    }
    for (uint16_t p {0}; p < fd.pixels.size(); ++p)
        img_.setPixel(p%256, std::floor(p/256), fd.pixels[p]);
}
