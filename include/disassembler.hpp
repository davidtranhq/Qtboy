#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include "debug_types.hpp"

#include <vector>
#include <string>

namespace gameboy
{
	
class Disassembler
{
	public:
	explicit Disassembler();
	
    std::string hex_dump(const std::vector<uint8_t> &ops) const;
    std::vector<Assembly> disassemble(const std::vector<uint8_t> &ops) const;


    private:
    class Hex;
    Assembly disassemble_op(const std::array<uint8_t, 3> &ops) const;
    void parse_operand(std::ostream &, const std::array<uint8_t, 3> &,
                       const std::string &) const;
};
	
}

#endif
