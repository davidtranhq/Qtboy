#ifndef TILE_TAB_H
#define TILE_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Tile_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Tile_tab(gameboy::System *s);

    private:
    gameboy::Debugger debugger_;


};

#endif // TILE_TAB_H
