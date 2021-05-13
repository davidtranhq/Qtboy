#pragma once

#include <array>
#include <vector>
#include <iostream>

#include "debug_types.hpp"

namespace qtboy
{

class Rom
{	
	public:
	constexpr static auto Bank_size = 0x4000;
	using Bank = std::array<uint8_t, Bank_size>; // 16 KB
    explicit Rom() = default;
	explicit Rom(std::istream &is);
	
	uint8_t read(uint8_t bank, uint16_t adr) const;
    std::vector<uint8_t> dump(uint8_t bank) const;
    std::vector<uint8_t> dump() const;
	
	private:
	std::vector<Bank> data_;
};
	
}
