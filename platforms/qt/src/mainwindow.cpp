#include <QtWidgets>
#include "mainwindow.h"
#include "disassemblerwindow.h"
#include "debuggerwindow.h"
#include "vram_window.h"
#include "qt_speaker.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow {parent},
      renderer_ {new Qt_renderer(160, 144)},
      speaker_ {new Qt_speaker},
      display {new QLabel}
{
    system.set_renderer(renderer_);
    system.set_speaker(speaker_);
    createActions();
    display->setScaledContents(true);
    display->setMinimumSize(160, 144);
    setCentralWidget(display);
    connect(renderer_, SIGNAL(present_screen()), this, SLOT(update_display()));
}


void MainWindow::loadRom(const QString &fileName)
{
    system.reset();
    system.load_cartridge(fileName.toStdString());
    system.run_concurrently();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    auto controls = controls_;
    auto key = event->key();
    if (key == controls_.b)
        system.press(gameboy::Joypad::Input::B);
    else if (key == controls.a)
        system.press(gameboy::Joypad::Input::A);
    else if (key == controls.up)
        system.press(gameboy::Joypad::Input::Up);
    else if (key == controls.down)
        system.press(gameboy::Joypad::Input::Down);
    else if (key == controls.left)
        system.press(gameboy::Joypad::Input::Left);
    else if (key == controls.right)
        system.press(gameboy::Joypad::Input::Right);
    else if (key == controls.select)
        system.press(gameboy::Joypad::Input::Select);
    else if (key == controls.start)
        system.press(gameboy::Joypad::Input::Start);
    else if (key == controls.turbo)
        system.set_throttle(0.0);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    auto controls = controls_;
    auto key = event->key();
    if (key == controls_.b)
        system.release(gameboy::Joypad::Input::B);
    else if (key == controls.a)
        system.release(gameboy::Joypad::Input::A);
    else if (key == controls.up)
        system.release(gameboy::Joypad::Input::Up);
    else if (key == controls.down)
        system.release(gameboy::Joypad::Input::Down);
    else if (key == controls.left)
        system.release(gameboy::Joypad::Input::Left);
    else if (key == controls.right)
        system.release(gameboy::Joypad::Input::Right);
    else if (key == controls.select)
        system.release(gameboy::Joypad::Input::Select);
    else if (key == controls.start)
        system.release(gameboy::Joypad::Input::Start);
    else if (key == controls.turbo)
        system.set_throttle(1.0);
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

void MainWindow::showVramViewer()
{
    auto vram_viewer = new Vram_window(&system);
    vram_viewer->show();
}


void MainWindow::about()
{
    QMessageBox::about(this, tr("About QtBoy"),
                       tr("A Gameboy emulator made using Qt."));
}

void MainWindow::toggleAntiAlias(bool b)
{
    prefs_.antialiasing = b;
}

void MainWindow::toggleForceDmg(bool b)
{
    prefs_.force_dmg = b;
    system.force_dmg = b;
}

void MainWindow::openCustomPaletteWindow()
{

}

void MainWindow::update_display()
{
    QImage img {renderer_->image()};
    Qt::TransformationMode transformation_mode = prefs_.antialiasing
            ? Qt::SmoothTransformation
            : Qt::FastTransformation;
    display->setPixmap(QPixmap::fromImage(img).scaled(
                           centralWidget()->width(),
                           centralWidget()->height(),
                           Qt::IgnoreAspectRatio,
                           transformation_mode));
}


void MainWindow::createActions()
{
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = fileMenu->addAction(tr("&Open"), this, &MainWindow::openRom);
    openAct->setShortcuts(QKeySequence::Open);

    QMenu *optionsMenu = menuBar()->addMenu(tr("&Options"));
    QAction *antiAliasAct = new QAction(tr("Anti-aliasing"), this);
    antiAliasAct->setCheckable(true);
    connect(antiAliasAct, SIGNAL(toggled(bool)), this, SLOT(toggleAntiAlias(bool)));
    optionsMenu->addAction(antiAliasAct);
    optionsMenu->addAction(tr("Custom Palettes"), this,
                           &MainWindow::openCustomPaletteWindow);
    QAction *forceDmgAct = new QAction(tr("Force DMG"), this);
    forceDmgAct->setCheckable(true);
    connect(forceDmgAct, SIGNAL(toggled(bool)), this, SLOT(toggleForceDmg(bool)));
    optionsMenu->addAction(forceDmgAct);


    QMenu *toolsMenu = menuBar()->addMenu(tr("&Tools"));
    toolsMenu->addAction(tr("Disassemble"), this, &MainWindow::showDisassembler);
    toolsMenu->addAction(tr("Debugger"), this, &MainWindow::showDebugger);
    toolsMenu->addAction(tr("VRAM Viewer"), this, &MainWindow::showVramViewer);

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
}
