#ifndef QT_RENDERER_H
#define QT_RENDERER_H

#include "renderer.hpp"
#include <QObject>
#include <QImage>

class Qt_renderer : public QObject, public gameboy::Renderer
{
    Q_OBJECT

    public:
    explicit Qt_renderer(QObject *parent = nullptr);

    void draw_scanline() override;
    void draw_framebuffer(gameboy::Ppu::Frame_data) override;

    QImage image() const noexcept { return img_; }

    signals:
    void present_screen() override;

    private:
    QImage img_;
};

#endif // QT_RENDERER_H
