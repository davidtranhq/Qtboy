#ifndef TILE_TAB_H
#define TILE_TAB_H

#include <QWidget>

namespace Ui {
class tile_tab;
}

class tile_tab : public QWidget
{
    Q_OBJECT

public:
    explicit tile_tab(QWidget *parent = nullptr);
    ~tile_tab();

private:
    Ui::tile_tab *ui;
};

#endif // TILE_TAB_H
