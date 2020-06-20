#include "frame_buffer_tab.h"
#include "qt_renderer.h"
#include "tile.h"

#include <QHBoxLayout>
#include <QFormLayout>
#include <QCheckBox>

Frame_buffer_tab::Frame_buffer_tab(gameboy::System *s)
    : debugger_ {s},
      renderer_ {new Qt_renderer(256, 256)},
      framebuffer_ {new QLabel},
      show_bg_ {new QCheckBox(tr("Show background"))},
      show_window_ {new QCheckBox(tr("Show window"))},
      show_sprites_ {new QCheckBox(tr("Show sprites"))}
{
    show_bg_->setChecked(true);
    show_window_->setChecked(true);
    show_sprites_->setChecked(true);

    connect(show_bg_, SIGNAL(stateChanged(int)),
            this, SLOT(layersChangedEvent()));
    connect(show_window_, SIGNAL(stateChanged(int)),
            this, SLOT(layersChangedEvent()));
    connect(show_sprites_, SIGNAL(stateChanged(int)),
            this, SLOT(layersChangedEvent()));

    QFormLayout *options = new QFormLayout;
    options->addWidget(show_bg_);
    options->addWidget(show_window_);
    options->addWidget(show_sprites_);

    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(framebuffer_, 5);
    layout->addLayout(options, 1);

    setLayout(layout);
    layersChangedEvent();
}

void Frame_buffer_tab::layersChangedEvent()
{
    renderer_->clear();
    fb_texture_ = debugger_.dump_framebuffer(show_bg_->isChecked(),
                                             show_window_->isChecked(),
                                             show_sprites_->isChecked());
    display();
}

void Frame_buffer_tab::resizeEvent(QResizeEvent *event)
{
    display();
}

void Frame_buffer_tab::display()
{
    renderer_->draw_texture(fb_texture_);
    QLayoutItem *fb = layout()->itemAt(0);
    int w = fb->geometry().width(), h = fb->geometry().height();
    framebuffer_->setPixmap(renderer_->pixmap().scaled(w, h, Qt::KeepAspectRatio));
}
