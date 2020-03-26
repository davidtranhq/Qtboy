#ifndef QT_RENDERER_H
#define QT_RENDERER_H

#include "renderer.hpp"
#include "graphics.hpp"
#include <QObject>
#include <QImage>

class Qt_renderer : public QObject, public gameboy::Renderer
{
    Q_OBJECT

    public:
    explicit Qt_renderer(int w, int h, QObject *parent = nullptr);

    void draw_texture(const gameboy::Texture &, int x = 0, int y = 0) override;

    QImage image() const;

    signals:
    void present_screen() override;

    private:
    int w_, h_;
    std::vector<uint8_t> buf_;
};

#endif // QT_RENDERER_H
