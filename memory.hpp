#pragma once

#include <array>
#include <optional>
#include <cstdint>
#include <stdexcept>

#include "cartridge.hpp"

namespace gameboy
{

class Memory
{
	static constexpr auto RAM_BANK_SIZE = 0x2000; // 8 KB
	public:
	uint8_t read(uint16_t adr) const;
	void write(uint8_t b, uint16_t adr);
	void load_cartridge(std::istream &is);
	
	private:
	std::optional<Cartridge> cart_;
	Ram vram_; // 8KB x 2
	Ram wram_;
	std::array<uint8_t, 0xa0> oam_;
	std::array<uint8_t, 0x80> io_;
	std::array<uint8_t, 0x7f> hram_;
	bool ram_enable_;
	uint8_t &vbk_ {io_[0x4f]}; // CGB only, specifies vram bank
	uint8_t &svbk_ {io_[0x70]}; // CGB only, specifies wram bank
	uint8_t ie_;
	
	void set_ram_size();
	
};

}