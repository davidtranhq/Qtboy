#include "ram.hpp"

#include <istream>
#include <stdexcept>
#include <iomanip>

namespace gameboy
{

template <uint16_t bank_sz>
uint8_t Ram<bank_sz>::read(uint8_t bank, uint16_t adr) const
{
	if (bank > data_.size() || adr > bank_sz)
		throw std::out_of_range {"Invalid RAM address"};
	return data_[bank][adr];
}

template <uint16_t bank_sz>
void Ram<bank_sz>::write(uint8_t b, uint8_t bank, uint16_t adr)
{
	if (bank > data_.size() || adr > bank_sz)
		throw std::out_of_range {"Invalid RAM address"};
	data_[bank][adr] = b;
}

template <uint16_t bank_sz>
std::vector<uint8_t> Ram<bank_sz>::dump(uint8_t bank) const
{
    if (bank > data_.size())
        throw std::out_of_range {"Invalid bank selection"};
    return std::vector<uint8_t>(data_[bank].begin(), data_[bank].end());
}

template <uint16_t bank_sz>
std::vector<uint8_t> Ram<bank_sz>::dump() const
{
    std::vector<uint8_t> out {};
    out.reserve(data_.size() * data_[0].size());
    for (size_t bank {0}; bank < data_.size(); ++bank)
        out.insert(out.end(), data_[bank].begin(), data_[bank].end());
    return out;
}

template <uint16_t bank_sz>
void Ram<bank_sz>::resize(uint8_t nbanks)
{
	data_.resize(nbanks);
}

template <uint16_t bank_sz>
void Ram<bank_sz>::reset()
{
    for (uint8_t i {0}; i < data_.size(); ++i)
        data_[i] = {};
}

template class Ram<0x2000>;
template class Ram<0x1000>;


}
