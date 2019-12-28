#pragma once

#include <array>
#include <vector>

#include "debug_types.hpp"

namespace gameboy
{

template <uint16_t bank_sz>
class Ram
{
    public:
	using Bank = std::array<uint8_t, bank_sz>; // 8 KB
    explicit Ram() = default;

	uint8_t read(uint8_t bank, uint16_t adr) const;
    void write(uint8_t b, uint8_t bank, uint16_t adr);
    std::vector<uint8_t> dump(uint8_t bank) const; // dump one bank
    std::vector<uint8_t> dump() const; // dump all banks
	void resize(uint8_t nbanks);
    void reset();
	
	private:
	std::vector<Bank> data_;
	
};

// explicit template instantiations
extern template class Ram<0x2000>;
extern template class Ram<0x1000>;

using Work_ram = Ram<0x1000>;
using Video_ram = Ram<0x2000>;
using External_ram = Ram<0x2000>;

	
}
