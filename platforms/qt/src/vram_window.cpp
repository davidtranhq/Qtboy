#include "vram_window.h"
#include "system.hpp"
#include "tile_tab.h"
#include "background_tab.h"
#include "sprite_tab.h"

Vram_window::Vram_window(gameboy::System *s)
    :
      system_ {s}
{
    auto tile_tab = new Tile_tab(s);
    auto background_tab = new Background_tab(s);
    auto sprite_tab = new Sprite_tab(s);
    addTab(tile_tab, tr("Tile Map"));
    addTab(background_tab, tr("Background"));
    addTab(sprite_tab, tr("Sprites"));
}
