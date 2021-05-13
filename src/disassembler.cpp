#include "disassembler.hpp"
#include "instruction_info.hpp"
#include "exception.hpp"
#include "debug_types.hpp"

#include <sstream>
#include <string>
#include <iomanip>
#include <iostream>

namespace qtboy
{
	
Disassembler::Disassembler()
{}


std::string Disassembler::pretty_disassemble(const std::vector<uint8_t> &ops)
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

// for more convenient stream formatting
class Disassembler::Hex
{
    public:
    explicit Hex(size_t x, int w) : x_ {x}, width_ {w} {}
    explicit Hex(size_t x) : x_ {x}, width_ {0} {} // unspecified width
    friend std::ostream &operator<<(std::ostream &os, const Hex &h)
    {
        os << std::right << std::hex << std::setfill('0')
           << std::uppercase;
        if (h.width_ > 0)
             os << std::setw(h.width_) << h.x_;
        else
            os << h.x_;
        return os;
    }

    private:
    size_t x_;
    int width_;
};

std::string Disassembler::parse_operand(const std::array<uint8_t, 3> &ops,
                                const std::string &operand, size_t adr)
{
    std::ostringstream out {};
    if (operand == "a16" || operand == "d16") // 16-bit immediate
        out << '$' << Hex(ops[2], 2) << Hex(ops[1], 2);
    else if (operand == "r8") // 8-bit signed immediate (relative address)
    {
        size_t dest_adr = adr+ops[1]+2;
        // if the destination address is 0000-ffff, set width to 4
        // otherwise, let ostream figure out the width
        Hex h = (dest_adr < 0x10000) ? Hex(dest_adr, 4) : Hex(dest_adr);
        out << '$' << h;
    }
    else if (operand == "d8") // 8-bit unsigned immediate
        out << '$' << Hex(ops[1], 2);
    else if (operand == "(a8)") // 8-bit unsigned indexing
        out << "($ff" << Hex(ops[1], 2) << ')';
    else if (operand == "(a16)" || operand == "(d16)") // 16-bit direct
        out << "($" << Hex(ops[2], 2) << Hex(ops[1], 2) << ')';
    else
        out << operand;
    return out.str();
}

// ops: next 3 bytes, adr: address of the instruction (for displaying relative
// jumps)
Assembly Disassembler::disassemble_op(const std::array<uint8_t, 3> &ops,
                                      size_t adr)
{
    Instruction ins;
    // handle CB prefix instructions
    if (ops[0] == 0xcb)
        ins = cb_instructions[ops[1]];
    else
        ins = instructions[ops[0]];
    // disassembly of instruction (e.g.) LD A,E
    std::ostringstream code {};
    // get max of 2 operands if they exist
    const std::string operand1 {ins.operand1.empty() ? "" : ins.operand1};
    const std::string operand2 {ins.operand2.empty() ? "" : ins.operand2};
    // use max width of 4 (LD, ADD, SBC, BIT)
    code << std::setfill(' ') << std::left << std::setw(4) << ins.name;
    // add operands (A,E; A,$12)
    if (!operand1.empty())
    {
        code << ' ' << parse_operand(ops, operand1, adr);
        if (!operand2.empty())
        {
            code << ',' << parse_operand(ops, operand2, adr);
        }
    }
    // get the number of bytes used (3 bytes are always passed, but some
    // instructions are only 1-2 bytes long)
    std::vector<uint8_t> ops_used(ins.length);
    for (uint8_t i {0}; i < ins.length; ++i)
        ops_used[i] = ops[i];
    return Assembly {ops_used, ins, code.str()};
}

std::vector<Assembly> Disassembler::disassemble(const std::vector<uint8_t> &ops)
{
    // disassemble a vector of bytes using the helper disassemble_op()
    std::vector<Assembly> out {};
    for (size_t i {0}; i < ops.size(); i += instructions[ops[i]].length)
    {
        std::array<uint8_t, 3> next_bytes {ops[i], ops[i+1], ops[i+2]};
        out.push_back(disassemble_op(next_bytes, i));
    }
    return out;
}

}
