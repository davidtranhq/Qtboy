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
    stack = new QPlainTextEdit;
    stack->setReadOnly(true);
    stack->setWordWrapMode(QTextOption::NoWrap);
    QFont f {"Courier New"};
    stack->setFont(f);
    stack->setPlainText(QString::fromStdString(system.dump_formatted_memory(gameboy::Dump_format::Stack)));
}

void Debugger_window::createMemoryViewer()
{
    memory = new QPlainTextEdit;
    memory->setReadOnly(true);
    memory->setWordWrapMode(QTextOption::NoWrap);
    QFont f {"Courier New"};
    memory->setFont(f);
    memory->setPlainText(QString::fromStdString(system.dump_formatted_memory()));
}

void Debugger_window::createInstructionViewer()
{
    rom = new QPlainTextEdit;
    rom->setReadOnly(true);
    rom->setWordWrapMode(QTextOption::NoWrap);
    QFont romFont {"Courier New"};
    rom->setFont(romFont);
    rom->setPlainText(QString::fromStdString(ds.hex_dump(system.dump_memory())));
}

void Debugger_window::createLayout()
{
    auto *run = new QPushButton(tr("Run"));
    auto *step = new QPushButton(tr("Step"));
    auto *stepN = new QPushButton(tr("Step n"));
    steps = new QLineEdit;
    auto *reset = new QPushButton(tr("Reset"));
    auto *viewFrameBuffer = new QPushButton(tr("View frame buffer"));
    auto *pause = new QPushButton(tr("Pause"));
    auto *breakpoint = new QPushButton(tr("Set breakpoint"));
    enableViewerUpdate = new QCheckBox(tr("Update viewers"));

    connect(run, SIGNAL(clicked()), this, SLOT(runCpu()));
    connect(step, SIGNAL(clicked()), this, SLOT(stepCpu()));
    connect(stepN, SIGNAL(clicked()), this, SLOT(stepNCpu()));
    connect(reset, SIGNAL(clicked()), this, SLOT(resetSystem()));
    connect(viewFrameBuffer, SIGNAL(clicked()), this, SLOT(showFrameBuffer()));
    connect(pause, SIGNAL(clicked()), this, SLOT(pauseCpu()));
    connect(breakpoint, SIGNAL(clicked()), this, SLOT(setBreakpoint()));

    af = new QLabel;
    bc = new QLabel;
    de = new QLabel;
    hl = new QLabel;
    sp = new QLabel;
    pc = new QLabel;
    cycles = new QLabel;
    stepsTakenL = new QLabel;

    QGridLayout *controls {new QGridLayout};
    controls->addWidget(run, 0, 0);
    controls->addWidget(pause, 0, 1);
    controls->addWidget(step, 1, 0);
    controls->addWidget(reset, 1, 1);
    controls->addWidget(stepN, 2, 0);
    controls->addWidget(steps, 2, 1);
    controls->addWidget(viewFrameBuffer, 3, 0);
    controls->addWidget(enableViewerUpdate, 3, 1);


    QVBoxLayout *cpuInfo {new QVBoxLayout};
    cpuInfo->addWidget(af);
    cpuInfo->addWidget(bc);
    cpuInfo->addWidget(de);
    cpuInfo->addWidget(hl);
    cpuInfo->addWidget(sp);
    cpuInfo->addWidget(pc);
    cpuInfo->addWidget(cycles);
    cpuInfo->addWidget(stepsTakenL);

    createInstructionViewer();
    createMemoryViewer();
    createStackViewer();

    QVBoxLayout *sidebar {new QVBoxLayout};
    sidebar->addLayout(controls);
    sidebar->addLayout(cpuInfo);

    QHBoxLayout *top {new QHBoxLayout};
    top->addLayout(sidebar, 30);
    top->addWidget(rom, 70);

    QVBoxLayout *left {new QVBoxLayout};
    left->addLayout(top, 30);
    left->addWidget(memory, 70);

    QHBoxLayout *mainLayout {new QHBoxLayout};
    mainLayout->addLayout(left, 80);
    mainLayout->addWidget(stack, 20);

    updateCpuLabels();
    setLayout(mainLayout);
}

void Debugger_window::highlightByte()
{
    uint16_t nByte = system.dump_cpu().pc;
    QPlainTextEdit *view = rom;
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

void Debugger_window::highlightStack()
{
    uint16_t nByte = system.dump_cpu().sp;
    QPlainTextEdit *view = stack;
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

void Debugger_window::updateCpuLabels()
{
    gameboy::Cpu_values dump {system.dump_cpu()};
    af->setText(QStringLiteral("AF: %1").arg(dump.af, 4, 16, QChar('0')));
    bc->setText(QStringLiteral("BC: %1").arg(dump.bc, 4, 16, QChar('0')));
    de->setText(QStringLiteral("DE: %1").arg(dump.de, 4, 16, QChar('0')));
    hl->setText(QStringLiteral("HL: %1").arg(dump.hl, 4, 16, QChar('0')));
    sp->setText(QStringLiteral("SP: %1").arg(dump.sp, 4, 16, QChar('0')));
    pc->setText(QStringLiteral("PC: %1").arg(dump.pc, 4, 16, QChar('0')));
    cycles->setText(QStringLiteral("Cycles: %1").arg(dump.cycles));
    stepsTakenL->setText(QStringLiteral("Steps taken: %1").arg(stepsTaken));
}

void Debugger_window::updateViewers()
{
    if (system.memory_changed())
    {
        int scroll = memory->verticalScrollBar()->value();
        memory->setPlainText(QString::fromStdString(system.dump_formatted_memory()));
        memory->verticalScrollBar()->setValue(scroll);

        int scroll2 = rom->verticalScrollBar()->value();
        rom->setPlainText(QString::fromStdString(ds.hex_dump(system.dump_memory())));
        rom->verticalScrollBar()->setValue(scroll2);

        int scroll3 = stack->verticalScrollBar()->value();
        stack->setPlainText(QString::fromStdString(system.dump_formatted_memory(gameboy::Dump_format::Stack)));
        stack->verticalScrollBar()->setValue(scroll3);
    }
    highlightByte();
    highlightStack();
}

void Debugger_window::runCpu()
{
    pause = false;
    for (;;)
    {
        if (pause)
            break;
        QCoreApplication::processEvents();
        system.step(1);
        ++stepsTaken;
        updateCpuLabels();
        if (enableViewerUpdate->isChecked())
            updateViewers();
    }
}

void Debugger_window::stepCpu()
{
    system.step(1);
    ++stepsTaken;
    updateCpuLabels();
    if (enableViewerUpdate->isChecked())
        updateViewers();
}

void Debugger_window::stepNCpu()
{
    size_t n {steps->text().toULongLong()};
    system.step(n);
    stepsTaken += n;
    updateCpuLabels();
    if (enableViewerUpdate->isChecked())
        updateViewers();
}

void Debugger_window::showFrameBuffer()
{
    frameBuffer = new QLabel(this, Qt::Window);
    system.draw_framebuffer();
    frameBuffer->setPixmap(QPixmap::fromImage(display.image()));
    frameBuffer->show();
}

void Debugger_window::resetSystem()
{
    system.reset();
    stepsTaken = 0;
    updateCpuLabels();
    if (enableViewerUpdate->isChecked())
        updateViewers();
}

void Debugger_window::pauseCpu()
{
    pause = true;
}
