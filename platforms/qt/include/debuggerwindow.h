#ifndef DEBUGGERWINDOW_H
#define DEBUGGERWINDOW_H
/*
#include <QWidget>

#include <memory> // std::shared_ptr

#include "debugger.hpp"

class QLabel;
class QLineEdit;
class QCheckBox;

namespace qtboy { class Gameboy; }

class MemoryViewer;

class DebuggerWindow : public QWidget
{
    Q_OBJECT

    public:
    // Create a DebuggerWindow for a specified Gameboy system.
    explicit DebuggerWindow(std::shared_ptr<qtboy::Gameboy> g,
                            QWidget *parent = nullptr);

    protected:
    // closeEvent is overriden to turn off debug mode and resume the Gameboy.
    void closeEvent(QCloseEvent *) override;

    private slots:
    // Pause the Gameboy.
    void pause();

    // Run the Gameboy, ignoring any breakpoints.
    void run_no_break();

    // Run the Gameboy, breaking at any set breakpoints.
    void run_until_break();

    // Take the amount of CPU steps specified by steps_to_take_.
    void step();

    // Reset the debugger.
    void reset();

    // Open a window for the management of breakpoints.
    void openBreakpointWindow();

    // Enable/disable CPU trace logging.
    void toggle_log(int state);

    // Update information to be displayed in the debugger.
    void update_info(const qtboy::Cpu_dump &,
                     const QString &disassembly,
                     const QString &memory,
                     const QString &stack);

    private:
    // Create the visual layout of buttons, memory viewers, and inputs.
    void create_layout();

    // Used for system control (stepping, running, pausing) and dumping debug information.
    std::shared_ptr<qtboy::Gameboy> system_ {};

    // Used for setting breakpoints.
    qtboy::Debugger debugger_;

    // text displays for CPU registers, cycles, and steps.
    QLabel *af_, *bc_, *de_, *hl_, *sp_, *pc_, *cycles_,
           *steps_;

    // Input to specify the amount of CPU steps to take when stepping with the debugger.
    QLineEdit *steps_to_take_;

    // Viewer for ROM, mapped memory, and stack information.
    MemoryViewer *rom_viewer_, *memory_viewer_, *stack_viewer_;

    // Option to enable/disable CPU trace logging.
    QCheckBox *enable_log_;

};
*/

#endif // DEBUGGERWINDOW_H
