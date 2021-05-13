/*
#include "vram_window.h"
#include "system.hpp"
#include "tile_tab.h"
#include "frame_buffer_tab.h"
#include "sprite_tab.h"
#include "window_tab.h"
#include "palette_tab.h"

Vram_window::Vram_window(qtboy::Gameboy *s)
    :
      system_ {s}
{
    auto tile_tab = new Tile_tab(s);
    auto background_tab = new Frame_buffer_tab(s);
    auto sprite_tab = new Sprite_tab(s);
    // auto window_tab = new Window_tab(s);
    auto palette_tab = new Palette_tab(s);
    addTab(background_tab, tr("Frame Buffer"));
    addTab(tile_tab, tr("Tile Data"));
    addTab(sprite_tab, tr("Sprites"));
    addTab(palette_tab, tr("Palettes"));
}
*/
