#include "tile_tab.h"
#include "ui_tile_tab.h"

tile_tab::tile_tab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::tile_tab)
{
    ui->setupUi(this);
}

tile_tab::~tile_tab()
{
    delete ui;
}
