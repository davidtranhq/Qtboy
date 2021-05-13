#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include "debug_types.hpp"

#include <vector>
#include <string>

namespace qtboy
{
	
class Disassembler
{
	public:
	explicit Disassembler();

    static std::string pretty_disassemble(const std::vector<uint8_t> &ops);
    static std::vector<Assembly> disassemble(const std::vector<uint8_t> &ops);
    static Assembly disassemble_op(const std::array<uint8_t, 3> &ops, size_t adr);

    private:
    class Hex;
    static std::string parse_operand(const std::array<uint8_t, 3> &next_ops,
                       const std::string &operand, size_t adr);
};
	
}

#endif
