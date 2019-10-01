#include "disassembler.hpp"
#include "exception.hpp"
#include "json_opcodes.hpp"

#include <sstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <deque>

namespace gameboy
{
	
Disassembler::Disassembler()
{
	opcodes_ = json::parse(json_opcodes);
}

std::string Disassembler::to_hex_string(uint8_t op) const
{
	std::ostringstream oss;
	oss << "0x" 
		<< std::setfill('0')
		<< std::hex << static_cast<int>(op);
	return oss.str();
}

std::string Disassembler::disassemble(const std::vector<uint8_t> &ops) const
{
	if (ops.empty())
		throw Bad_disassembly {"No opcodes to disassemble", __FILE__, __LINE__};
	// convert vector of bytes to a hex string (the instructions' keys in the
	// json are strings)
	std::deque<std::string> codes;
	for (uint8_t o : ops)
		codes.push_back(to_hex_string(o));
	// 0xcb instructions indicate a prefix, indexed as "cbprefixed" otherwise
	// "unprefixed"
	bool prefixed = ops[0] == 0xcb;
	std::string prefix {prefixed ? "cbprefixed" : "unprefixed"};
	// removing 0xcb allows the first element to always be the real 
	// instruction
	if (prefixed)
		codes.pop_front();
	// json op {opcodes_[prefix][codes[0]]};
	json op = opcodes_.at("unprefixed").at(codes[0]);
	// may as well throw an error if the incorrect number of ops were passed
	uint8_t len {op["length"]};
	if (len != codes.size())
		throw Bad_disassembly {"Incorrect number of instructions passed", __FILE__, __LINE__};
	std::string mnemonic {op["mnemonic"]};
	// if operands exist they are assigned, otherwise they are empty strings
	std::string operand1 {op.find("operand1") == op.end() ? "" : op["operand1"]};
	std::string operand2 {op.find("operand2") == op.end() ? "" : op["operand2"]};
	// if operands are not registers, show their actual value
	
	std::ostringstream out;
	out << mnemonic << ' ';
	if (operand1 == "a16")
	{
		out << '$' << std::hex << std::setfill('0')
		<< std::setw(2) << static_cast<int>(ops[2]) 
		<< std::setw(2) << static_cast<int>(ops[1]);
	}
	else
	{
		out << operand1 << ',' << operand2;
	}
	return out.str();
}
	
}