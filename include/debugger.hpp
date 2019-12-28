#ifndef DEBUGGER_HPP
#define DEBUGGER_HPP

#include <cstdint>
#include <map>
#include <vector>

#include "debug_types.hpp"

namespace gameboy
{

class System;

class Debugger
{
    public:
    explicit Debugger(System *);

    void run();
    void step(size_t);
    void add_breakpoint(uint16_t);
    void delete_breakpoint(uint16_t);

    std::vector<uint8_t> dump_memory() const;
    std::map<std::string, Memory_range> dump_mapped_memory() const;
    bool memory_changed() const;
    std::vector<uint8_t> dump_rom() const noexcept;
    Cpu_values dump_cpu() const noexcept;
    void draw_framebuffer();

    private:
    bool at_breakpoint();

    System *system_;
    std::map<uint16_t, bool> breaks_;
    size_t steps_;
    std::map<std::string, Memory_range> memory_map_;
};

}

#endif // DEBUGGER_HPP
