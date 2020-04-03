#include "graphic_types.hpp"

using namespace gameboy;

Texture::Texture(unsigned w, unsigned h)
    : w_ {w}, h_ {h}, data_(w*h), indices_(w*h)
{}

void Texture::set_pixel(size_t i, Color c)
{
    data_[i] = c;
}

Color Texture::pixel(size_t i) const
{
    return data_[i];
}

void Texture::set_pixel_index(size_t i, int index)
{
    indices_[i] = index;
}

int Texture::pixel_index(size_t i) const
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

