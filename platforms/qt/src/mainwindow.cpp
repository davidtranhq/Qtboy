#include <QtWidgets>
#include "mainwindow.h"
#include "disassemblerwindow.h"
#include "debuggerwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow {parent},
      renderer {new Qt_renderer},
      display {new QLabel}
{
    loadRom("../../../roms/cpu_instrs.gb");
    createActions();
    setCentralWidget(display);
}

void MainWindow::openRom()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadRom(fileName);
}

void MainWindow::showDisassembler()
{
    auto disassembler = new DisassemblerWindow(this, &system);
    disassembler->show();
}

void MainWindow::showDebugger()
{
    auto debugger = new Debugger_window(this, &system);
    debugger->show();
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
    QAction *disassembleAct = toolsMenu->addAction(tr("Disassemble"), this, &MainWindow::showDisassembler);
    QAction *debuggerAct = toolsMenu->addAction(tr("Debugger"), this, &MainWindow::showDebugger);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}

void MainWindow::loadRom(const QString &fileName)
{
    system.reset();
    system.load_cartridge(fileName.toStdString());
}
