#ifndef QT_RENDERER_H
#define QT_RENDERER_H

#include "renderer.hpp"
#include "graphic_types.hpp"
#include <QObject>
#include <QImage>
#include <QPixmap>
#include <mutex>

class Qt_renderer : public QObject, public qtboy::Renderer
{
    Q_OBJECT

    public:
    explicit Qt_renderer(unsigned w, unsigned h, QObject *parent = nullptr);

    void draw_texture(const qtboy::Texture &,
                      unsigned x = 0, unsigned y = 0) override;

    void clear();

    QImage image() const;
    QPixmap pixmap() const;

    signals:
    void present_screen() override;

    private:
    mutable std::mutex buf_mutex_;
    unsigned w_, h_;
    std::vector<uint8_t> buf_;
};

#endif // QT_RENDERER_H
