#pragma once

#include <array>
#include <vector>

namespace gameboy
{

class Ram
{
	constexpr static auto bank_sz = 0x2000;
	using Bank = std::array<uint8_t, bank_sz>; // 8 KB
	
	public:
	uint8_t read(uint8_t bank, uint16_t adr) const;
	void write(uint8_t b, uint8_t bank, uint16_t adr);
	void dump(std::ostream &os) const;
	void resize(uint8_t nbanks);
	
	private:
	std::vector<Bank> data_;
	
};
	
}