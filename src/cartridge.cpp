#include "cartridge.hpp"

#include <stdexcept>
#include <tuple>
#include <cstdint>
#include <sys/stat.h> // stat()
#include <fstream>

#include "rom.hpp"
#include "ram.hpp"

namespace gameboy
{	

Cartridge::Cartridge(std::istream &is)
	: rom_ {is}
{
	init_mbc();
    init_info();
    init_ram();
}

void Cartridge::init_mbc()
{
	switch (rom_.read(0, 0x147))
	{
        case 0x09:
            has_battery_ = true;
        case 0x08:
		case 0x00: 
			break; // no MBC
        case 0x03:
            has_battery_ = true;
		case 0x01:
        case 0x02:
            mbc_ = std::make_unique<Mbc1>(&rom_, &ram_);
            break;
        case 0x06:
            has_battery_ = true;
        case 0x05:
            mbc_ = std::make_unique<Mbc2>(&rom_);
            break;
		case 0x0f:
		case 0x10:
        case 0x13:
            has_battery_ = true;
		case 0x11:
        case 0x12:
            mbc_ = std::make_unique<Mbc3>(&rom_, &ram_);
            break;
        case 0x1b:
        case 0x1e:
            has_battery_ = true;
		case 0x19:
        case 0x1a:
		case 0x1c:
        case 0x1d:
            mbc_ = std::make_unique<Mbc5>(&rom_, &ram_);
            break;
		default:
			throw std::runtime_error {"Unimplemented memory bank controller\n"};
	}
}

void Cartridge::init_info()
{
	for (uint8_t i {0}; i < 11; ++i)
		title_ += rom_.read(0, 0x134+i);
}

void Cartridge::init_ram()
{
    switch (rom_.read(0, 0x149))
	{
		default:
        case 0x00:
			break; // no ram
		case 0x01:
		case 0x02:
            ram_ = External_ram(1); // ERAM with 1 bank
			break;
		case 0x03:
            ram_ = External_ram(4); // 4 banks
            break;
		case 0x04:
            ram_ = External_ram(16); // 16 banks
            break;
		case 0x05:
            ram_ = External_ram(8); // 8 banks
            break;
    }
}
	
uint8_t Cartridge::read(uint16_t adr) const
{
	uint8_t b;
    if (mbc_)
	{
        b = mbc_->read(adr);
	}
	else // no banking, 32 KB rom, 2 banks of 16KB
	{
		uint8_t bank_n = adr < Rom::Bank_size ? 0 : 1;
		uint16_t relative_adr = adr - (bank_n ? Rom::Bank_size : 0);
		b = rom_.read(bank_n, relative_adr);
	}
	return b;
}

void Cartridge::write(uint8_t b, uint16_t adr)
{
    if (mbc_)
        mbc_->write(b, adr);
}

bool Cartridge::load_save(const std::string &path)
{
    if (!has_battery_)
        return false;
    // check if a save file exists and load it into SRAM
    std::ifstream sav(path, std::ios::binary);
    if (sav)
    {
        sav.seekg(0, std::ios::end);
        size_t len = sav.tellg();
        sav.seekg(0, std::ios::beg);
        std::vector<uint8_t> sram(len);
        sav.read(reinterpret_cast<char *>(sram.data()), len);
        if (mbc_)
            mbc_->load_sram(sram);
        else
            ram_->load(sram);
        return true;
    }
    return false;
}

std::map<std::string, Memory_range> Cartridge::dump() const
{
    std::map<std::string, Memory_range> out {};
    // external ram on cartridge is optional
    if (ram_) // external ram exists
    {
        // if no mbc chip, default bank 0
        uint8_t ram_bank = mbc_ ? mbc_->ram_bank() : 0;
        out["RAMX"] = {"RAM" + std::to_string(ram_bank), ram_->dump(ram_bank)};
    }
    else if (!ram_) // ex ram doesn't exist, dump empty ram bank
    {
        out["RAMX"] = {"RAMX", std::vector<uint8_t>(sizeof(External_ram::Bank))};
    }
    out["ROM0"] = {"ROM0", rom_.dump(0)};
    uint8_t rom_bank = mbc_ ? mbc_->rom_bank() : 1;
    out["ROMX"] = {"ROM" + std::to_string(rom_bank), rom_.dump(rom_bank)};
    return out;
}

std::vector<uint8_t> Cartridge::dump_rom() const
{
    return rom_.dump();
}

std::vector<uint8_t> Cartridge::dump_ram() const
{
    // return empty if no battery RAM
    if (!has_battery_ )
        return {};
    if (mbc_)
        return mbc_->dump_ram();
    else
        return ram_->dump();
}
	
bool Cartridge::is_cgb() const
{
	return rom_.read(0, 0x143) & 0x80; // upper bit
}	

std::string Cartridge::title() const
{
    return title_;
}

}
