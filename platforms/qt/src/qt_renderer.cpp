#include "qt_renderer.h"

#include <cmath>

Qt_renderer::Qt_renderer(unsigned w, unsigned h, QObject *parent)
    : QObject {parent},
      w_ {w}, h_ {h}, buf_(2*w*h)
{}

void Qt_renderer::draw_texture(const gameboy::Texture &t,
                               unsigned x_off, unsigned y_off)
{
    const unsigned int w {t.width()};
    const unsigned int h {t.height()};
    {
        const std::lock_guard<std::mutex> lock(buf_mutex_);
        for (unsigned int i = 0; i < w*h; ++i)
        {
            int x = x_off + i % w;
            int y = y_off + i / w;
            // prevent wrapping
            if (x > w_ || y > h_)
                continue;
            int j = y*w_+x;
            // represent in RGB555
            gameboy::Color c = t.pixel(i);
            buf_[2*j] = c & 0xff;
            buf_[2*j+1] = c >> 8;
        }
    }
}

void Qt_renderer::clear()
{
    buf_.clear();
    buf_.resize(w_*h_*4);
}

QImage Qt_renderer::image() const
{
    const std::lock_guard<std::mutex> lock(buf_mutex_);
    QImage img(buf_.data(), w_, h_, QImage::Format_RGB555);
    return img;
}

QPixmap Qt_renderer::pixmap() const
{
    return QPixmap::fromImage(image());
}
