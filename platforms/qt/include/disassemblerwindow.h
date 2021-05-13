#ifndef DISASSEMBLERWINDOW_H
#define DISASSEMBLERWINDOW_H
/*
#include "system.hpp"
#include "disassembler.hpp"
#include "debugger.hpp"

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLayout>
#include <QLineEdit>

class DisassemblerWindow : public QWidget
{
    Q_OBJECT

    public:
    explicit DisassemblerWindow(std::shared_ptr<qtboy::Gameboy> g,
                                QWidget *parent = nullptr);

    signals:

    private slots:
    void goToAddress();

private:
    qtboy::Debugger debugger_;
    qtboy::Disassembler dsmblr;
    QVBoxLayout *mainLayout;
    QHBoxLayout *goToLayout;
    QLineEdit *goToPrompt;
    QPushButton *goToButton;
    QTextEdit *textArea;
};
*/
#endif // DISASSEMBLERWINDOW_H
