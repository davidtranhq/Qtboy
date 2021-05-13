#include <QtWidgets>
#include "mainwindow.h"
#include "debuggerwindow.h"
#include "vram_window.h"
#include "qt_speaker.h"

#include <chrono>
#include <sstream>

MainWindow::MainWindow(QWidget *parent,
                       const QString &title)
    : QMainWindow {parent},
      system_ {std::make_shared<qtboy::Gameboy>()},
      display_ {new QLabel(this)},
      renderer_ {new Qt_renderer(160, 144, this)},
      speaker_ {new Qt_speaker(this)},
      fpsTimer_ {new QTimer(this)},
      title_ {title}
{
    setCentralWidget(display_);
    setWindowTitle(title);

    display_->setScaledContents(true);
    display_->setMinimumSize(160, 144);

    system_->set_renderer(renderer_);
    system_->set_speaker(speaker_);

    fpsTimer_->setInterval(1000);
    fpsTimer_->callOnTimeout(this, &MainWindow::updateFps);
    fpsTimer_->start();

    connect(renderer_, SIGNAL(present_screen()), this, SLOT(updateDisplay()));
    createActions();
}


void MainWindow::loadRom(const QString &fileName)
{
    system_->reset();
    system_->load_cartridge(fileName.toStdString());
    system_->run_concurrently();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    auto key = event->key();
    if (key == controls_.a)
        system_->press(qtboy::Joypad::Input::A);
    else if (key == controls_.b)
        system_->press(qtboy::Joypad::Input::B);
    else if (key == controls_.up)
        system_->press(qtboy::Joypad::Input::Up);
    else if (key == controls_.down)
        system_->press(qtboy::Joypad::Input::Down);
    else if (key == controls_.left)
        system_->press(qtboy::Joypad::Input::Left);
    else if (key == controls_.right)
        system_->press(qtboy::Joypad::Input::Right);
    else if (key == controls_.select)
        system_->press(qtboy::Joypad::Input::Select);
    else if (key == controls_.start)
        system_->press(qtboy::Joypad::Input::Start);
    else if (key == controls_.turbo)
        system_->set_throttle(false);
}

void MainWindow::keyReleaseEvent(QKeyEvent *event)
{
    auto key = event->key();
    if (key == controls_.a)
        system_->release(qtboy::Joypad::Input::A);
    else if (key == controls_.b)
        system_->release(qtboy::Joypad::Input::B);
    else if (key == controls_.up)
        system_->release(qtboy::Joypad::Input::Up);
    else if (key == controls_.down)
        system_->release(qtboy::Joypad::Input::Down);
    else if (key == controls_.left)
        system_->release(qtboy::Joypad::Input::Left);
    else if (key == controls_.right)
        system_->release(qtboy::Joypad::Input::Right);
    else if (key == controls_.select)
        system_->release(qtboy::Joypad::Input::Select);
    else if (key == controls_.start)
        system_->release(qtboy::Joypad::Input::Start);
    else if (key == controls_.turbo)
        system_->set_throttle(true);
}

void MainWindow::updateDisplay()
{
    QImage img {renderer_->image()};
    Qt::TransformationMode transformation_mode = prefs_.antialiasing
            ? Qt::SmoothTransformation
            : Qt::FastTransformation;
    display_->setPixmap(QPixmap::fromImage(img).scaled(
                           centralWidget()->width(),
                           centralWidget()->height(),
                           Qt::IgnoreAspectRatio,
                           transformation_mode));
    ++frames_;
}

void MainWindow::updateFps()
{
    QString newTitle {title_ + " - FPS: " + QString::number(frames_)};
    setWindowTitle(newTitle);
    frames_ = 0;
}

void MainWindow::openRom()
{
    QString fileName = QFileDialog::getOpenFileName(this);
    if (!fileName.isEmpty())
        loadRom(fileName);
}

void MainWindow::showDebugger()
{
    /*
    if (!debuggerWindow_)
        debuggerWindow_ = new DebuggerWindow(system_, this);
    debuggerWindow_->show();
    */
}

void MainWindow::showVramViewer()
{
    /*
     auto *vram_viewer = new Vram_window(system_);
     vram_viewer->show();
     */
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
    system_->set_force_dmg(b);
}


void MainWindow::toggleSound(bool b)
{
    system_->toggle_sound(b);
}

QMenu *MainWindow::createMenu(const QString &name)
{
    return menuBar()->addMenu(name);
}

QAction *MainWindow::createSingleAction(const QString &name,
                                        QMenu *menu,
                                        void (MainWindow::*slot)())
{
    if (!menu)
        throw std::runtime_error {"Cannot add single action to menu pointing to null"};
    return menu->addAction(name, this, slot);
}

QAction *MainWindow::createCheckableAction(const QString &name,
                                           QMenu *menu,
                                           void (MainWindow::*slot)(bool),
                                           bool initiallyChecked)
{
    if (!menu)
        throw std::runtime_error {"Cannot add checkable action to menu pointing to null"};
    QAction *action = new QAction(name, this);
    action->setCheckable(true);
    action->setChecked(initiallyChecked);
    connect(action, &QAction::toggled, this, slot);
    menu->addAction(action);
    return action;
}

void MainWindow::createActions()
{
    // File menu
    QMenu *fileMenu = createMenu(tr("&File"));
    QAction *openAct = createSingleAction(tr("&Open"), fileMenu, &MainWindow::openRom);
    openAct->setShortcuts(QKeySequence::Open);

    // Options menu
    QMenu *optionsMenu = createMenu(tr("&Options"));
    createCheckableAction(tr("Anti-aliasing"),
                          optionsMenu,
                          &MainWindow::toggleAntiAlias);
    createCheckableAction(tr("Force DMG"),
                          optionsMenu,
                          &MainWindow::toggleForceDmg,
                          true);
    createCheckableAction(tr("Enable Sound"),
                          optionsMenu,
                          &MainWindow::toggleSound,
                          true);

    /*
    // Tools menu
    QMenu *toolsMenu = createMenu(tr("&Tools"));
    createSingleAction(tr("Debugger"), toolsMenu, &MainWindow::showDebugger);
    createSingleAction(tr("VRAM Viewer"), toolsMenu, &MainWindow::showVramViewer);
    */

    // Help menu
    QMenu *helpMenu = createMenu(tr("&Help"));
    createSingleAction(tr("&About"), helpMenu, &MainWindow::about);
}
