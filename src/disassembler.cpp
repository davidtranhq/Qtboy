#include "disassembler.hpp"
#include "instruction_info.hpp"
#include "exception.hpp"

#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>

namespace gameboy
{
	
Disassembler::Disassembler()
{}


std::string Disassembler::disassemble(const std::vector<uint8_t> &ops) const
{
	uint8_t len {0};
	std::ostringstream out {};
	for (uint32_t pc {0}; pc < ops.size(); pc += len)
    {
        Instruction ins {instructions[ops[pc]]};
		len = ins.length;
		out << std::setfill('0') << std::setw(4) << std::hex << pc; // address
		for (uint8_t i {0}; i < ins.length; ++i) // opcodes
			out << std::setfill(' ') << ' ' << static_cast<int>(ops[pc+i]);
        const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
        const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
        out << std::setw(10) << ins.name;
        if (!operand1.empty())
        {
            out << ' ' << operand1;
            if (!operand2.empty())
                out << ',' << operand2;
        }
        out << '\n';
	}
	return out.str();
}
	
}
