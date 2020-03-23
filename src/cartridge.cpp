#include "cartridge.hpp"

#include <stdexcept>
#include <tuple>
#include <cstdint>

#include "rom.hpp"
#include "ram.hpp"

namespace gameboy
{	

Cartridge::Cartridge(std::istream &is)
	: rom_ {is}
{
	init_mbc();
	init_ram();
	init_info();
}

void Cartridge::init_mbc()
{
	switch (rom_.read(0, 0x147))
	{
		case 0x00: 
			break; // no MBC
		case 0x01:
		case 0x02:
        case 0x03:
            mbc_ = std::make_unique<Mbc1>(&rom_, &ram_);
            break;
		/*
		case 0x05:
		case 0x06:
			mbc_ = std::make_unique<Mbc2>(rom_, ram_);
			break;
		case 0x0f:
		case 0x10:
		case 0x11:
		case 0x12:
		case 0x13:
			mbc_ = std::make_unique<Mbc3>(rom_, ram_);
			break;
		case 0x19:
		case 0x1a:
		case 0x1b:
		case 0x1c:
		case 0x1d:
		case 0x1e:
			mbc_ = std::make_unique<Mbc5>(rom_, ram_);
			break;
		*/
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
            ram_ = {};
            ram_->resize(1); // 1 bank of 8KB RAM
			break;
		case 0x03:
            ram_ = {};
            ram_->resize(4);
			break;
		case 0x04:
            ram_ = {};
            ram_->resize(16);
			break;
		case 0x05:
            ram_ = {};
            ram_->resize(8);
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
    if (ram_)
        return ram_->dump();
    return {};
}
	
bool Cartridge::is_cgb() const
{
	return rom_.read(0, 0x143) & 0x80; // upper bit
}	

}
