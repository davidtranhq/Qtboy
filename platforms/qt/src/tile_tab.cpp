/*
#include "tile_tab.h"
#include "qt_renderer.h"
#include <QGridLayout>
#include <QRadioButton>
#include <QHBoxLayout>
#include <QFormLayout>

Tile_tab::Tile_tab(qtboy::Gameboy *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(128, 192)},
      tilemap_ {new QLabel},
      vram0_ {new QRadioButton("VRAM Bank 0", this)},
      vram1_ {new QRadioButton("VRAM Bank 1", this)}
{
    // disable viewing VRAM 1 if system is in DMG mode (not CGB)
    if (!s->is_cgb())
    {
        vram1_->setText("VRAM Bank 1 is unavailable in DMG mode");
        vram1_->setEnabled(false);
    }

    connect(vram0_, SIGNAL(clicked()), this, SLOT(button0ClickedEvent()));
    connect(vram1_, SIGNAL(clicked()), this, SLOT(button1ClickedEvent()));

    QFormLayout *btn_layout = new QFormLayout;
    btn_layout->addWidget(vram0_);
    btn_layout->addWidget(vram1_);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(tilemap_, 4);
    layout->addLayout(btn_layout, 1);
    setLayout(layout);
    // draw by default tilemap in VRAM bank 0
    vram0_->setChecked(true);
    button0ClickedEvent();

}

void Tile_tab::resizeEvent(QResizeEvent *)
{
    display_tilemap(vram1_->isChecked());
}

QPixmap Tile_tab::load_tilemap(bool idx)
{
    std::array<qtboy::Texture, 384> tiles(debugger_.dump_tileset(idx));
    for (unsigned i = 0; i < tiles.size(); ++i)
    {
        int x = (i%16) * 8; // 16 tiles per row, 8 pixels per tile
        int y = (i>>4) * 8; // 24 tiles per column, 8 pixels per tile
        renderer_->draw_texture(tiles[i], x, y);
    }
    return QPixmap::fromImage(renderer_->image());
}

void Tile_tab::display_tilemap(bool idx)
{
    QLayoutItem *tilemap = layout()->itemAt(0);
    int w = tilemap->geometry().width(), h = tilemap->geometry().height();
    tilemap_->setPixmap(load_tilemap(idx).scaled(w, h));
}

void Tile_tab::button0ClickedEvent()
{
    display_tilemap(0);
}

void Tile_tab::button1ClickedEvent()
{
    display_tilemap(1);
}
*/
