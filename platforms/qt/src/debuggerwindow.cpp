#include "debuggerwindow.h"
#include "qt_renderer.h"
#include "debug_types.hpp"

#include <QTextCursor>
#include <QScrollBar>
#include <QCoreApplication>
#include <QMessageBox>
#include <QCheckBox>

#include <sstream>
#include <iomanip>
#include <cstdlib>

Debugger_window::Debugger_window(QWidget *parent, gameboy::System *s)
    : QWidget {parent, Qt::Window},
      debugger_ {s},
      disassembler_ {}
{
    create_layout();
}

void Debugger_window::create_stack_viewer()
{
    stack_ = new QPlainTextEdit;
    stack_->setReadOnly(true);
    stack_->setWordWrapMode(QTextOption::NoWrap);
    QFont f {"Courier New"};
    stack_->setFont(f);
    stack_->setPlainText(QString::fromStdString(debugger_.dump_formatted_memory(gameboy::Dump_format::Stack)));
}

void Debugger_window::create_memory_viewer()
{
    memory_ = new QPlainTextEdit;
    memory_->setReadOnly(true);
    memory_->setWordWrapMode(QTextOption::NoWrap);
    QFont f {"Courier New"};
    memory_->setFont(f);
    memory_->setPlainText(QString::fromStdString(debugger_.dump_formatted_memory()));
}

void Debugger_window::create_instruction_viewer()
{
    rom_ = new QPlainTextEdit;
    rom_->setReadOnly(true);
    rom_->setWordWrapMode(QTextOption::NoWrap);
    QFont romFont {"Courier New"};
    rom_->setFont(romFont);
    rom_->setPlainText(QString::fromStdString(disassembler_.hex_dump(debugger_.dump_memory())));
}

void Debugger_window::create_layout()
{
    auto *run = new QPushButton(tr("Run"));
    auto *step = new QPushButton(tr("Step"));
    auto *stepN = new QPushButton(tr("Step n"));
    steps_to_take_ = new QLineEdit;
    auto *reset = new QPushButton(tr("Reset"));
    auto *viewFrameBuffer = new QPushButton(tr("View frame buffer"));
    auto *pause = new QPushButton(tr("Pause"));
    auto *breakpoint = new QPushButton(tr("Set breakpoint"));
    enable_viewer_update_ = new QCheckBox(tr("Update viewers"));
    enable_log_ = new QCheckBox(tr("Trace log"));

    connect(run, SIGNAL(clicked()), this, SLOT(run()));
    connect(step, SIGNAL(clicked()), this, SLOT(step()));
    connect(stepN, SIGNAL(clicked()), this, SLOT(stepN()));
    connect(reset, SIGNAL(clicked()), this, SLOT(reset()));
    connect(viewFrameBuffer, SIGNAL(clicked()), this, SLOT(show_frame_buffer()));
    connect(pause, SIGNAL(clicked()), this, SLOT(pause()));
    connect(breakpoint, SIGNAL(clicked()), this, SLOT(set_breakpoint()));
    connect(enable_log_, SIGNAL(stateChanged(int)), this, SLOT(toggle_log(int)));

    af_ = new QLabel;
    bc_ = new QLabel;
    de_ = new QLabel;
    hl_ = new QLabel;
    sp_ = new QLabel;
    pc_ = new QLabel;
    cycles_ = new QLabel;
    steps_ = new QLabel;

    QGridLayout *controls {new QGridLayout};
    controls->addWidget(run, 0, 0);
    controls->addWidget(pause, 0, 1);
    controls->addWidget(step, 1, 0);
    controls->addWidget(reset, 1, 1);
    controls->addWidget(stepN, 2, 0);
    controls->addWidget(steps_to_take_, 2, 1);
    controls->addWidget(viewFrameBuffer, 3, 0);
    controls->addWidget(enable_viewer_update_, 3, 1);
    controls->addWidget(enable_log_, 4, 0);


    QVBoxLayout *cpuInfo {new QVBoxLayout};
    cpuInfo->addWidget(af_);
    cpuInfo->addWidget(bc_);
    cpuInfo->addWidget(de_);
    cpuInfo->addWidget(hl_);
    cpuInfo->addWidget(sp_);
    cpuInfo->addWidget(pc_);
    cpuInfo->addWidget(cycles_);
    cpuInfo->addWidget(steps_);

    create_instruction_viewer();
    create_stack_viewer();
    create_memory_viewer();

    QVBoxLayout *sidebar {new QVBoxLayout};
    sidebar->addLayout(controls);
    sidebar->addLayout(cpuInfo);

    QHBoxLayout *top {new QHBoxLayout};
    top->addLayout(sidebar, 30);
    top->addWidget(rom_, 70);

    QVBoxLayout *left {new QVBoxLayout};
    left->addLayout(top, 30);
    left->addWidget(memory_, 70);

    QHBoxLayout *mainLayout {new QHBoxLayout};
    mainLayout->addLayout(left, 80);
    mainLayout->addWidget(stack_, 20);

    update_labels();
    setLayout(mainLayout);
}

void Debugger_window::highlight_byte()
{
    uint16_t nByte = debugger_.dump_cpu().pc;
    QPlainTextEdit *view = rom_;
    std::ostringstream adr;
    adr << std::hex << std::setfill('0') << std::setw(4)
        << nByte;
    QTextCursor cursor = view->textCursor();
    QTextCursor savedCursor = cursor; // save the current position
    QString current_pc {QString::fromStdString(adr.str())};
    if (!view->find(current_pc))
    {
        // found nothing, jump to the start
        cursor.movePosition(QTextCursor::Start);
        view->setTextCursor(cursor);
        if (!view->find(current_pc))
        {
            // no match in the document
            QMessageBox msgBox;
            msgBox.setText(tr("Something may be wrong. The PC doesn't point to"
                              "the start of an instruction."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            // word not found : we set the cursor back to its initial position
            view->setTextCursor(savedCursor);
        }
    }
    QTextCursor current = view->textCursor();
    current.select(QTextCursor::LineUnderCursor);
    QTextCharFormat fmt;
    fmt.setBackground(Qt::cyan);
    QTextEdit::ExtraSelection selection {current, fmt};
    view->setExtraSelections({selection});

    current.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    view->setTextCursor(current);
}

void Debugger_window::highlight_stack()
{
    uint16_t nByte = debugger_.dump_cpu().sp;
    QPlainTextEdit *view = stack_;
    std::ostringstream adr;
    adr << std::hex << std::setfill('0') << std::setw(4)
        << nByte;
    QTextCursor cursor = view->textCursor();
    QTextCursor savedCursor = cursor; // save the current position
    QString current_sp {QString::fromStdString(adr.str())};
    if (!view->find(current_sp))
    {
        // found nothing, jump to the start
        cursor.movePosition(QTextCursor::Start);
        view->setTextCursor(cursor);
        if (!view->find(current_sp))
        {
            // no match in the document
            QMessageBox msgBox;
            msgBox.setText(tr("Something may be wrong. The SP doesn't point to"
                              "a valid address."));
            msgBox.setStandardButtons(QMessageBox::Ok);
            msgBox.setDefaultButton(QMessageBox::Ok);
            msgBox.exec();

            // word not found : we set the cursor back to its initial position
            view->setTextCursor(savedCursor);
        }
    }
    QTextCursor current = view->textCursor();
    current.select(QTextCursor::LineUnderCursor);
    QTextCursor current_hi = view->textCursor();
    current_hi.movePosition(QTextCursor::Up, QTextCursor::MoveAnchor);
    current_hi.select(QTextCursor::LineUnderCursor);
    QTextCharFormat fmt;
    fmt.setBackground(Qt::cyan);
    QTextEdit::ExtraSelection selection {current, fmt};
    QTextEdit::ExtraSelection selection2 {current_hi, fmt};
    view->setExtraSelections({selection, selection2});

    current.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);
    view->setTextCursor(current);
}

void Debugger_window::update_labels()
{
    gameboy::Cpu_values dump {debugger_.dump_cpu()};
    af_->setText(QStringLiteral("AF: %1").arg(dump.af, 4, 16, QChar('0')));
    bc_->setText(QStringLiteral("BC: %1").arg(dump.bc, 4, 16, QChar('0')));
    de_->setText(QStringLiteral("DE: %1").arg(dump.de, 4, 16, QChar('0')));
    hl_->setText(QStringLiteral("HL: %1").arg(dump.hl, 4, 16, QChar('0')));
    sp_->setText(QStringLiteral("SP: %1").arg(dump.sp, 4, 16, QChar('0')));
    pc_->setText(QStringLiteral("PC: %1").arg(dump.pc, 4, 16, QChar('0')));
    cycles_->setText(QStringLiteral("Cycles: %1").arg(dump.cycles));
    steps_->setText(QStringLiteral("Steps taken: %1").arg(debugger_.steps()));
}

void Debugger_window::update_viewers()
{
    int scroll = memory_->verticalScrollBar()->value();
    memory_->setPlainText(QString::fromStdString(debugger_.dump_formatted_memory()));
    memory_->verticalScrollBar()->setValue(scroll);

    int scroll2 = rom_->verticalScrollBar()->value();
    rom_->setPlainText(QString::fromStdString(disassembler_.hex_dump(debugger_.dump_memory())));
    rom_->verticalScrollBar()->setValue(scroll2);

    int scroll3 = stack_->verticalScrollBar()->value();
    stack_->setPlainText(QString::fromStdString(debugger_.dump_formatted_memory(gameboy::Dump_format::Stack)));
    stack_->verticalScrollBar()->setValue(scroll3);
    highlight_byte();
    highlight_stack();
}

void Debugger_window::run()
{
    /*
    update_labels();
    if (enable_viewer_update_->isChecked())
        update_viewers();
    */
    debugger_.run();
}

void Debugger_window::step()
{
    debugger_.step(1);
    update_labels();
    if (enable_viewer_update_->isChecked())
        update_viewers();
}

void Debugger_window::stepN()
{
    size_t n {steps_to_take_->text().toULongLong()};
    debugger_.step(n);
    update_labels();
    if (enable_viewer_update_->isChecked())
        update_viewers();
}

void Debugger_window::show_frame_buffer()
{
}

void Debugger_window::reset()
{
    debugger_.reset();
    update_labels();
    if (enable_viewer_update_->isChecked())
        update_viewers();
}

void Debugger_window::pause()
{
    debugger_.pause();
}

void Debugger_window::toggle_log(int state)
{
    debugger_.enable_logging(state);
}
