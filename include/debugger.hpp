#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <atomic>

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
    ~Debugger();

    void enable_debug(bool b);
    void update();
    bool was_updated();
    void run_until_break();
    void run_no_break();
    bool running();
    void pause();
    void step(size_t n = 1);
    void reset();
    void add_breakpoint(uint16_t);
    void delete_breakpoint(uint16_t);
    const std::unordered_map<uint16_t, bool> &breakpoints() const;
    void enable_logging(bool b = true);
    bool set_log_file(const std::string &);
    void write_log(); // write log to file
    size_t steps() const;

    void update_memory_cache() const;
    std::vector<uint8_t> dump_memory() const;
    std::string dump_formatted_memory(Dump_format d = Dump_format::Hex) const;
    std::vector<uint8_t> dump_rom() const noexcept;
    Cpu_dump dump_cpu() const noexcept;
    Ppu::Dump dump_ppu() const noexcept;
    std::array<Palette, 8> dump_bg_palettes() const;
    std::array<Palette, 8> dump_sprite_palettes() const;
    std::array<Texture, 384> dump_tileset(uint8_t bank);
    Texture dump_framebuffer(bool with_bg = true, bool with_win = true,
                             bool with_sprites = true) const;
    Texture dump_background() const;
    Texture dump_window() const;
    std::array<Texture, 40> dump_sprites();

    std::vector<Assembly> disassemble() const; // disassemble all

    private:
    std::string log(); // return log as string
    bool at_breakpoint();

    private:
    System *system_ {nullptr};
    Disassembler disassembler_ {};
    std::unordered_map<uint16_t, bool> breaks_ {false};
    size_t steps_ {0};
    bool paused_ {false};
    bool logging_ {false};
    bool breaking_ {true};
    bool updated_ {true}; // for a debugger viewer to know when to update
    std::mutex update_mutex_;
    std::ofstream log_file_;
    // memory map cache
    mutable std::unordered_map<std::string, Memory_range> memory_map_ {};
};

}

#endif // DEBUGGER_HPP
