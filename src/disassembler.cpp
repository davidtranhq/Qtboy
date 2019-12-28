#include "disassembler.hpp"
#include "instruction_info.hpp"
#include "exception.hpp"
#include "debug_types.hpp"

#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>

namespace gameboy
{
	
Disassembler::Disassembler()
{}


std::string Disassembler::hex_dump(const std::vector<uint8_t> &ops) const
{
	uint8_t len {0};
	std::ostringstream out {};
	for (uint32_t pc {0}; pc < ops.size(); pc += len)
    {
        Instruction ins {instructions[ops[pc]]};
        len = ins.length;
        std::ostringstream bytes {};
		for (uint8_t i {0}; i < ins.length; ++i) // opcodes
            bytes << std::setfill('0') << ' ' << std::hex
                  << std::setw(2) << static_cast<int>(ops[pc+i]);
        const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
        const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
        out << std::setfill('0') << std::setw(4) << std::hex << std::right << pc; // address
        out << std::setfill(' ') << std::setw(10) << std::left << bytes.str() // opcodes
            << ' ' << std::setw(4) << ins.name; // instruction
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

class Disassembler::Hex
{
    public:
    explicit Hex(int x) : x_ {x} {}
    friend std::ostream &operator<<(std::ostream &os, const Hex &h)
    {
        os << std::hex << std::setfill('0') << std::setw(2) << h.x_;
        return os;
    }

    private:
    int x_;
};

void Disassembler::parse_operand(std::ostream &out, const std::array<uint8_t, 3> &ops,
                                const std::string &operand) const
{
    if (operand == "a16" || operand == "d16") // 16-bit immediate
        out << Hex(ops[2]) << Hex(ops[1]);
    else if (operand == "r8") // 8-bit signed immediate
        out << Hex(static_cast<int8_t>(ops[1]));
    else if (operand == "d8") // 8-bit unsigned immediate
        out << Hex(ops[1]);
    else if (operand == "(a8)") // 8-bit unsigned indexing
        out << '(' << Hex(ops[1]) << ')';
    else if (operand == "(a16)" || operand == "(d16)") // 16-bit direct
        out << '(' << Hex(ops[2]) << Hex(ops[1]) << ')';
    else
        out << operand;
}

Assembly Disassembler::disassemble_op(const std::array<uint8_t, 3> &ops) const
{
    Instruction ins;
    if (ops[0] == 0xcb)
        ins = cb_instructions[ops[1]];
    else
        ins = instructions[ops[0]];
    std::ostringstream code {};
    const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
    const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
    code << ins.name; // instruction
    if (!operand1.empty())
    {
        code << ' ';
        parse_operand(code, ops, operand1);
        if (!operand2.empty())
        {
            code << ',';
            parse_operand(code, ops, operand2);
        }
    }
    std::vector<uint8_t> ops_used(ins.length);
    for (uint8_t i {0}; i < ops.size(); ++i)
        ops_used[i] = ops[i];
    return Assembly {ops_used, ins, code.str()};
}

std::vector<Assembly> Disassembler::disassemble(const std::vector<uint8_t> &ops) const
{
    std::vector<Assembly> out {};
    for (size_t i {0}; i < ops.size(); i += instructions[ops[i]].length)
    {
        std::array<uint8_t, 3> next_bytes {ops[i], ops[i+1], ops[i+2]};
        out.push_back(disassemble_op(next_bytes));
    }
    return out;
}

}
