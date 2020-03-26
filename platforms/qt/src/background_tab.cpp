#include "background_tab.h"
#include "qt_renderer.h"
#include "tile.h"

#include <QVBoxLayout>
#include <QGridLayout>

Background_tab::Background_tab(gameboy::System *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(256, 256)},
      bg_ {new QLabel}
{
    auto background = debugger_.dump_background();
    renderer_->draw_texture(background);
    bg_->setScaledContents(true);
    bg_->setPixmap(QPixmap::fromImage(renderer_->image()));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bg_);
    setLayout(layout);

    /* using tiles
    std::array<uint8_t, 32*32> bg_map = debugger_.dump_raw_background();
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
    for (int8_t t : bg_map)
    {
        QLabel *label = new QLabel;
        label->setPixmap(QPixmap::fromImage(tiles[256+t].image()));
        label->setScaledContents(true);
        layout->addWidget(label, i>>5, i%32);
        ++i;
    }
    */
}
