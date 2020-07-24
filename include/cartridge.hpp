#pragma once

#include <array>
#include <vector>
#include <cstdint>
#include <memory>
#include <optional>
#include <map>

#include "rom.hpp"
#include "ram.hpp"
#include "memory_bank_controller.hpp"
#include "debug_types.hpp"

namespace gameboy
{

class Cartridge
{
	public:
    explicit Cartridge(std::istream &is);
    Cartridge(const Cartridge &) = delete;
    Cartridge(Cartridge &&) = default;
    Cartridge &operator=(const Cartridge &) = delete;
    Cartridge &operator=(Cartridge &&) = default;
    ~Cartridge() = default;

	uint8_t read(uint16_t adr) const;
    void write(uint8_t b, uint16_t adr);
    bool load_save(const std::string &path);
    std::unordered_map<std::string, Memory_range> dump() const;
    std::vector<uint8_t> dump_rom() const;
    std::vector<uint8_t> dump_ram() const;
    bool is_cgb() const;
    std::string title() const;
	
	private:
    Rom rom_;
    std::optional<External_ram> ram_ {std::nullopt};

    std::unique_ptr<Memory_bank_controller> mbc_ {nullptr};
    std::string title_ {};
    bool has_battery_ {false};

	void init_mbc();
	void init_info();
	void init_ram();
};
	
}
