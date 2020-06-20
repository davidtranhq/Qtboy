#ifndef TILE_TAB_H
#define TILE_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class QRadioButton;

class Qt_renderer;

class Tile_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Tile_tab(gameboy::System *s);

    protected:
    void resizeEvent(QResizeEvent *e) override;

    private slots:
    void button0ClickedEvent();
    void button1ClickedEvent();

    private:
    QPixmap load_tilemap(bool idx);
    void display_tilemap(bool idx);

    private:
    gameboy::Debugger debugger_;
    Qt_renderer *renderer_;
    QLabel *tilemap_;
    QRadioButton *vram0_, *vram1_;
};

#endif // TILE_TAB_H
