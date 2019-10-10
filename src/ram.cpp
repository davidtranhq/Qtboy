#include "ram.hpp"

#include <istream>
#include <stdexcept>
#include <iomanip>

namespace gameboy
{

uint8_t Ram::read(uint8_t bank, uint16_t adr) const
{
	if (bank > data_.size() || adr > bank_sz)
		throw std::out_of_range {"Invalid RAM address"};
	return data_[bank][adr];
}

void Ram::write(uint8_t b, uint8_t bank, uint16_t adr)
{
	if (bank > data_.size() || adr > bank_sz)
		throw std::out_of_range {"Invalid RAM address"};
	data_[bank][adr] = b;
}

void Ram::dump(std::ostream &os) const
{
	for (size_t i {0}; i < data_.size(); ++i)
	{
		os << "Bank " << i << ":\n";
		os << std::setw(11) << ' ';
		// address columns
		for (uint8_t col {0x00}; col <= 0x0F; ++col)
		{
			os  << std::setfill('0') << std::setw(2) << std::hex
				<< std::uppercase << static_cast<int>(col) << ' ';
		}
		os << '\n';
		
		std::vector<uint8_t> buf;
		for (std::size_t j {0}; j < data_[i].size(); j += 16)
		{
			buf = {data_[i].begin() + j, data_[i].begin() + j + 16};
			os << std::setw(8) << std::setfill('0') << std::hex << i; // address rows
			os << std::setw(3) << std::setfill(' ') << ' ';
			for (uint8_t h : buf)
			{
				os << std::setw(2) << std::setfill('0')
					<< std::hex << static_cast<int>(h) << ' ';
			}
			for (uint8_t h : buf)
			{
				if (h > 32 && h < 122)
					os << h;
				else
					os << '.';
			}
			os << '\n';
		}
		
	}
}

std::vector<uint8_t> Ram::dump() const
{
    std::vector<uint8_t> out {};
    out.reserve(data_.size() * data_[0].size());
    for (size_t bank {0}; bank < data_.size(); ++bank)
        out.insert(out.end(), data_[bank].begin(), data_[bank].end());
    return out;
}

void Ram::resize(uint8_t nbanks)
{
	data_.resize(nbanks);
}
	
}
