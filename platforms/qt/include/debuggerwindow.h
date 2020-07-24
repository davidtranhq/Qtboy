#ifndef DEBUGGERWINDOW_H
#define DEBUGGERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>
#include <QThread>

#include "disassembler.hpp"
#include "debugger_thread.h"

namespace gameboy
{
    class Debugger;
}

class Memory_viewer;

class Debugger_window : public QWidget
{
    Q_OBJECT

    public:
    explicit Debugger_window(QWidget *parent, gameboy::Debugger *d);

    protected:
    void closeEvent(QCloseEvent *event) override;

    private slots:
    void run_no_break();
    void run_until_break();
    void step();
    void stepN();
    void reset();
    void openBreakpointWindow();
    void pause();
    void toggle_log(int state);

    void update_info(const gameboy::Cpu_dump &,
                     const QString &disassembly,
                     const QString &memory,
                     const QString &stack);

    private:
    void create_layout();

    gameboy::Debugger *debugger_;
    gameboy::Disassembler disassembler_;
    Debugger_thread debug_thread_ {debugger_, &disassembler_};

    QLabel *af_, *bc_, *de_, *hl_, *sp_, *pc_, *cycles_,
           *steps_;
    QLineEdit *steps_to_take_;
    Memory_viewer *rom_viewer_, *memory_viewer_, *stack_viewer_;
    QCheckBox *enable_log_;

};

#endif // DEBUGGERWINDOW_H
