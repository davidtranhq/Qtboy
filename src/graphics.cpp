#include "graphics.h"

using namespace gameboy;

uint32_t Color::argb() const
{
    return static_cast<uint32_t>(a << 24 | r << 16 | g << 8 | b);
}
