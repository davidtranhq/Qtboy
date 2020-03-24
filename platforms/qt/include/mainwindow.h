#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "system.hpp"
#include "qt_renderer.h"

#include <QMainWindow>
#include <chrono>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QImage;
class QLabel;
QT_END_NAMESPACE

class DisassemblerWindow;
class DebuggerWindow;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    void loadRom(const QString &fileName);

protected:
    void keyPressEvent(QKeyEvent *);
    void keyReleaseEvent(QKeyEvent *);
    void closeEvent(QCloseEvent *);

private slots:
    void openRom();
    void showDisassembler();
    void showDebugger();
    void showVramViewer();
    void about();

    void update_display();

private:
    void createActions();

    gameboy::System system;
    Qt_renderer *renderer_ {nullptr};
    QLabel *display;
    QString curRom;
    std::chrono::high_resolution_clock::time_point start_;

};

#endif // MAINWINDOW_H
