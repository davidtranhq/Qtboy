#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "system.hpp"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QImage;
class QLabel;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void loadRom(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openRom();
    void showDisassembler();
    void about();

private:
    void createActions();

    gameboy::System system;
    QImage *display;
    QWidget *displayArea;
    QLabel *displayLabel;
    QString curRom;
};

#endif // MAINWINDOW_H
