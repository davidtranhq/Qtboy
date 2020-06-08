#ifndef WINDOW_TAB_H
#define WINDOW_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Qt_renderer;

class Window_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Window_tab(gameboy::System *s);

    private:
    gameboy::Debugger debugger_;
    Qt_renderer *renderer_;
    QLabel *bg_;
};

#endif // WINDOW_TAB_H
