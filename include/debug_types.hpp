#ifndef DEBUG_TYPES_HPP
#define DEBUG_TYPES_HPP

#include "instruction_info.hpp"

#include <string>
#include <cstdint>
#include <vector>

namespace gameboy
{

// for dumping memory
struct Memory_range
{
    std::string name;
    std::vector<uint8_t> data;
};

enum class Dump_format
{
    Hex, Stack
};

const std::array<std::string, 11> Memory_range_names
{
    "ROM0",
    "ROMX",
    "VRMX",
    "RAMX",
    "WRM0",
    "WRMX",
    "ECHO",
    "OAM",
    "XXXX",
    "IO",
    "HRAM"
};

// for dumping CPU values
struct Cpu_values
{
    uint16_t af, bc, de, hl, sp, pc;
    uint32_t cycles;
    bool ime;
    std::array<uint8_t, 3> next_ops;
};

struct Assembly
{
    std::vector<uint8_t> ops;
    Instruction ins;
    std::string code;
};


}

#endif // DEBUG_TYPES_HPP
