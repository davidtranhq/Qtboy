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
    virtual uint8_t rom_bank() const { return 0; }
    virtual uint8_t ram_bank() const { return 0; }
    // for MBCs with built-in RAM (MBC2)
    virtual std::vector<uint8_t> dump_ram() const { return {}; }
    virtual void load_sram(const std::vector<uint8_t> &sram) { return; }
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
    void load_sram(const std::vector<uint8_t> &sram) override;
    std::vector<uint8_t> dump_ram() const override;
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



class Mbc2 : public Memory_bank_controller
{
    public:
    explicit Mbc2(Rom *rom);
    Mbc2(const Mbc2 &) = delete;
    Mbc2 &operator=(const Mbc2 &) = delete;
    ~Mbc2() override = default;

    uint8_t read(uint16_t adr) const override;
    void write(uint8_t b, uint16_t adr) override;
    void load_sram(const std::vector<uint8_t> &sram) override;
    std::vector<uint8_t> dump_ram() const override;
    const char *type() const override { return "MBC2"; }
    uint8_t rom_bank() const override { return rom_bank_; }

    private:
    Rom *rom_;
    uint8_t rom_bank_ {1};
    std::vector<uint8_t> ram_;
    bool ram_enable_ {false};
};


class Mbc3 : public Memory_bank_controller
{
    public:
    explicit Mbc3(Rom *rom, std::optional<External_ram> *ram);
    Mbc3(const Mbc3 &) = delete;
    Mbc3 &operator=(const Mbc3 &) = delete;
    ~Mbc3() override = default;

    uint8_t read(uint16_t adr) const override final;
    void write(uint8_t b, uint16_t adr) override final;
    void load_sram(const std::vector<uint8_t> &sram) override;
    std::vector<uint8_t> dump_ram() const override;
    const char *type() const override { return "MBC3"; }
    uint8_t rom_bank() const override { return rom_bank_; }
    uint8_t ram_bank() const override { return ram_bank_; }

    void load_rtc(const std::array<uint8_t, 5> &base);

    private:
    void update_rtc() const;

	private:
    Rom *rom_;
    std::optional<External_ram> *ram_;
    bool ram_rtc_enable_ {false};
	bool ram_timer_enable_ {false};
    uint8_t rom_bank_ {1};
	uint8_t ram_bank_ {0};
    bool is_rtc_ {false}; // specifies whether RTC or RAM bank is mapped
    uint8_t mapped_rtc_ {0}; // specifies the RTC register mapped at a000-bfff
    mutable struct // mutable so read() can update RTC
    {
        time_t base_time;
        std::array<uint8_t, 5> base_regs, latched_regs;
        bool latched;
    } rtc_;
};

class Mbc5 : public Memory_bank_controller
{
	public:
    explicit Mbc5(Rom *rom, std::optional<External_ram> *ram);
    Mbc5(const Mbc5 &) = delete;
    Mbc5 &operator=(const Mbc5 &) = delete;
    ~Mbc5() override = default;

    uint8_t read(uint16_t adr) const override final;
    void write(uint8_t b, uint16_t adr) override final;
    void load_sram(const std::vector<uint8_t> &sram) override;
    std::vector<uint8_t> dump_ram() const override;
    const char *type() const override { return "MBC5"; }
    uint8_t rom_bank() const override { return rom_bank_; }
    uint8_t ram_bank() const override { return ram_bank_; }
	
	private:
    Rom *rom_;
    std::optional<External_ram> *ram_;
	bool ram_enable_ {false};
    uint8_t ram_bank_ {0};
    uint16_t rom_bank_ {0};
};


	
}
