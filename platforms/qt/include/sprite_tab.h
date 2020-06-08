#ifndef SPRITE_TAB_H
#define SPRITE_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Qt_renderer;

class Sprite_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Sprite_tab(gameboy::System *s);

    private:
    gameboy::Debugger debugger_;
    Qt_renderer *renderer_;
    QLabel *bg_;
};

#endif // SPRITE_TAB_H
