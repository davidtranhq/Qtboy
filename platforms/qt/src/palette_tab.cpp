/*
#include "palette_tab.h"

#include "system.hpp"
#include "debugger.hpp"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QLabel>

using qtboy::Palette;
using qtboy::Ppu;

Palette_tab::Palette_tab(qtboy::Gameboy *s, QWidget *parent)
    : QWidget(parent),
      debugger_(s)
{
    std::array<Palette, 8> bg_pals(debugger_.dump_bg_palettes());
    std::array<Palette, 8> spr_pals(debugger_.dump_sprite_palettes());
    Ppu::Dump dmp(debugger_.dump_ppu());
    QVBoxLayout *bg = create_palette_views(Palette_type::Background, bg_pals,
                                           dmp.bgpd);
    QVBoxLayout *obp = create_palette_views(Palette_type::Sprite, spr_pals,
                                            dmp.obpd);
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addLayout(bg);
    layout->addLayout(obp);
    setLayout(layout);
}

QLabel *Palette_tab::create_swatch(const qtboy::Color &c)
{
    QPixmap pix(50, 20);
    double b = (c & 0x1f) / 0x1f;
    double g = (c >> 5 & 0x1f) / 0x1f;
    double r = (c >> 10 & 0x1f) / 0x1f;
    pix.fill(QColor::fromRgbF(r, g, b));
    QLabel *l = new QLabel;
    l->setPixmap(pix);
    return l;
}

QVBoxLayout *Palette_tab::create_palette_views(Palette_type t,
    const std::array<qtboy::Palette, 8> &pals,
    const std::array<uint8_t, 64> &raw_pals)
{
    QVBoxLayout *palette_view = new QVBoxLayout;
    std::string lbl_txt {};
    if (t == Palette_type::Background)
        lbl_txt = "BGP%1";
    else if (t == Palette_type::Sprite)
        lbl_txt = "OBP%1";
    else
        return palette_view;
    for (uint8_t pal = 0; pal < 8; ++pal)
    {
        QLabel *pal_lbl = new QLabel(QString::fromStdString(lbl_txt).arg(pal));
        QHBoxLayout *hl = new QHBoxLayout;
        hl->addWidget(pal_lbl);
        // set color of each swatch
        for (uint8_t col = 0; col < 4; ++col)
        {
            // create swatch
            qtboy::Color c = pals[pal][col];
            QLabel *swatch = create_swatch(c);
            // create color label (#xxxx)
            uint16_t color_value = static_cast<uint16_t>(
                        raw_pals[pal*8+col*2] | raw_pals[pal*8+col*2+1] << 8);
            QLabel *lbl = new QLabel(QString("%1h").arg(color_value, 4, 16));
            QVBoxLayout *vl = new QVBoxLayout;
            vl->addWidget(swatch);
            vl->addWidget(lbl);
            hl->addLayout(vl);
        }
        palette_view->addLayout(hl);
    }
    return palette_view;
}
*/
