// g++ -o create_opcodes.exe opcodes.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

#include "json.hpp"

using nlohmann::json;

bool exists(const json &j, const char *key)
{
	if (j.find(key) == j.end())
		return false;
	return true;
}

std::string to_hex_string(uint8_t op)
{
	std::ostringstream oss;
	oss << "0x" 
		<< std::setfill('0')
		<< std::hex << static_cast<int>(op);
	return oss.str();
}

int main()
{
	std::ifstream in {"json_opcodes.hpp"};
	json j;
	in >> j;
	
	std::ostringstream out {};
	out << R"""(#ifndef INSTRUCTION_INFO_HPP
	#define INSTRUCTION_INFO_HPP
	
	const std::array<Instruction, 501> instructions
	{
	)""";
	const char *prefixed {"false"};
	std::array<const char *, 2> key {"unprefixed", "cbprefixed"};
	for (int i {0}; i < 2; ++i)
	{
		for (int k {0x00}; k < 0x100; ++k)
		{
			const json &op = j[key[i]][to_hex_string(k)];
			if (op.is_null())
			{
				out << "\t{\"Non-existant OP\", 0, 0, 0, nullptr, nullptr, false}\n";
				continue;
			}
			out << "\t{" << op["mnemonic"] << ", " << op["length"] << ", "
				<< op["cycles"][0] << ", " 
				<< ((op["cycles"].size() > 1) ? op["cycles"][1] : op["cycles"][0]) << ", "
				<< (exists(op, "operand1") ? to_string(op["operand1"]) : "nullptr") << ", "
				<< (exists(op, "operand2") ? to_string(op["operand2"]) : "nullptr")
				<< "},\n";
		}
		out << "\n};\n\nconst std::array<Instruction, 256> cb_instructions\n{\n";
	}
	out << "#endif";
	std::ofstream output {"opcodes.hpp"};
	output << out.str();
	return 0;
}

struct Instruction
{
	const char *const name;
	uint8_t length;
	uint8_t cycles;
	uint8_t alt_cycles;
	const char *operand1;
	const char *operand2;
	bool cb_prefixed;
};
