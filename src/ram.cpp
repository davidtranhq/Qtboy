#include "ram.hpp"

#include <istream>
#include <stdexcept>
#include <vector>
#include <algorithm>

namespace qtboy
{

template <uint16_t bank_sz>
Ram<bank_sz>::Ram(uint8_t banks, const std::vector<uint8_t> &sram)
    : data_(banks)
{
    if (!sram.empty())
        load(sram);
}

template <uint16_t bank_sz>
uint8_t Ram<bank_sz>::read(uint8_t bank, uint16_t adr) const
{
    return data_[bank][adr];
}

template <uint16_t bank_sz>
void Ram<bank_sz>::write(uint8_t b, uint8_t bank, uint16_t adr)
{
    data_[bank][adr] = b;
}

template<uint16_t bank_sz>
void Ram<bank_sz>::load(const std::vector<uint8_t> &sram)
{
    size_t i {0};
    for (Bank &b : data_)
    {
        std::copy(sram.begin() + i, sram.begin() + i + bank_sz, b.data());
        i += bank_sz;
    }
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
size_t Ram<bank_sz>::size() const
{
    return bank_sz*data_.size();
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
