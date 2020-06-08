#include "tile_tab.h"
#include "qt_renderer.h"
#include <QGridLayout>

Tile_tab::Tile_tab(gameboy::System *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(128, 192)},
      bg_ {new QLabel}
{
    std::array<gameboy::Texture, 384> tiles(debugger_.dump_tileset());
    for (int i = 0; i < tiles.size(); ++i)
    {
        int x = (i%16) * 8; // 16 tiles per row, 8 pixels per tile
        int y = (i>>4) * 8; // 24 tiles per column, 8 pixels per tile
        renderer_->draw_texture(tiles[i], x, y);
    }
    renderer_->draw_texture(tiles[0]);
    bg_->setScaledContents(true);
    bg_->setPixmap(QPixmap::fromImage(renderer_->image()));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bg_);
    setLayout(layout);
}
