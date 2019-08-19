#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>

#include "rom.hpp"
#include "ram.hpp"
#include "memory_bank_controller.hpp"

namespace gameboy
{

class Cartridge
{
	public:
	explicit Cartridge(std::istream &is);
	
	uint8_t read(uint16_t adr) const;
	void write(uint8_t b, uint16_t adr);
	void dump(std::ostream &os) const;
	bool is_cgb() const;
	
	private:
	Rom rom_;
	Ram ram_;
	std::optional<std::unique_ptr<Memory_bank_controller>> mbc_;
	std::string title_;

	void init_mbc();
	void init_info();
	void init_ram();
};
	
}