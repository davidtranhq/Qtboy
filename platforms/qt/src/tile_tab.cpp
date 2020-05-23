#include "tile_tab.h"
#include "tile.h"

#include <QGridLayout>

Tile_tab::Tile_tab(gameboy::System *s)
    : debugger_ {s}
{
    /*
    auto tileset = debugger_.dump_raw_tileset();
    std::array<Tile, 384> tiles {};
    uint16_t i = 0;
    for (auto t : tileset)
    {
        tiles[i].load(t);
        ++i;
    }
    QGridLayout *layout = new QGridLayout;
    layout->setContentsMargins(0,0,0,0);
    layout->setSpacing(0);
    setLayout(layout);
    i = 0;
    for (auto t : tiles)
    {
        QLabel *label = new QLabel;
        label->setPixmap(QPixmap::fromImage(t.image()));
        label->setScaledContents(true);
        layout->addWidget(label, i>>4, i%16);
        ++i;
    }
    */
}
