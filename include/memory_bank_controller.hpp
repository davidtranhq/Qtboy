#pragma once

#include <cstdint>
#include <vector>
#include <functional>

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
    virtual uint8_t rom_bank() const = 0;
    virtual uint8_t ram_bank() const = 0;
    virtual ~Memory_bank_controller() = default;
};

class Mbc1 : public Memory_bank_controller
{
	public:
    explicit Mbc1(Rom *rom, std::optional<External_ram> *ram);
    Mbc1(const Mbc1 &) = delete;
    Mbc1 &operator=(const Mbc1 &) = delete;
    ~Mbc1() override = default;

	uint8_t read(uint16_t adr) const override;
	void write(uint8_t b, uint16_t adr) override;
	const char *type() const override { return "MBC1"; }
    uint8_t rom_bank() const override { return rom_bank_; }
    uint8_t ram_bank() const override { return ram_bank_; }

    private:
    void adjust_rom_bank();

	private:
    Rom *rom_;
    std::optional<External_ram> *ram_;
	uint8_t ram_bank_ {0};
    uint8_t rom_bank_ {1};
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

*/

class Mbc3 : public Memory_bank_controller
{
    public:
    explicit Mbc3(Rom *rom, External_ram *ram);
    Mbc3(const Mbc3 &) = delete;
    Mbc3 &operator=(const Mbc3 &) = delete;
    ~Mbc3() override = default;

    uint8_t read(uint16_t adr) const override final;
    void write(uint8_t b, uint16_t adr) override final;
    const char *type() const override { return "MBC3"; }
    uint8_t rom_bank() const override { return rom_bank_; }
    uint8_t ram_bank() const override { return ram_bank_; }
	
	private:
    Rom *rom_;
    External_ram *ram_;
    bool ram_rtc_enable_ {false};
	bool ram_timer_enable_ {false};
	uint8_t rom_bank_ {0};
	uint8_t ram_bank_ {0};
    bool is_rtc_ {false}; // specifies whether RTC or RAM bank is mapped
    uint8_t mapped_rtc_ {0}; // specifies the RTC register mapped at a000-bfff
    std::array<uint8_t, 5> rtc_registers_ {};
};

/*
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
