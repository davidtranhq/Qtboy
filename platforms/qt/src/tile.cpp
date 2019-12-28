#include "tile.h"

Tile::Tile()
    : img_(8, 8, QImage::Format_Indexed8)
{
    const QRgb white {0xff000000},
               light_gray {0xffd3d3d3},
               gray {0xff808080},
               black {0xffffffff};

    img_.setColor(3, white);
    img_.setColor(2, gray);
    img_.setColor(1, light_gray);
    img_.setColor(0, black);
}

void Tile::load(const std::array<uint8_t, 16> &b)
{
    // one row of pixels is encoded in 2 bytes
    for (uint8_t byte {0}; byte < 16; byte += 2)
    {
        // pixel lsb in byte 1; msb in byte 2
        for (uint8_t bit {0}; bit < 8; ++bit)
        {
            bool lsb = b[byte] & (1 << (7-bit));
            bool msb = b[byte+1] & (1 << (7-bit));
            uint8_t colorIndex = (msb << 1) | lsb;
            img_.setPixel(bit, byte/2, colorIndex);
        }
    }
}

const QImage &Tile::image() const
{
    return img_;
}
