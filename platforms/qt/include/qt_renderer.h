#ifndef QT_RENDERER_H
#define QT_RENDERER_H

#include "renderer.hpp"
#include <QObject>
#include <QImage>

class Qt_renderer : public QObject, public gameboy::Renderer
{
    Q_OBJECT

    public:
    explicit Qt_renderer(int w, int h, QObject *parent = nullptr);

    void draw_tile(gameboy::Tile_data, size_t x, size_t y) override;
    void draw_scanline(gameboy::Line_data) override;
    void draw_framebuffer(gameboy::Frame_data) override;

    QImage image() const noexcept { return img_; }

    signals:
    void present_screen() override;

    private:
    void set_palette(const gameboy::Palette &p);

    private:
    QImage img_;
};

#endif // QT_RENDERER_H
