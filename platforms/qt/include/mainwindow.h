#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "system.hpp"
#include "qt_renderer.h"
#include "qt_speaker.h"

#include <QMainWindow>

class QAction;
class QMenu;
class QImage;
class QLabel;
class QTimer;

/*
class DebuggerWindow;
class VramWindow;
*/

struct Preferences
{
    bool antialiasing;
    bool force_dmg;
};

struct Controls
{
    Qt::Key a {Qt::Key_D},
            b {Qt::Key_S},
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
    explicit MainWindow(QWidget *parent = nullptr,
                        const QString &title = "Gameboy");

    protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);

    private slots:
    // Update the display to show the most recent frame in the renderer.
    // This is called by the PPU on VBLANK.
    void updateDisplay();

    // Update the FPS counter in the window title. A QTimer calls this every second.
    void updateFps();

    // Open file dialog to choose a ROM file
    void openRom();

    // Create debugger window
    void showDebugger();

    // Create VRAM viewer window
    void showVramViewer();

    // Show `about` window
    void about();

    // options menu slots
    void toggleAntiAlias(bool);
    void toggleForceDmg(bool);
    void toggleSound(bool);

    private:
    // load the cartridge at fileName onto the Gameboy.
    void loadRom(const QString &fileName);

    // Create a new menu in the toolbar.
    QMenu *createMenu(const QString &name);

    // Add a button to a menu that calls slot when clicked.
    QAction *createSingleAction(const QString &name,
                                QMenu *menu,
                                void (MainWindow::*slot)());

    // Add a toggle-able button to a menu that toggles *slot when clicked.
    QAction *createCheckableAction(const QString &name,
                                   QMenu *menu,
                                   void (MainWindow::*slot)(bool),
                                   bool initallyChecked = false);

    // Create all the actions performed by the buttons in the toolbar.
    void createActions();

    std::shared_ptr<qtboy::Gameboy> system_;

    // Display for the Gameboy. Displays the contents output by the renderer.
    QLabel *display_;

    // Produces graphical output for the display.
    Qt_renderer *renderer_ {nullptr};

    // Produces audio output.
    std::shared_ptr<Qt_speaker> speaker_ {nullptr};

    // Timer that calls updateFps() every second
    QTimer *fpsTimer_;

    // Base title of the MainWindow (i.e. not including FPS counter)
    QString title_;

    // Frames drawn since the last call to updateFps()
    int frames_;

    // Title of the current ROM loaded
    QString curRom;

    // Saved preferences for options
    Preferences prefs_;

    // Control mapping
    Controls controls_;

    /*
    // Windows that can be opened through toolbar actions. An instance is only created on the first
    // open request. Subsequent open/close requests only show/hide the window.
    DebuggerWindow *debuggerWindow_ {nullptr};
    VramWindow *vramWindow_ {nullptr};
    */

};

#endif // MAINWINDOW_H
