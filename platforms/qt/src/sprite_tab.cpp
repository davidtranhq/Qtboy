/*
#include "sprite_tab.h"
#include "qt_renderer.h"

#include <QGridLayout>

Sprite_tab::Sprite_tab(qtboy::Gameboy *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(8, 8)},
      bg_ {new QLabel}
{
    auto sprites = debugger_.dump_sprites();
    QGridLayout *layout = new QGridLayout;
    int i = 0;
    for (const qtboy::Texture &t : sprites)
    {
        QLabel *label = new QLabel;
        renderer_->draw_texture(t);
        label->setScaledContents(true);
        label->setPixmap(QPixmap::fromImage(renderer_->image()));
        layout->addWidget(label, i/10, i%10);
        ++i;
    }
    setLayout(layout);
}
*/
