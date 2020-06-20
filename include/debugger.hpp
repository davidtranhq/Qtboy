#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>

#include "debug_types.hpp"
#include "instruction_info.hpp"
#include "ppu.hpp"
#include "disassembler.hpp"

namespace gameboy
{

class System;

class Debugger
{
    public:
    explicit Debugger(System *);

    void run();
    void run_concurrently();
    void run_until_break();
    void pause();
    void step(size_t n = 1);
    void reset();
    void add_breakpoint(uint16_t);
    void delete_breakpoint(uint16_t);
    void enable_logging(bool b = true);
    bool set_log_file(const std::string &);
    void write_log(); // write log to file
    std::string log(); // return log as string
    size_t steps() const;

    void update_memory_cache() const;
    std::vector<uint8_t> dump_memory() const;
    std::unordered_map<std::string, Memory_range> dump_mapped_memory() const;
    std::string dump_formatted_memory(Dump_format d = Dump_format::Hex) const;
    std::vector<uint8_t> dump_rom() const noexcept;
    Cpu_values dump_cpu() const noexcept;
    Ppu::Dump dump_ppu() const noexcept;
    std::array<Palette, 8> dump_bg_palettes() const;
    std::array<Palette, 8> dump_sprite_palettes() const;
    std::array<Texture, 384> dump_tileset(uint8_t bank);
    Texture dump_framebuffer(bool with_bg = true, bool with_win = true,
                             bool with_sprites = true) const;
    Texture dump_background() const;
    Texture dump_window() const;
    std::array<Texture, 40> dump_sprites();

    std::string hex_dump() const;
    Assembly disassemble_op() const; // disassemble next instruction
    Assembly disassemble_op(uint16_t adr) const; // disassemble instruction at
    std::vector<Assembly> disassemble() const; // disassemble all

    private:
    void step_callback(); // called after each CPU step
    bool at_breakpoint();
    std::string parse_operand(const std::array<uint8_t, 3> &ops,
                              const std::string &operand,
                              uint16_t adr) const;

    private:
    System *system_ {nullptr};
    Disassembler disassembler_ {};
    std::array<bool, 0x10000> breaks_ {false};
    size_t steps_ {0};
    bool paused_ {false};
    bool log_ {false};
    std::ofstream log_file_;
    // memory map cache
    mutable std::unordered_map<std::string, Memory_range> memory_map_ {};
    std::thread thread_;
    std::mutex mutex_;
};

}

#endif // DEBUGGER_HPP
