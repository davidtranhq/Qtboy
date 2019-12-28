#ifndef DEBUGGERWINDOW_H
#define DEBUGGERWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QCheckBox>

#include <string>
#include <vector>

#include "debugger.hpp"
#include "disassembler.hpp"

class Debugger_window : public QWidget
{
    Q_OBJECT
public:
    explicit Debugger_window(QWidget *parent, gameboy::System *s);

private slots:
    void run();
    void step();
    void stepN();
    void reset();
    void show_frame_buffer();
    void pause();

private:
    void create_stack_viewer();
    void create_memory_viewer();
    void create_instruction_viewer();
    void create_layout();

    void update_viewers();
    void update_labels();
    void highlight_byte();
    void highlight_stack();

    gameboy::Debugger debugger_;
    gameboy::Disassembler disassembler_;
    size_t steps_taken_;

    QLabel *af_, *bc_, *de_, *hl_, *sp_, *pc_, *cycles_,
           *steps_;
    QLineEdit *steps_to_take_;
    QPlainTextEdit *rom_, *memory_, *stack_;
    QCheckBox *enable_viewer_update_;

};

#endif // DEBUGGERWINDOW_H
