#pragma once

#include <cstdint>
#include <array>
#include <ostream>
#include <functional>

#include "register_pair.hpp"
#include "memory.hpp"
#include "disassembler.hpp"


namespace gameboy
{

class Processor
{
    public:

    enum Interrupt : uint8_t
    {
        VBLANK = 0,
        LCD_STAT,
        TIMER,
        SERIAL,
        JOYPAD,
    };

    Processor(std::function<uint8_t(uint16_t)> rd,
              std::function<void(uint8_t, uint16_t)> wr);
    void step();
    void reset(bool force_dmg = false);
    Cpu_dump dump() const noexcept;
    void request_interrupt(Interrupt i);
    void toggle_double_speed();

    uint16_t af() const noexcept { return af_; }
    uint16_t bc() const noexcept { return bc_; }
    uint16_t de() const noexcept { return de_; }
    uint16_t hl() const noexcept { return hl_; }
    uint16_t sp() const noexcept { return sp_; }
    uint16_t pc() const noexcept { return pc_; }
    uint32_t cycles() const noexcept { return cycles_; }
    bool stopped() const noexcept { return stpd_; }
    bool halted() const noexcept { return hltd_; }
    bool double_speed() const noexcept { return double_speed_; }

    std::vector<uint8_t> next_ops(uint16_t n) const;



    private:

    enum Flags : uint8_t
    {
        CARRY = 1 << 4,
        HALF = 1 << 5,
        NEGATIVE = 1 << 6,
        ZERO = 1 << 7,
    };

    Register_pair af_, bc_, de_, hl_, sp_, pc_;
    uint32_t cycles_ {0};
    bool stpd_ {false};
    bool hltd_ {false};
    bool use_branch_cycles_ {false};
    bool ime_ {false};
    bool ei_set_ {false}, di_set_ {false}; // for tracking when EI and DI are
        // called, since they don't take effect until the next instruction
    bool halt_bug_ {false};
    bool double_speed_ {false}; // CGB only

    std::function<uint8_t(uint16_t)> read;
    std::function<void(uint8_t, uint16_t)> write;
    uint8_t fetch8();
    uint16_t fetch16();

    // flags
    bool get_flag(Flags f) const;
    void set_flag(Flags, bool);

    bool execute_interrupt(Interrupt i);
    bool check_interrupt();

    // instructions
    // misc/control
    void nop() const {}
    void stop() { stpd_ = true; }
    void halt();
    void di();
    void ei();
    // jump and calls
    void jr(bool, const int8_t);
    void ret(bool);
    void jp(bool, const uint16_t);
    void call(bool, const uint16_t);
    void rst(const uint8_t);
    void reti(); // ?
    // 8-bit load
    void ld(uint8_t &a, uint8_t b) { a = b; }
    void ldd(uint16_t adr, uint8_t b) { write(b, adr); }
    void ldd_sp(uint16_t adr);
    void ldr(const Register_pair &rp, uint8_t b) { write(b, rp); }

    // 16-bit load
    void ld(Register_pair &a, const uint16_t b) { a = b; }
    void ldhl_sp(const int8_t);
    void push(const uint16_t);
    void pop(Register_pair &);
    void pop_af(); // special logic for AF needed
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
    void add_sp(int8_t);
    // 8-bit rotations and shifts
    void rlc(uint8_t &);
    void rlca();
    void rlc_i(uint16_t adr);
    void rrc(uint8_t &);
    void rrca();
    void rrc_i(uint16_t adr);
    void rl(uint8_t &);
    void rla();
    void rl_i(uint16_t adr);
    void rr(uint8_t &);
    void rra();
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
