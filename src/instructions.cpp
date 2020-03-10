#include <cstdint>
#include "processor.hpp"

// change the nth bit of b to x
#define CHANGE_BIT(b, n, x) b ^= (-x ^ b) & (1UL << n)
#define CLEAR_BIT(b, n) b &= ~(1UL << n)
#define SET_BIT(b, n) b |= (1UL << n)

namespace gameboy
{

inline bool half_check(const uint8_t a, const uint8_t b, const uint8_t res)
{
    return ((a ^ b ^ res) & 0x10);
}

inline bool half_check_16(const uint16_t a, const uint16_t b, const uint16_t res)
{
    return ((a ^ b ^ res) & 0x1000);
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
    ++pc_;
    write(pc_.hi, --sp_);
    write(pc_.lo, --sp_);
	pc_.hi = 0;
	pc_.lo = n;
    control_op_ = true;
}

void Processor::reti()
{
    pc_.lo = read(sp_++);
    pc_.hi = read(sp_++);
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

void Processor::pop_af()
{
    af_.lo = read(sp_++);
    af_.hi = read(sp_++);
    af_.lo &= 0xf0; // the lower 4 bits of the f register are unused
}

void Processor::ldhl_sp(const int8_t d)
{
	set_flag(ZERO, false);
	set_flag(NEGATIVE, false);
    uint16_t res = sp_ + static_cast<uint16_t>(d);
	if (d >= 0)
	{
		set_flag(CARRY, (sp_ & 0xff) + d > 0xff);
        set_flag(HALF, half_check(sp_.lo, static_cast<uint8_t>(d), res & 0xff));
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
    set_flag(HALF, half_check(r, 1, r+1));
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
    set_flag(HALF, half_check(r, 1, r-1));
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
    uint16_t res = sp_ + d;
    set_flag(ZERO, false);
    set_flag(NEGATIVE, false);
    set_flag(HALF, (res & 0xf) < (sp_ & 0xf));
    set_flag(CARRY, (res & 0xff) < (sp_ & 0xff));
    sp_ = res;
}

void Processor::daa()
{
    uint8_t a = af_.hi;
    if (!get_flag(NEGATIVE))
    {
        if (get_flag(CARRY) || a > 0x99)
        {
            a += 0x60;
            set_flag(CARRY, true);
        }
        if (get_flag(HALF) || (a & 0x0f) > 0x09)
        {
            a += 0x6;
        }
    }
    else
    {
        if (get_flag(CARRY))
            a -= 0x60;
        if (get_flag(HALF))
            a -= 0x6;
    }
    set_flag(ZERO, a == 0);
    set_flag(HALF, false);
    af_.hi = a;
}

void Processor::scf()
{
	set_flag(CARRY, true);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
}

void Processor::cpl()
{
	af_.hi = ~af_.hi;
    set_flag(NEGATIVE, true);
    set_flag(HALF, true);
}

void Processor::ccf()
{
	set_flag(CARRY, !get_flag(CARRY));
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
}

void Processor::adc(const uint8_t r, bool cy)
{
	uint16_t res = af_.hi + r + cy;
    uint8_t res8 = af_.hi + r + cy;
    set_flag(ZERO, res8 == 0);
	set_flag(NEGATIVE, false);
    set_flag(HALF, half_check(af_.hi, r, res8));
	set_flag(CARRY, res > 0x00ff);
	af_.hi = af_.hi + r + cy;
}

void Processor::sbc(const uint8_t r, bool cy)
{
    uint16_t res = af_.hi - r - cy;
    uint8_t res8 = af_.hi - r - cy;
    set_flag(ZERO, res8 == 0);
	set_flag(NEGATIVE, true);
    set_flag(HALF, half_check(af_.hi, r, res8));
    set_flag(CARRY, res > 0xff);
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
    set_flag(HALF, half_check(af_.hi, r, res & 0xff));
	set_flag(CARRY, res < 0); 
}

void Processor::add(const uint16_t rp)
{
    set_flag(NEGATIVE, false);
    set_flag(HALF, half_check_16(hl_, rp, hl_ + rp));
    set_flag(CARRY, (hl_ + rp) > 0xffff);
    hl_ += rp;
}

void Processor::rlc(uint8_t &r)
{
    bool msb {static_cast<bool>(r & 0x80)};
	r <<= 1;
    CHANGE_BIT(r, 0, msb);
    set_flag(ZERO, r == 0);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
    set_flag(CARRY, msb);
}

void Processor::rlca()
{
    rlc(af_.hi);
    set_flag(ZERO, false);
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
    CHANGE_BIT(r, 7, lsb);
    set_flag(CARRY, lsb);
    set_flag(ZERO, r == 0);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
}

// 0x0f (rrca) modifies flags differently than cb 0f
void Processor::rrca()
{
    rrc(af_.hi);
    set_flag(ZERO, false);
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
    set_flag(ZERO, r == 0);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
}

void Processor::rla()
{
    rl(af_.hi);
    set_flag(ZERO, false);
}

void Processor::rl_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	rl(b);
	write(b, adr);
}

// 0x1f modifies flags differently to cb 1f
void Processor::rra()
{
    rr(af_.hi);
    set_flag(ZERO, false);
}

void Processor::rr(uint8_t &r)
{
	bool old_carry = get_flag(CARRY);
	set_flag(CARRY, r & 0x01);
	r >>= 1;
    CHANGE_BIT(r, 7, old_carry);
    set_flag(ZERO, r == 0);
    set_flag(HALF, false);
    set_flag(NEGATIVE, false);
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
    set_flag(ZERO, r == 0);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
}

void Processor::sla_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	sla(b);
	write(b, adr);
}

void Processor::sra(uint8_t &r)
{
    bool lsb = r & 1;
    r = (r >> 1) | (r & 0x80);
    set_flag(ZERO, r == 0);
    set_flag(NEGATIVE, false);
    set_flag(HALF, false);
    set_flag(CARRY, lsb);
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
    set_flag(ZERO, r == 0);
    set_flag(HALF, false);
    set_flag(NEGATIVE, false);
    set_flag(CARRY, false);
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
    set_flag(ZERO, r == 0);
}

void Processor::srl_i(uint16_t adr)
{
	uint8_t b {read(adr)};
	srl(b);
	write(b, adr);
}

void Processor::bit(const uint8_t n, const uint8_t r)
{
    set_flag(ZERO, !(r & (1 << n))); // set Z if bit is NOT set
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
