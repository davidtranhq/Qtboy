#ifndef FRAME_BUFFER_TAB_H
#define FRAME_BUFFER_TAB_H

#include <QWidget>
#include <QLabel>

#include "system.hpp"
#include "debugger.hpp"

class Qt_renderer;
class QCheckBox;

class Frame_buffer_tab : public QWidget
{
    Q_OBJECT
    public:
    explicit Frame_buffer_tab(gameboy::System *s);

    private slots:
    void layersChangedEvent();
    void resizeEvent(QResizeEvent *event) override;

    private:
    void display();

    private:
    enum Layers { Background, Window, Sprite };
    gameboy::Debugger debugger_;
    gameboy::Texture fb_texture_ {};
    Qt_renderer *renderer_;
    QLabel *framebuffer_;
    QCheckBox *show_bg_, *show_window_, *show_sprites_, *show_viewport_;
};

#endif // Frame_buffer_tab_H
