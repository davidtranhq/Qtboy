#include "window_tab.h"
#include "qt_renderer.h"

#include <QVBoxLayout>

Window_tab::Window_tab(gameboy::System *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(256, 256)},
      bg_ {new QLabel}
{
    auto background = debugger_.dump_window();
    renderer_->draw_texture(background);
    bg_->setScaledContents(true);
    bg_->setPixmap(QPixmap::fromImage(renderer_->image()));
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(bg_);
    setLayout(layout);
}
