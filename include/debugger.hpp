#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <vector>
#include <fstream>

#include "debug_types.hpp"
#include "instruction_info.hpp"
#include "ppu.hpp"

namespace gameboy
{

class System;

class Debugger
{
    public:
    explicit Debugger(System *);

    void run();
    void pause();
    void step(size_t n = 1);
    void reset();
    void add_breakpoint(uint16_t);
    void delete_breakpoint(uint16_t);
    void enable_logging(bool b = true);
    void write_log(); // write log to file
    std::string log(); // return log as string
    bool set_log_file(const std::string &);


    size_t steps() const;
    bool paused() const;
    std::vector<uint8_t> dump_memory() const;
    std::map<std::string, Memory_range> dump_mapped_memory() const;
    std::string dump_formatted_memory(Dump_format d = Dump_format::Hex) const;
    bool memory_changed() const;
    std::vector<uint8_t> dump_rom() const noexcept;
    Cpu_values dump_cpu() const noexcept;
    std::array<Tile_data, 384> dump_tileset();
    std::array<std::array<uint8_t, 16>, 384> dump_raw_tileset();
    Frame_data dump_background();
    Frame_data dump_window();
    std::array<uint8_t, 32*32> dump_raw_background();
    std::array<Sprite, 40> dump_sprites();

    std::string hex_dump() const;
    Assembly disassemble_op() const; // disassemble next instruction
    Assembly disassemble_op(uint16_t adr) const; // disassemble instruction at
    std::vector<Assembly> disassemble() const; // disassemble all

    private:
    bool valid_log_file(const std::string &s) const;
    bool at_breakpoint();
    std::string parse_operand(const std::array<uint8_t, 3> &ops,
                              const std::string &operand,
                              uint16_t adr) const;

    System *system_;
    std::map<uint16_t, bool> breaks_ {};
    size_t steps_ {0};
    bool memory_changed_ {true};
    std::map<std::string, Memory_range> memory_map_ {};
    bool paused_ {false};
    bool log_ {true};
    std::ofstream log_file_;
};

}

#endif // DEBUGGER_HPP
