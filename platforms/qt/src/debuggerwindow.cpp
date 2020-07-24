#include "debuggerwindow.h"
#include "debugger.hpp"
#include "qt_renderer.h"
#include "debug_types.hpp"
#include "breakpoint_window.h"
#include "debugger_thread.h"

#include <QTextCursor>
#include <QScrollBar>
#include <QCoreApplication>
#include <QMessageBox>
#include <QCheckBox>

#include <sstream>
#include <iomanip>
#include <cstdlib>

#include "memory_viewer.h"

Q_DECLARE_METATYPE(gameboy::Cpu_dump);

Debugger_window::Debugger_window(QWidget *parent, gameboy::Debugger *d)
    : QWidget {parent, Qt::Window},
      debugger_ {d},
      disassembler_ {},
      rom_viewer_(new Memory_viewer(this)),
      memory_viewer_(new Memory_viewer(this)),
      stack_viewer_(new Memory_viewer(this))
{
    if (!d)
        throw std::runtime_error("Unable to start debugger: Invalid pointer");
    // enable debugging on system and pause the emulator when window is opened
    debugger_->enable_debug(true);
    debugger_->pause();
    // create widgets, connect signals and slots
    create_layout();
    // run seperate debug thread to continuously check for updates
    qRegisterMetaType<gameboy::Cpu_dump>();
    connect(&debug_thread_, &Debugger_thread::info_ready,
            this, &Debugger_window::update_info);
    debug_thread_.start();
}

void Debugger_window::closeEvent(QCloseEvent *event)
{
    debugger_->enable_debug(false);
    // resume emulator when window is closed
    debugger_->run_no_break();
}

void Debugger_window::create_layout()
{
    auto *run = new QPushButton(tr("Run"));
    auto *run_until_break = new QPushButton(tr("Run until break"));
    auto *step = new QPushButton(tr("Step"));
    auto *stepN = new QPushButton(tr("Step n"));
    steps_to_take_ = new QLineEdit;
    auto *reset = new QPushButton(tr("Reset"));
    auto *breakpoints = new QPushButton(tr("Breakpoints"));
    auto *pause = new QPushButton(tr("Pause"));
    enable_log_ = new QCheckBox(tr("Trace log"));

    // buttons (controls)
    connect(run, SIGNAL(clicked()), this, SLOT(run_no_break()));
    connect(run_until_break, SIGNAL(clicked()), this, SLOT(run_until_break()));
    connect(step, SIGNAL(clicked()), this, SLOT(step()));
    connect(stepN, SIGNAL(clicked()), this, SLOT(stepN()));
    connect(reset, SIGNAL(clicked()), this, SLOT(reset()));
    connect(breakpoints, SIGNAL(clicked()), this, SLOT(openBreakpointWindow()));
    connect(pause, SIGNAL(clicked()), this, SLOT(pause()));
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
    controls->addWidget(run_until_break, 0, 1);
    controls->addWidget(pause, 1, 0);
    controls->addWidget(step, 1, 1);
    controls->addWidget(reset, 2, 0);
    controls->addWidget(breakpoints, 2, 1);
    controls->addWidget(enable_log_, 3, 0);


    QVBoxLayout *cpuInfo {new QVBoxLayout};
    cpuInfo->addWidget(af_);
    cpuInfo->addWidget(bc_);
    cpuInfo->addWidget(de_);
    cpuInfo->addWidget(hl_);
    cpuInfo->addWidget(sp_);
    cpuInfo->addWidget(pc_);
    cpuInfo->addWidget(cycles_);
    cpuInfo->addWidget(steps_);

    QVBoxLayout *sidebar {new QVBoxLayout};
    sidebar->addLayout(controls);
    sidebar->addLayout(cpuInfo);

    QHBoxLayout *top {new QHBoxLayout};
    top->addLayout(sidebar, 30);
    top->addWidget(rom_viewer_, 70);

    QVBoxLayout *left {new QVBoxLayout};
    left->addLayout(top, 30);
    left->addWidget(memory_viewer_, 70);

    QHBoxLayout *mainLayout {new QHBoxLayout};
    mainLayout->addLayout(left, 80);
    mainLayout->addWidget(stack_viewer_, 20);

    setLayout(mainLayout);
}

void Debugger_window::update_info(const gameboy::Cpu_dump &dump,
                                  const QString &disassembly,
                                  const QString &memory,
                                  const QString &stack)
{
    // update CPU labels (registers)
    af_->setText(QStringLiteral("AF: %1").arg(dump.af, 4, 16, QChar('0')));
    bc_->setText(QStringLiteral("BC: %1").arg(dump.bc, 4, 16, QChar('0')));
    de_->setText(QStringLiteral("DE: %1").arg(dump.de, 4, 16, QChar('0')));
    hl_->setText(QStringLiteral("HL: %1").arg(dump.hl, 4, 16, QChar('0')));
    sp_->setText(QStringLiteral("SP: %1").arg(dump.sp, 4, 16, QChar('0')));
    pc_->setText(QStringLiteral("PC: %1").arg(dump.pc, 4, 16, QChar('0')));
    cycles_->setText(QStringLiteral("Cycles: %1").arg(dump.cycles));
    steps_->setText(QStringLiteral("Steps taken: %1").arg(debugger_->steps()));

    // update contents in memory viewers
    rom_viewer_->update(disassembly);
    stack_viewer_->update(stack);
    memory_viewer_->update(memory);
    // select the lines of interest (PC and SP)
    rom_viewer_->make_selection(dump.pc, Qt::cyan);
    stack_viewer_->make_selection(dump.sp, Qt::cyan);
    // select lines with breakpoints
    std::unordered_map<uint16_t, bool> breakpoints = debugger_->breakpoints();
    for (const auto &b : breakpoints)
        if (b.second)
            rom_viewer_->make_selection(b.first, Qt::red);
    // higlight selected lines
    rom_viewer_->highlight_lines();
    stack_viewer_->highlight_lines();
}

void Debugger_window::run_no_break()
{
    debugger_->run_no_break();
}

void Debugger_window::run_until_break()
{
    debugger_->run_until_break();
}

void Debugger_window::step()
{
    debugger_->step(1);
}

void Debugger_window::stepN()
{
    size_t n {steps_to_take_->text().toULongLong()};
    debugger_->step(n);
}

void Debugger_window::reset()
{
    debugger_->reset();
}

void Debugger_window::pause()
{
    debugger_->pause();
}

void Debugger_window::openBreakpointWindow()
{
    auto *bp_win = new Breakpoint_window(debugger_);
    bp_win->show();
}

void Debugger_window::toggle_log(int state)
{
    debugger_->enable_logging(state);
}
