#pragma once

#include <array>
#include <vector>
#include <iostream>

namespace gameboy
{

class Rom
{	
	public:
	constexpr static auto Bank_size = 0x4000;
	using Bank = std::array<uint8_t, Bank_size>; // 16 KB
	explicit Rom(std::istream &is);
	
	uint8_t read(uint8_t bank, uint16_t adr) const;
	void dump(std::ostream &os) const;
	
	private:
	std::vector<Bank> data_;
};
	
}