#ifndef TILE_HPP
#define TILE_HPP

#include <QImage>
#include <array>

class Tile
{
    public:
    Tile();

    void load(const std::array<uint8_t, 16> &b);
    const QImage &image() const;

    private:
    QImage img_;
};

#endif // TILE_HPP
