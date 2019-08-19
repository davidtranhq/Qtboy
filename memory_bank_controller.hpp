#pragma once

#include <cstdint>
#include <vector>

#include "rom.hpp"
#include "ram.hpp"

namespace gameboy
{
	
class Memory_bank_controller
{
	public:
	virtual uint8_t read(uint16_t adr) const = 0;
	virtual void write(uint8_t b, uint16_t adr) = 0; 
	virtual const char *type() const = 0;
};

class Mbc1 : public Memory_bank_controller
{
	public:
	explicit Mbc1(Rom &rom, Ram &ram);
	
	uint8_t read(uint16_t adr) const override;
	void write(uint8_t b, uint16_t adr) override;
	const char *type() const override { return "MBC1"; }
	
	private:
	Rom &rom_;
	Ram &ram_;
	uint8_t ram_bank_ {0};
	uint8_t rom_bank_ {0};
	bool ram_enable_ {false};
	bool ram_mode_select_ {false};
};

/*

class Mbc2 : public Memory_bank_controller
{
	using Cartridge::Rom_bank;
	using Cartridge::Ram_bank;
	
	public:
	virtual uint8_t read(uint16_t adr) const override final;
	virtual void write(uint8_t b, uint16_t adr) override final;
	
	private:
	std::vector<Rom_bank> &rom_;
	std::array<uint8_t, 0x800> ram_ {0}; // 512x4 bits RAM	
	uint8_t ram_bank_ {0};
	uint8_t rom_bank_ {0};
	bool ram_enable_ {false};
};

class Mbc3 : public Memory_bank_controller
{
	using Cartridge::Rom_bank;
	using Cartridge::Ram_bank;
	
	public:
	virtual uint8_t read(uint16_t adr) const override final;
	virtual void write(uint8_t b, uint16_t adr) override final;
	
	private:
	std::vector<Rom_bank> &rom_;
	bool ram_timer_enable_ {false};
	uint8_t rom_bank_ {0};
	uint8_t ram_bank_ {0};
	void latch_rtc();	
};

class Mbc5 : public Memory_bank_controller
{
	public:
	virtual uint8_t read(uint16_t adr) const override final;
	virtual void write(uint8_t b, uint16_t adr) override final;
	
	private:
	std::vector<Rom_bank> &rom_;
	std::vector<Ram_bank> &ram_;
	bool ram_enable_ {false};
	uint8_t ram_bank_ {0};
	uint8_t rom_bank_ {0};
};

*/

	
}