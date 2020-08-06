#include "debugger_thread.h"
#include "debugger.hpp"
#include "disassembler.hpp"

Debugger_thread::Debugger_thread(gameboy::Debugger *d,
                                 gameboy::Disassembler *ds)
    : debugger_ {d},
      disassembler_ {ds}
{
    if (!d)
        throw std::runtime_error("Unable to create Debugger_thread: "
                                 "invalid pointer to Debugger");
    if (!ds)
        throw std::runtime_error("Unable to create Debugger_thread: "
                                 "invalid pointer to Disassembler");
}

[[ noreturn ]]
void Debugger_thread::run()
{
    while (true)
    {
        auto dump = debugger_->dump_cpu();
        auto disassembly(QString::fromStdString(
            disassembler_->pretty_disassemble(debugger_->dump_memory())));
        auto memory(QString::fromStdString(
            debugger_->dump_formatted_memory()));
        auto stack(QString::fromStdString(
            debugger_->dump_formatted_memory(gameboy::Dump_format::Stack)));
        emit info_ready(dump, disassembly, memory, stack);
        msleep(100);
    }
}
