#pragma once

#include <cstdint>

namespace gameboy
{
	
struct Register_pair
{
	public:
	explicit Register_pair(uint16_t rp = 0);
	
	Register_pair &operator+=(const int16_t);
	Register_pair &operator-=(const int16_t);
	Register_pair &operator++();
	Register_pair operator++(int);
	Register_pair &operator--();
	Register_pair operator--(int);
	Register_pair &operator=(const uint16_t);
	operator uint16_t() const;
	uint8_t hi, lo;
	
};

inline Register_pair::Register_pair(uint16_t rp)
	: hi {static_cast<uint8_t>(rp >> 8 & 0xff)}, 
	  lo {static_cast<uint8_t>(rp & 0xff)}
{}

inline Register_pair &Register_pair::operator+=(const int16_t d)
{
	uint16_t x {*this};
	*this = x + d;
	return *this;
}

inline Register_pair &Register_pair::operator-=(const int16_t d)
{
	uint16_t x {*this};
	*this = x - d;
	return *this;
}

inline Register_pair &Register_pair::operator++()
{
	uint16_t x {*this};
	*this = ++x;
	return *this;
}
	
inline Register_pair Register_pair::operator++(int)
{
	Register_pair tmp {*this};
	++*this;
	return tmp;
}

inline Register_pair &Register_pair::operator--()
{
	uint16_t x {*this};
	*this = --x;
	return *this;
}

inline Register_pair Register_pair::operator--(int)
{
	Register_pair tmp {*this};
	--*this;
	return tmp;
}

inline Register_pair &Register_pair::operator=(const uint16_t x)
{
	hi = x >> 8 & 0xff;
	lo = x & 0xff;
	return *this;
}
	
inline Register_pair::operator uint16_t() const
{
	return (hi << 8) || lo;
}
	
}