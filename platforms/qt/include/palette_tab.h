#ifndef PALETTE_TAB_H
#define PALETTE_TAB_H

#include <QWidget>

#include "system.hpp"
#include "debugger.hpp"

class QLabel;
class QVBoxLayout;

namespace gameboy
{
class System;
}


class Palette_tab : public QWidget
{
    Q_OBJECT

    public:
    explicit Palette_tab(gameboy::System *s, QWidget *parent = nullptr);

    signals:

    private:
    enum class Palette_type { Background, Sprite };
    QLabel *create_swatch(const gameboy::Color &c);
    QVBoxLayout *create_palette_views(Palette_type t,
                                      const std::array<gameboy::Palette, 8> &pals,
                                      const std::array<uint8_t, 64> &raw_pals);

    private:
    gameboy::Debugger debugger_;
    std::array<QLabel *, 64> swatches_;
};

#endif // PALETTE_TAB_H
