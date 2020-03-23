#ifndef BACKGROUND_TAB_H
#define BACKGROUND_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Qt_renderer;

class Background_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Background_tab(gameboy::System *s);

    private:
    gameboy::Debugger debugger_;
    Qt_renderer *renderer_;
    QLabel *bg_;
};

#endif // BACKGROUND_TAB_H
