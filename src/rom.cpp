#include "rom.hpp"

#include <istream>
#include <stdexcept>
#include <iomanip>

namespace gameboy
{
	
Rom::Rom(std::istream &is)
{
	if (!is.good())
        throw std::runtime_error {"ROM: Invalid input stream!\n"};
    Bank buf {};
	while (is.read(reinterpret_cast<char *>(buf.data()), buf.size()))
        data_.push_back(buf);
}

uint8_t Rom::read(uint8_t bank, uint16_t adr) const
{
	if (bank > data_.size() || adr > data_[0].size())
		throw std::out_of_range {"Invalid ROM address"};
	return data_[bank][adr];
}
	
std::vector<uint8_t> Rom::dump(uint8_t bank) const
{
    if (bank > data_.size())
        throw std::out_of_range {"Invalid bank selection"};
    return std::vector<uint8_t>(data_[bank].begin(), data_[bank].end());
}

std::vector<uint8_t> Rom::dump() const
{
    std::vector<uint8_t> out {};
    out.reserve(data_.size() * data_[0].size());
    for (size_t bank {0}; bank < data_.size(); ++bank)
        out.insert(out.end(), data_[bank].begin(), data_[bank].end());
    return out;
}
	
}
