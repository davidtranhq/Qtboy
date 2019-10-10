#include <QtWidgets>
#include "mainwindow.h"
#include "disassemblerwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      display(new QImage),
      displayArea(new QWidget),
      displayLabel(new QLabel(displayArea))
{
    createActions();
    displayLabel->setPixmap(QPixmap::fromImage(*display));
    setCentralWidget(displayArea);
    displayArea->show();
    loadRom("C:/Users/david/Documents/GitHub/Gameboy/roms/tetris.gb");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    event->accept();
}

void MainWindow::openRom()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadRom(fileName);
}

void MainWindow::showDisassembler()
{
    DisassemblerWindow dw {system};
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QtBoy"),
                       tr("A Gameboy emulator made using Qt."));
}

void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = fileMenu->addAction(tr("&Open"), this, &MainWindow::openRom);
    openAct->setShortcuts(QKeySequence::Open);

    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    QAction *disassembleAct = toolsMenu->addAction(tr("&Disassemble"), this, &MainWindow::showDisassembler);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}

void MainWindow::loadRom(const QString &fileName)
{
    system.load_cartridge(fileName.toStdString());
}
