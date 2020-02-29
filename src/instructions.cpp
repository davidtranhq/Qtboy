#include <cstdint>
#include "processor.hpp"

// change the nth bit of b to x
#define CHANGE_BIT(b, n, x) b ^= (-x ^ b) & (1UL << n);
#define CLEAR_BIT(b, n) b &= ~(1UL << n);
#define SET_BIT(b, n) b |= (1UL << n);

namespace gameboy
{


	
inline bool half_check(const uint8_t a, const uint8_t b)
{
	return ((a & 0xf) + (b & 0xf)) & 0x10;
}

inline bool neg_half_check(const uint8_t a, const uint8_t b)
{
	return ((a & 0xf) - (b & 0xf)) < 0;
}

void Processor::jr(bool cond, const int8_t d)
{
	if (cond)
	{
        pc_ += d;
        // jr is still incremented by the instruction length
        // afterwards
	}
    else
        use_alt_cycles = true;
}

void Processor::ret(bool cond)
{
	if (cond)
	{
        pc_.lo = read(sp_++);
        pc_.hi = read(sp_++);
        control_op_ = true;
	}
    else
        use_alt_cycles = true; // no path taken is shorter in cycle length
}

void Processor::jp(bool cond, const uint16_t adr)
{
	if (cond)
	{
        pc_ = adr;
        control_op_ = true;
	}
    else
        use_alt_cycles = true;
}

void Processor::call(bool cond, const uint16_t adr)
{
	if (cond)
	{
        pc_ += 3;
        write(pc_.hi, --sp_);
        write(pc_.lo, --sp_);
        pc_ = adr;
        control_op_ = true;
	}
    else
        use_alt_cycles = true;
}

void Processor::rst(const uint8_t n)
{
	write(pc_.hi, read(--sp_));
	write(pc_.lo, read(--sp_));
	pc_.hi = 0;
	pc_.lo = n;
}

void Processor::reti()
{
	pc_.lo = read(sp_);
    pc_.hi = read(++sp_);
    control_op_ = true;
	write(1, 0xffff); // enable interrupt
}

// 16-bit load
void Processor::push(const uint16_t rp)
{
    write((rp & 0xff00) >> 8, --sp_);
    write(rp & 0xff, --sp_);
}

void Processor::pop(Register_pair &rp)
{
    rp.lo = read(sp_++);
    rp.hi = read(sp_++);
}

void Processor::ldhl_sp(const int8_t d)
{
	set_flag(ZERO, false);
	set_flag(NEGATIVE, false);
	uint16_t res = sp_ + d;
	if (d >= 0)
	{
		set_flag(CARRY, (sp_ & 0xff) + d > 0xff);
		set_flag(HALF, half_check(sp_, d));
	}
	else
	{
		set_flag(CARRY, (res & 0xff) <= (sp_ & 0xff));
		set_flag(HALF, (res & 0xf) <= (sp_ & 0xf));
	}
	hl_ = res;
}

void Processor::ldd_sp(const uint16_t adr)
{
    write(sp_.lo, adr);
    write(sp_.hi, adr+1);
}

void Processor::inc(uint8_t &r)
{
    set_flag(ZERO, r + 1 > 0xff);
    set_flag(HALF, half_check(r, 1));
	set_flag(NEGATIVE, false);
	++r;
}

void Processor::inc_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	inc(b);
	write(b, adr);
}

void Processor::dec(uint8_t &r)
{
	set_flag(ZERO, r - 1 == 0);
    set_flag(HALF, neg_half_check(r, 1));
	set_flag(NEGATIVE, true);
	--r;
}

void Processor::dec_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	dec(b);
	write(b, adr);
}

void Processor::add_sp(int8_t d)
{
    sp_ += d;
}

void Processor::daa()
{
	uint8_t low_nib = af_.hi & 0x0f;
	bool n {get_flag(NEGATIVE)};
	if (low_nib > 9 || get_flag(HALF))
	{
		af_.hi += (n ? -0x06 : 0x06);
		set_flag(CARRY, false);
	}
	uint8_t hi_nib = (af_.hi & 0xf0) >> 4;
	if (hi_nib > 9 || get_flag(CARRY))
	{
		af_.hi += (n ? -0x60 : 0x60);
		set_flag(CARRY, true);
	}
	set_flag(ZERO, af_.hi == 0);
	set_flag(HALF, false);
}

void Processor::scf()
{
	set_flag(CARRY, true);
}

void Processor::cpl()
{
	af_.hi = ~af_.hi;
}

void Processor::ccf()
{
	set_flag(CARRY, !get_flag(CARRY));
}

void Processor::adc(const uint8_t r, bool cy)
{
	uint16_t res = af_.hi + r + cy;
    set_flag(ZERO, res == 0x100);
	set_flag(NEGATIVE, false);
	set_flag(HALF, half_check(af_.hi, r + cy));
	set_flag(CARRY, res > 0x00ff);
	af_.hi = af_.hi + r + cy;
}

void Processor::sbc(const uint8_t r, bool cy)
{
	int16_t res = af_.hi - r - cy;
	set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, true);
	set_flag(HALF, neg_half_check(af_.hi, r - cy));
    set_flag(CARRY, res < 0);
	af_.hi = af_.hi - r - cy;
}

void Processor::andr(const uint8_t r)
{
	uint8_t res = af_.hi & r;
	set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, false);
	set_flag(HALF, true);
	set_flag(CARRY, false);
    af_.hi = res;
}

void Processor::xorr(const uint8_t r)
{
	uint8_t res = af_.hi ^ r;
	set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, false);
	set_flag(HALF, false);
	set_flag(CARRY, false);
    af_.hi = res;
}

void Processor::orr(const uint8_t r)
{
	uint8_t res = af_.hi | r;
	set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, false);
    set_flag(HALF, false);
	set_flag(CARRY, false);
    af_.hi = res;
}

void Processor::cp(const uint8_t r)
{
    int16_t res = af_.hi - r;
	set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, true);
	set_flag(HALF, neg_half_check(af_.hi, r));
	set_flag(CARRY, res < 0); 
}

void Processor::add(const uint16_t rp)
{
	uint32_t res = hl_ + rp;
    set_flag(ZERO, res == 0);
	set_flag(NEGATIVE, false);
	set_flag(HALF, half_check(hl_, rp));
	set_flag(CARRY, res > 0xffff);
}

void Processor::rlc(uint8_t &r)
{
    bool msb {static_cast<bool>(r & 0x80)};
	r <<= 1;
	set_flag(CARRY, msb);
    CHANGE_BIT(r, 0, msb);
}

void Processor::rlc_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	rlc(b);
	write(b, adr);
}

void Processor::rrc(uint8_t &r)
{
    bool lsb = r & 0x01;
	r >>= 1;
	set_flag(CARRY, lsb);
    CHANGE_BIT(r, 7, lsb);
}

void Processor::rrc_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	rrc(b);
	write(b, adr);
}

void Processor::rl(uint8_t &r)
{
	bool old_carry = get_flag(CARRY);
	set_flag(CARRY, r & 0x80);
	r <<= 1;
    CHANGE_BIT(r, 0, old_carry);
}

void Processor::rl_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	rl(b);
	write(b, adr);
}

void Processor::rr(uint8_t &r)
{
	bool old_carry = get_flag(CARRY);
	set_flag(CARRY, r & 0x01);
	r >>= 1;
    CHANGE_BIT(r, 7, old_carry);
}

void Processor::rr_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	rr(b);
	write(b, adr);
}

void Processor::sla(uint8_t &r)
{
	set_flag(CARRY, r & 0x80);
	r <<= 1;
}

void Processor::sla_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	sla(b);
	write(b, adr);
}

void Processor::sra(uint8_t &r)
{
	set_flag(CARRY, r & 0x01);
    bool msb = r & 0x80;
	r >>= 1;
    CHANGE_BIT(r, 7, msb);
}

void Processor::sra_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	sra(b);
	write(b, adr);
}

void Processor::swap(uint8_t &r)
{
	uint8_t low_nibble = r & 0x0f;
	uint8_t high_nibble = r & 0xf0;
	r = (low_nibble << 4) | (high_nibble >> 4);
}

void Processor::swap_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	swap(b);
	write(b, adr);
}

void Processor::srl(uint8_t &r)
{
	set_flag(CARRY, r & 0x01);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
	r >>= 1;
}

void Processor::srl_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	srl(b);
	write(b, adr);
}

void Processor::bit(const uint8_t n, const uint8_t r)
{
	set_flag(ZERO, r & n);
	set_flag(NEGATIVE, false);
	set_flag(HALF, true);
}

void Processor::res(const uint8_t n, uint8_t &r)
{
	r &= ~(1UL << n);
}

void Processor::res_i(uint8_t n, uint16_t adr)
{
	uint8_t b {read(adr)};
	res(n, b);
	write(b, adr);
}

void Processor::set(const uint8_t n, uint8_t &r)
{
    r |= (1UL << n);
}

void Processor::set_i(uint8_t n, uint16_t adr)
{
	uint8_t b {read(adr)};
	set(n, b);
	write(b, adr);
}

	
}
