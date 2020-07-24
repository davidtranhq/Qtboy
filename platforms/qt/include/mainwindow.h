#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "system.hpp"
#include "qt_renderer.h"
#include "qt_speaker.h"

#include <QMainWindow>
#include <thread>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QImage;
class QLabel;
QT_END_NAMESPACE

class DisassemblerWindow;
class DebuggerWindow;
class CustomPaletteWindow;

struct Preferences
{
    bool antialiasing;
    bool force_dmg;
};

struct Controls
{
    Qt::Key a {Qt::Key_A},
            b {Qt::Key_B},
            up {Qt::Key_Up},
            down {Qt::Key_Down},
            left {Qt::Key_Left},
            right {Qt::Key_Right},
            select {Qt::Key_Shift},
            start {Qt::Key_Return},
            turbo {Qt::Key_Space};
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void loadRom(const QString &fileName);

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

private slots:
    // file menu
    void openRom();
    void showDisassembler();
    void showDebugger();
    void showVramViewer();
    void about();
    // options menu
    void toggleAntiAlias(bool);
    void toggleForceDmg(bool);
    void openCustomPaletteWindow();

    void update_display();

private:
    void createActions();

    gameboy::System system_;
    gameboy::Debugger debugger_ {&system_};
    Qt_renderer *renderer_ {nullptr};
    Qt_speaker *speaker_ {nullptr};
    QLabel *display;
    QString curRom;
    Preferences prefs_;
    Controls controls_;

};

#endif // MAINWINDOW_H
