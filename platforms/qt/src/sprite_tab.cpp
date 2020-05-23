#include "sprite_tab.h"
#include "tile.h"

#include <QGridLayout>

Sprite_tab::Sprite_tab(gameboy::System *s)
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
    auto sprites = debugger_.dump_sprites();
    QGridLayout *layout = new QGridLayout;
    setLayout(layout);
    i = 0;
    for (auto s : sprites)
    {
        QLabel *label = new QLabel;
        label->setPixmap(QPixmap::fromImage(tiles[s.tile].image()));
        label->setScaledContents(true);
        layout->addWidget(label, i/10, i%10);
        QLabel *y = new QLabel;
        y->setText(QString::number(s.y));
        layout->addWidget(y, 5, 0);
        ++i;
    }
    */
}
