#include "tiletab.h"
#include "ui_tiletab.h"

TileTab::TileTab(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TileTab)
{
    ui->setupUi(this);
}

TileTab::~TileTab()
{
    delete ui;
}
