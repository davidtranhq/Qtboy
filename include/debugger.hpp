#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>
#include <memory>

#include "debug_types.hpp"
#include "instruction_info.hpp"
#include "ppu.hpp"
#include "disassembler.hpp"

namespace qtboy
{

class Gameboy;

class Debugger
{
    public:
    // Construct a debugger attached to a Gameboy.
    explicit Debugger(std::shared_ptr<Gameboy> g);
    ~Debugger();

    // Enables or disables debug mode on the attached system.
    void set_debug_mode(bool b);

    // Passed to the system to be called after every CPU instruction.
    bool cpu_callback();

    // Passed to the system to be called after every memory write.
    void memory_callback(uint8_t b, uint16_t adr);

    // Run until a breakpoint is reached (continue).
    void run_until_break();

    // Run, ignoring breakpoints.
    void run_no_break();

    // Adds a breakpoint at the specified address (0000h-ffffh)
    void add_breakpoint(uint16_t);

    // Deletes the breakpoint at the specified address. If there is no breakpoint,
    // nothing happens.
    void delete_breakpoint(uint16_t);

    // Get all currently set breakpoints.
    const std::unordered_map<uint16_t, bool> &breakpoints() const;

    // Sets whether or not a CPU trace is output after every CPU instruction.
    void set_logging(bool b);

    // Sets the path for the output of the CPU trace. Returns true if the log file is valid.
    bool set_log_file(const std::string &path);

    // Writes a CPU trace to the file specified by set_log_file().
    void write_log();

    // Get how many CPU steps have been taken.
    size_t steps() const;

    // Disassemble everything currently mapped in memory.
    std::vector<Assembly> disassemble() const;

    // Reset the debugger and the system.
    void reset();

    void update_memory_cache() const;
    std::vector<uint8_t> dump_memory() const;
    std::string dump_formatted_memory(Dump_format d = Dump_format::Hex) const;


    private:
    // Generate a CPU trace
    std::string log();

    // Check if execution is currently at a breakpoint.
    bool at_breakpoint();

    private:
    std::shared_ptr<Gameboy> system_ {nullptr};
    Disassembler disassembler_ {};
    std::unordered_map<uint16_t, bool> breaks_ {false};
    size_t steps_ {0};
    bool paused_ {false};
    bool logging_ {false};
    bool breaking_ {true};
    std::ofstream log_file_;
    // memory map cache
    mutable std::unordered_map<std::string, Memory_range> memory_map_ {};
};

}

#endif // DEBUGGER_HPP
