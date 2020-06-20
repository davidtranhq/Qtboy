#ifndef TILETAB_H
#define TILETAB_H

#include <QWidget>

namespace Ui {
class TileTab;
}

class TileTab : public QWidget
{
    Q_OBJECT

public:
    explicit TileTab(QWidget *parent = nullptr);
    ~TileTab();

private:
    Ui::TileTab *ui;
};

#endif // TILETAB_H
