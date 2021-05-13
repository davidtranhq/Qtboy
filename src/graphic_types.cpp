#include "graphic_types.hpp"

using namespace qtboy;

Texture::Texture(unsigned w, unsigned h)
    : w_ {w}, h_ {h}, data_(w*h), indices_(w*h)
{
    // set all pixels to lowest priority
    for (uint8_t &i : indices_)
        i = 0xff;
}

void Texture::fill(const Color &c)
{
    for (size_t i = 0; i < w_*h_; ++i)
    {
        data_[i] = c;
        indices_[i] = 0;
    }
}

void Texture::set_pixel(size_t i, const Color &c)
{
    data_[i] = c;
}

Color Texture::pixel(size_t i) const
{
    return data_[i];
}

void Texture::set_pixel_priority(size_t i, uint8_t index)
{
    indices_[i] = index;
}

uint8_t Texture::pixel_priority(size_t i) const
{
    return indices_[i];
}

unsigned Texture::width() const
{
    return w_;
}

unsigned Texture::height() const
{
    return h_;
}

