#ifndef DISASSEMBLER_HPP
#define DISASSEMBLER_HPP

#include <vector>
#include <string>

namespace gameboy
{
	
class Disassembler
{
	public:
	explicit Disassembler();
	
	std::string disassemble(const std::vector<uint8_t> &ops) const;
};
	
}

#endif