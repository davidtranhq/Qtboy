#pragma once

#include <cstdint>
#include <array>
#include <ostream>

#include "register_pair.hpp"
#include "memory.hpp"
#include "disassembler.hpp"


namespace gameboy
{
	
class Processor
{
	public:
	
	Processor(Memory &mem,
		uint16_t af = 0, uint16_t bc = 0, uint16_t de = 0, uint16_t hl = 0, 
		uint16_t sp = 0, uint16_t pc = 0);
	void step();
	void dump(std::ostream &os);
	
	private:
	enum Flags : uint8_t
	{
		CARRY = 1 << 4,
		HALF = 1 << 5,
		NEGATIVE = 1 << 6,
		ZERO = 1 << 7,
	};

	
	Memory &memory_;
	Register_pair af_, bc_, de_, hl_, sp_, pc_;
	Disassembler disassembler_;
	bool stpd_ {false};
	bool hltd_ {false};
	uint32_t cycles_ {0};
	
	void write(uint8_t b, uint16_t adr) { memory_.write(b, adr); }
	uint8_t read(uint16_t adr) const { return memory_.read(adr); };
	
	// flags
	bool get_flag(Flags f) const;
	void set_flag(Flags, bool);
	
	// instructions
	// misc/control
	void nop() const {};
	void stop() { stpd_ = true; }
	void halt() { hltd_ = true; }
	void di() {memory_.write(0, 0xffff); }
	void ei() {memory_.write(1, 0xffff); }
	// jump and calls
	void jr(bool, const int8_t);
	void ret(bool);
	void jp(bool, const uint16_t);
	void call(bool, const uint16_t);
	void rst(const uint8_t);
	void reti(); // ?
	// 8-bit load
	void ld(uint8_t &a, uint8_t b) { a = b; }
	void ldd(uint16_t adr, uint8_t b) { write(adr, b); }
	void ldr(const Register_pair &rp, uint8_t b) { write(rp, b); }
	
	// 16-bit load
	void ld(Register_pair &a, const uint16_t b) { a = b; }
	void ldhl_sp(const int8_t);
	void push(const uint16_t);
	void pop(Register_pair &);
	// 8-bit arithmetic and logic
	void inc(uint8_t &);
	void inc_i(uint16_t adr); // we could refactor this indirect addressing
							  // using decorators with templates (todo...)
	void dec(uint8_t &);
	void dec_i(uint16_t adr);
	void daa();
	void scf();
	void cpl();
	void ccf();
	void adc(const uint8_t, bool);
	void sbc(const uint8_t, bool);
	void andr(const uint8_t);
	void xorr(const uint8_t);
	void orr(const uint8_t);
	void cp(const uint8_t);
	// 16-bit arithmetic and logic
	void inc(Register_pair &rp) { ++rp; }
	void dec(Register_pair &rp) { --rp; }
	void add(const uint16_t);
	// 8-bit rotations and shifts
	void rlc(uint8_t &);
	void rlc_i(uint16_t adr);
	void rrc(uint8_t &);
	void rrc_i(uint16_t adr);
	void rl(uint8_t &);
	void rl_i(uint16_t adr);
	void rr(uint8_t &);
	void rr_i(uint16_t adr);
	void sla(uint8_t &);
	void sla_i(uint16_t adr);
	void sra(uint8_t &);
	void sra_i(uint16_t adr);
	void swap(uint8_t &);
	void swap_i(uint16_t adr);
	void srl(uint8_t &);
	void srl_i(uint16_t adr);
	void bit(const uint8_t, const uint8_t);
	void res(const uint8_t, uint8_t &);
	void res_i(uint8_t, uint16_t adr);
	void set(const uint8_t, uint8_t &);
	void set_i(uint8_t, uint16_t adr);
	
	static constexpr uint16_t IO_MEMORY {0xff00};
};
	
}