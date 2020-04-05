#include "qt_renderer.h"

#include <cmath>

Qt_renderer::Qt_renderer(int w, int h, QObject *parent)
    : QObject {parent},
      w_ {w}, h_ {h}, buf_(4*w*h)
{}

void Qt_renderer::draw_texture(const gameboy::Texture &t, int x_pos, int y_pos)
{
    const unsigned int w {t.width()};
    const unsigned int h {t.height()};
    buf_mutex_.lock();
    for (size_t i = 0; i < w*h; ++i)
    {
        gameboy::Color c = t.pixel(i);
        size_t j = y_pos*w_+i;
        buf_[4*j] = c.r;
        buf_[4*j+1] = c.g;
        buf_[4*j+2] = c.b;
    }
    buf_mutex_.unlock();
    /*
    for (size_t y = 0; y < h; ++y)
    {
        for (size_t x = 0; x < w; ++x)
        {
            size_t i = y*w+x;
            gameboy::Color c = t.pixel(i);
            buf_[4*((y+y_pos)*w+x)] = c.r;
            buf_[4*((y+y_pos)*w+x)+1] = c.g;
            buf_[4*((y+y_pos)*w+x)+2] = c.b;
        }
    }
    */
}

QImage Qt_renderer::image() const
{
    buf_mutex_.lock();
    QImage img(buf_.data(), w_, h_, QImage::Format_RGB32);
    buf_mutex_.unlock();
    return img;
}
