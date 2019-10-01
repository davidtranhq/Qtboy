#include <fstream>
#include <string>
#include <vector>
#include "json.hpp"


using json = nlohmann::json;

namespace gameboy
{
	
class Disassembler
{
	public:
	explicit Disassembler();
	
	std::string disassemble(const std::vector<uint8_t> &op) const;
	std::string to_hex_string(uint8_t op) const;
	
	private:
	json opcodes_;
	
};
	
}