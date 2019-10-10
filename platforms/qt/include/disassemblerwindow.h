#ifndef DISASSEMBLERWINDOW_H
#define DISASSEMBLERWINDOW_H

#include "system.hpp"

#include <QWidget>
#include <QTextEdit>

class DisassemblerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DisassemblerWindow(gameboy::System &s);

signals:

public slots:

private:
    gameboy::System &system;
    QTextEdit *textArea;
};

#endif // DISASSEMBLERWINDOW_H
