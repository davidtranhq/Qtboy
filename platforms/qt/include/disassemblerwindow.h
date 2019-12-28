#ifndef DISASSEMBLERWINDOW_H
#define DISASSEMBLERWINDOW_H

#include "system.hpp"
#include "disassembler.hpp"

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLayout>
#include <QLineEdit>

class DisassemblerWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DisassemblerWindow(QWidget *parent, gameboy::System &s);

signals:

private slots:
    void goToAddress();

private:
    gameboy::System &system;
    gameboy::Disassembler dsmblr;
    QVBoxLayout *mainLayout;
    QHBoxLayout *goToLayout;
    QLineEdit *goToPrompt;
    QPushButton *goToButton;
    QTextEdit *textArea;
};

#endif // DISASSEMBLERWINDOW_H
