#ifndef VRAM_WINDOW_H
#define VRAM_WINDOW_H

#include <QWidget>
#include <QTabWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Qt_renderer;

class Vram_window : public QTabWidget
{
    Q_OBJECT
    public:
    explicit Vram_window(gameboy::System *s);

    private:
    gameboy::System *system_;


};

#endif // VRAM_WINDOW_H
