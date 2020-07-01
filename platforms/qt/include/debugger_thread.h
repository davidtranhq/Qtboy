#ifndef WORKER_H
#define WORKER_H

#include <QThread>

namespace gameboy
{
    class Debugger;
    class Disassembler;
    struct Cpu_dump;
}

class Debugger_thread : public QThread
{
    Q_OBJECT

    public:
    explicit Debugger_thread(gameboy::Debugger *d,
                             gameboy::Disassembler *ds);

    protected:
    [[ noreturn ]]
    void run() override;

    signals:
    void info_ready(const gameboy::Cpu_dump &,
                    const QString &disassembly,
                    const QString &memory,
                    const QString &stack);

    private:
    gameboy::Debugger *debugger_;
    gameboy::Disassembler *disassembler_;
};

#endif // WORKER_H
