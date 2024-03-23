#include <iomanip>
#include <sstream>
#include <chrono>
#include <ctime>

#include "processor.hpp"
#include "register_pair.hpp"
#include "instruction_info.hpp"
#include "exception.hpp"

#define B bc_.hi
#define C bc_.lo
#define D de_.hi
#define E de_.lo
#define H hl_.hi
#define L hl_.lo
#define A af_.hi
#define F af_.lo
#define SPH sp_.hi
#define SPL sp_.lo
#define PCH pc_.hi
#define PCL pc_.lo

#define BC bc_
#define DE de_
#define HL hl_
#define AF af_
#define SP sp_
#define PC pc_

#define SET_BIT(b, n) b |= (1UL << n)
#define CLEAR_BIT(b, n) b &= ~(1UL << n)

namespace qtboy
{

Processor::Processor(std::function<uint8_t(uint16_t)> rd,
    std::function<void(uint8_t, uint16_t)> wr)
    : read {std::move(rd)},
      write {std::move(wr)}
{
    reset();
}

void Processor::reset(bool force_dmg)
{
    af_ = force_dmg ? 0x01b0 : 0x11b0;
    bc_ = 0x0013;
    de_ = 0x00d8;
    hl_ = 0x014d;
    sp_ = 0xfffe;
    pc_ = 0x0100;
    cycles_ = 0;
    stpd_ = false;
    hltd_ = false;
    use_branch_cycles_ = false;
    ime_ = false;
    ei_set_ = false;
    di_set_ = false;
    halt_bug_ = false;
    double_speed_ = false;
}

void Processor::add_cycles(uint32_t c)
{
    cycles_ += c;
}

std::vector<uint8_t> Processor::next_ops(uint16_t n) const
{
    std::vector<uint8_t> out {};
    for (uint16_t i {0}; i < n; ++i)
        out.push_back(read(pc_+i));
    return out;
}

bool Processor::get_flag(Flags f) const
{
    return (F & f);
}

void Processor::set_flag(Flags f, bool cond)
{
    uint8_t mask;
    if (cond)
        mask = 0xff;
    else
        mask = 0;
    F = (F & ~f) | (mask & f);
}

uint8_t Processor::fetch8()
{
    uint8_t op = read(PC);
    // HALT bug: the processor fails to increment the PC
    if (halt_bug_)
        halt_bug_ = false;
    else
        ++PC;
    return op;

}

uint16_t Processor::fetch16()
{
    uint8_t lo = fetch8();
    uint8_t hi = fetch8();
    return static_cast<uint16_t>(hi << 8 | lo);
}

bool Processor::execute_interrupt(Interrupt i)
{
    hltd_ = false;
    stpd_ = false;
    if (!ime_)
        return false; // don't service the interrupt if IME is disabled
    /*
    if (i == VBLANK)
    {
        static auto last = std::chrono::high_resolution_clock::now();
        auto now = std::chrono::high_resolution_clock::now();
        uint64_t frame_time = std::chrono::duration_cast<std::chrono::milliseconds>(now-last).count();
        std::cout << "Frame time: " << frame_time << "ms\n";
        last = now;
    }
    */

    write(pc_.hi, --sp_);
    write(pc_.lo, --sp_);
    pc_.hi = 0;
    pc_.lo = 0x40 + i*8;
    ime_ = false;
    write(0, 0xff0f); // clear IF
    return true;

}

bool Processor::check_interrupt()
{
    uint8_t int_enable {read(0xffff)};
    uint8_t int_flag {read(0xff0f)};
    uint8_t request = int_enable & int_flag;
    bool serviced {false};
    if (request & 1)
        serviced = execute_interrupt(VBLANK);
    else if (request & 1 << 1)
        serviced = execute_interrupt(LCD_STAT);
    else if (request & 1 << 2)
        serviced = execute_interrupt(TIMER);
    else if (request & 1 << 3)
        serviced = execute_interrupt(SERIAL);
    else if (request & 1 << 4)
        serviced = execute_interrupt(JOYPAD);
    return serviced;
}

void Processor::request_interrupt(Interrupt i)
{
    uint8_t int_flag {read(0xff0f)};
    SET_BIT(int_flag, i);
    write(int_flag, 0xff0f);
}

void Processor::toggle_double_speed()
{
    double_speed_ = !double_speed_;
}

void Processor::step()
{
    if (check_interrupt())
        return;
    if (ei_set_) // EI was called last cycle
    {
        ei_set_ = false;
        ime_ = true;
    }
    else if (di_set_) // DI was called last cycle
    {
        di_set_ = false;
        ime_ = false;
    }
    if (hltd_)
    {
        cycles_ += 4; // assume NOP when CPU is halted
        return;
    }
    uint8_t op {fetch8()};
    switch (op)
    {
        // misc and control
        case 0x00: nop(); break;
        case 0x10: stop(); break;
        case 0x76: halt(); break;
        case 0xf3: di(); break;
        case 0xfb: ei(); break;

        // jump and calls
        case 0x18: jr(true, static_cast<int8_t>(fetch8())); break;
        case 0x20: jr(!get_flag(ZERO), static_cast<int8_t>(fetch8())); break;
        case 0x28: jr(get_flag(ZERO), static_cast<int8_t>(fetch8())); break;
        case 0x30: jr(!get_flag(CARRY), static_cast<int8_t>(fetch8())); break;
        case 0x38: jr(get_flag(CARRY), static_cast<int8_t>(fetch8())); break;

        case 0xc0: ret(!get_flag(ZERO)); break;
        case 0xc8: ret(get_flag(ZERO)); break;
        case 0xc9: ret(true); break;
        case 0xd0: ret(!get_flag(CARRY)); break;
        case 0xd8: ret(get_flag(CARRY)); break;

        case 0xc2: jp(!get_flag(ZERO), fetch16()); break;
        case 0xc3: jp(true, fetch16()); break;
        case 0xca: jp(get_flag(ZERO), fetch16()); break;
        case 0xd2: jp(!get_flag(CARRY), fetch16()); break;
        case 0xda: jp(get_flag(CARRY), fetch16()); break;

        case 0xc4: call(!get_flag(ZERO), fetch16()); break;
        case 0xcc: call(get_flag(ZERO), fetch16()); break;
        case 0xcd: call(true, fetch16()); break;
        case 0xd4: call(!get_flag(CARRY), fetch16()); break;
        case 0xdc: call(get_flag(CARRY), fetch16()); break;

        case 0xe9: jp(true, HL); break;

        case 0xc7: rst(0x00); break;
        case 0xcf: rst(0x08); break;
        case 0xd7: rst(0x10); break;
        case 0xdf: rst(0x18); break;
        case 0xe7: rst(0x20); break;
        case 0xef: rst(0x28); break;
        case 0xf7: rst(0x30); break;
        case 0xff: rst(0x38); break;

        case 0xd9: reti(); break;

        // 8-bit load
        case 0x02: ldr(BC, A); break;
        case 0x12: ldr(DE, A); break;
        case 0x22: ldr(HL++, A); break;
        case 0x32: ldr(HL--, A); break;

        case 0x06: ld(B, fetch8()); break;
        case 0x0e: ld(C, fetch8()); break;
        case 0x16: ld(D, fetch8()); break;
        case 0x1e: ld(E, fetch8()); break;
        case 0x26: ld(H, fetch8()); break;
        case 0x2e: ld(L, fetch8()); break;
        case 0x36: ldr(HL, fetch8()); break;
        case 0x3e: ld(A, fetch8()); break;

        case 0x0a: ld(A, read(BC)); break;
        case 0x1a: ld(A, read(DE)); break;
        case 0x2a: ld(A, read(HL++)); break;
        case 0x3a: ld(A, read(HL--)); break;

        case 0x40: break;
        case 0x41: ld(B, C); break;
        case 0x42: ld(B, D); break;
        case 0x43: ld(B, E); break;
        case 0x44: ld(B, H); break;
        case 0x45: ld(B, L); break;
        case 0x46: ld(B, read(HL)); break;
        case 0x47: ld(B, A); break;

        case 0x48: ld(C, B); break;
        case 0x49: break;
        case 0x4a: ld(C, D); break;
        case 0x4b: ld(C, E); break;
        case 0x4c: ld(C, H); break;
        case 0x4d: ld(C, L); break;
        case 0x4e: ld(C, read(HL)); break;
        case 0x4f: ld(C, A); break;

        case 0x50: ld(D, B); break;
        case 0x51: ld(D, C); break;
        case 0x52: break;
        case 0x53: ld(D, E); break;
        case 0x54: ld(D, H); break;
        case 0x55: ld(D, L); break;
        case 0x56: ld(D, read(HL)); break;
        case 0x57: ld(D, A); break;

        case 0x58: ld(E, B); break;
        case 0x59: ld(E, C); break;
        case 0x5a: ld(E, D); break;
        case 0x5b: break;
        case 0x5c: ld(E, H); break;
        case 0x5d: ld(E, L); break;
        case 0x5e: ld(E, read(HL)); break;
        case 0x5f: ld(E, A); break;

        case 0x60: ld(H, B); break;
        case 0x61: ld(H, C); break;
        case 0x62: ld(H, D); break;
        case 0x63: ld(H, E); break;
        case 0x64: break;
        case 0x65: ld(H, L); break;
        case 0x66: ld(H, read(HL)); break;
        case 0x67: ld(H, A); break;

        case 0x68: ld(L, B); break;
        case 0x69: ld(L, C); break;
        case 0x6a: ld(L, D); break;
        case 0x6b: ld(L, E); break;
        case 0x6c: ld(L, H); break;
        case 0x6d: break;
        case 0x6e: ld(L, read(HL)); break;
        case 0x6f: ld(L, A); break;

        case 0x70: ldr(HL, B); break;
        case 0x71: ldr(HL, C); break;
        case 0x72: ldr(HL, D); break;
        case 0x73: ldr(HL, E); break;
        case 0x74: ldr(HL, H); break;
        case 0x75: ldr(HL, L); break;
        case 0x77: ldr(HL, A); break;

        case 0x78: ld(A, B); break;
        case 0x79: ld(A, C); break;
        case 0x7a: ld(A, D); break;
        case 0x7b: ld(A, E); break;
        case 0x7c: ld(A, H); break;
        case 0x7d: ld(A, L); break;
        case 0x7e: ld(A, read(HL)); break;
        case 0x7f: break;

        case 0xe0: ldd(IO_MEMORY + fetch8(), A); break; // ldh (a8),A
        case 0xea: ldd(fetch16(), A); break; // ld (a16),A
        case 0xf0: ld(A, read(IO_MEMORY + fetch8())); break; // ldh A,(a8)
        case 0xfa: ld(A, read(fetch16())); break; // ldh A,(a16)
        case 0xe2: ldd(IO_MEMORY + C, A); break; // ld (C),A
        case 0xf2: ld(A, read(IO_MEMORY + C)); break; // ld A,(C)

        // 16-bit load
        case 0x01: ld(BC, fetch16()); break;
        case 0x11: ld(DE, fetch16()); break;
        case 0x21: ld(HL, fetch16()); break;
        case 0x31: ld(SP, fetch16()); break;
        case 0x08: ldd_sp(fetch16()); break;

        case 0xc1: pop(BC); break;
        case 0xd1: pop(DE); break;
        case 0xe1: pop(HL); break;
        case 0xf1: pop_af(); break;

        case 0xc5: push(BC); break;
        case 0xd5: push(DE); break;
        case 0xe5: push(HL); break;
        case 0xf5: push(AF); break;

        case 0xf8: ldhl_sp(fetch8()); break;
        case 0xf9: ld(SP, HL); break;

        // 8-bit arithmetic
        case 0x27: daa(); break;
        case 0x37: scf(); break;
        case 0x2f: cpl(); break;
        case 0x3f: ccf(); break;

        case 0x04: inc(B); break;
        case 0x0c: inc(C); break;
        case 0x14: inc(D); break;
        case 0x1c: inc(E); break;
        case 0x24: inc(H); break;
        case 0x2c: inc(L); break;
        case 0x34: inc_i(HL); break;
        case 0x3c: inc(A); break;

        case 0x05: dec(B); break;
        case 0x0d: dec(C); break;
        case 0x15: dec(D); break;
        case 0x1d: dec(E); break;
        case 0x25: dec(H); break;
        case 0x2d: dec(L); break;
        case 0x35: dec_i(HL); break;
        case 0x3d: dec(A); break;

        case 0x80: adc(B, false); break;
        case 0x81: adc(C, false); break;
        case 0x82: adc(D, false); break;
        case 0x83: adc(E, false); break;
        case 0x84: adc(H, false); break;
        case 0x85: adc(L, false); break;
        case 0x86: adc(read(HL), false); break;
        case 0x87: adc(A, false); break;

        case 0x88: adc(B, get_flag(CARRY)); break;
        case 0x89: adc(C, get_flag(CARRY)); break;
        case 0x8a: adc(D, get_flag(CARRY)); break;
        case 0x8b: adc(E, get_flag(CARRY)); break;
        case 0x8c: adc(H, get_flag(CARRY)); break;
        case 0x8d: adc(L, get_flag(CARRY)); break;
        case 0x8e: adc(read(HL), get_flag(CARRY)); break;
        case 0x8f: adc(A, get_flag(CARRY)); break;

        case 0x90: sbc(B, false); break;
        case 0x91: sbc(C, false); break;
        case 0x92: sbc(D, false); break;
        case 0x93: sbc(E, false); break;
        case 0x94: sbc(H, false); break;
        case 0x95: sbc(L, false); break;
        case 0x96: sbc(read(HL), false); break;
        case 0x97: sbc(A, false); break;

        case 0x98: sbc(B, get_flag(CARRY)); break;
        case 0x99: sbc(C, get_flag(CARRY)); break;
        case 0x9a: sbc(D, get_flag(CARRY)); break;
        case 0x9b: sbc(E, get_flag(CARRY)); break;
        case 0x9c: sbc(H, get_flag(CARRY)); break;
        case 0x9d: sbc(L, get_flag(CARRY)); break;
        case 0x9e: sbc(read(HL), get_flag(CARRY)); break;
        case 0x9f: sbc(A, get_flag(CARRY)); break;

        case 0xa0: andr(B); break;
        case 0xa1: andr(C); break;
        case 0xa2: andr(D); break;
        case 0xa3: andr(E); break;
        case 0xa4: andr(H); break;
        case 0xa5: andr(L); break;
        case 0xa6: andr(read(HL)); break;
        case 0xa7: andr(A); break;

        case 0xa8: xorr(B); break;
        case 0xa9: xorr(C); break;
        case 0xaa: xorr(D); break;
        case 0xab: xorr(E); break;
        case 0xac: xorr(H); break;
        case 0xad: xorr(L); break;
        case 0xae: xorr(read(HL)); break;
        case 0xaf: xorr(A); break;

        case 0xb0: orr(B); break;
        case 0xb1: orr(C); break;
        case 0xb2: orr(D); break;
        case 0xb3: orr(E); break;
        case 0xb4: orr(H); break;
        case 0xb5: orr(L); break;
        case 0xb6: orr(read(HL)); break;
        case 0xb7: orr(A); break;

        case 0xb8: cp(B); break;
        case 0xb9: cp(C); break;
        case 0xba: cp(D); break;
        case 0xbb: cp(E); break;
        case 0xbc: cp(H); break;
        case 0xbd: cp(L); break;
        case 0xbe: cp(read(HL)); break;
        case 0xbf: cp(A); break;

        case 0xc6: adc(fetch8(), false); break;
        case 0xce: adc(fetch8(), get_flag(CARRY)); break;
        case 0xd6: sbc(fetch8(), false); break;
        case 0xde: sbc(fetch8(), get_flag(CARRY)); break;
        case 0xe6: andr(fetch8()); break;
        case 0xee: xorr(fetch8()); break;
        case 0xf6: orr(fetch8()); break;
        case 0xfe: cp(fetch8()); break;

        // 16-bit arithmetic
        case 0x03: inc(BC); break;
        case 0x13: inc(DE); break;
        case 0x23: inc(HL); break;
        case 0x33: inc(SP); break;

        case 0x09: add(BC); break;
        case 0x19: add(DE); break;
        case 0x29: add(HL); break;
        case 0x39: add(SP); break;

        case 0x0b: dec(BC); break;
        case 0x1b: dec(DE); break;
        case 0x2b: dec(HL); break;
        case 0x3b: dec(SP); break;

        case 0xe8: add_sp(static_cast<int8_t>(fetch8())); break;

        // bit operations
        case 0x07: rlca(); break;
        case 0x0f: rrca(); break;
        case 0x17: rla(); break;
        case 0x1f: rra(); break;

        // prefix cb
        case 0xcb:
        {

            switch (fetch8())
            {
                case 0x00: rlc(B); break;
                case 0x01: rlc(C); break;
                case 0x02: rlc(D); break;
                case 0x03: rlc(E); break;
                case 0x04: rlc(H); break;
                case 0x05: rlc(L); break;
                case 0x06: rlc_i(HL); break;
                case 0x07: rlc(A); break;

                case 0x08: rrc(B); break;
                case 0x09: rrc(C); break;
                case 0x0a: rrc(D); break;
                case 0x0b: rrc(E); break;
                case 0x0c: rrc(H); break;
                case 0x0d: rrc(L); break;
                case 0x0e: rrc_i(HL); break;
                case 0x0f: rrc(A); break;

                case 0x10: rl(B); break;
                case 0x11: rl(C); break;
                case 0x12: rl(D); break;
                case 0x13: rl(E); break;
                case 0x14: rl(H); break;
                case 0x15: rl(L); break;
                case 0x16: rl_i(HL); break;
                case 0x17: rl(A); break;

                case 0x18: rr(B); break;
                case 0x19: rr(C); break;
                case 0x1a: rr(D); break;
                case 0x1b: rr(E); break;
                case 0x1c: rr(H); break;
                case 0x1d: rr(L); break;
                case 0x1e: rr_i(HL); break;
                case 0x1f: rr(A); break;

                case 0x20: sla(B); break;
                case 0x21: sla(C); break;
                case 0x22: sla(D); break;
                case 0x23: sla(E); break;
                case 0x24: sla(H); break;
                case 0x25: sla(L); break;
                case 0x26: sla_i(HL); break;
                case 0x27: sla(A); break;

                case 0x28: sra(B); break;
                case 0x29: sra(C); break;
                case 0x2a: sra(D); break;
                case 0x2b: sra(E); break;
                case 0x2c: sra(H); break;
                case 0x2d: sra(L); break;
                case 0x2e: sra_i(HL); break;
                case 0x2f: sra(A); break;

                case 0x30: swap(B); break;
                case 0x31: swap(C); break;
                case 0x32: swap(D); break;
                case 0x33: swap(E); break;
                case 0x34: swap(H); break;
                case 0x35: swap(L); break;
                case 0x36: swap_i(HL); break;
                case 0x37: swap(A); break;

                case 0x38: srl(B); break;
                case 0x39: srl(C); break;
                case 0x3a: srl(D); break;
                case 0x3b: srl(E); break;
                case 0x3c: srl(H); break;
                case 0x3d: srl(L); break;
                case 0x3e: srl_i(HL); break;
                case 0x3f: srl(A); break;

                // BIT x,r
                // 00xx x000
                // xxx = bit to change
                case 0x40: bit(0, B); break;
                case 0x41: bit(0, C); break;
                case 0x42: bit(0, D); break;
                case 0x43: bit(0, E); break;
                case 0x44: bit(0, H); break;
                case 0x45: bit(0, L); break;
                case 0x46: bit(0, read(HL)); break;
                case 0x47: bit(0, A); break;

                case 0x48: bit(1, B); break;
                case 0x49: bit(1, C); break;
                case 0x4a: bit(1, D); break;
                case 0x4b: bit(1, E); break;
                case 0x4c: bit(1, H); break;
                case 0x4d: bit(1, L); break;
                case 0x4e: bit(1, read(HL)); break;
                case 0x4f: bit(1, A); break;

                case 0x50: bit(2, B); break;
                case 0x51: bit(2, C); break;
                case 0x52: bit(2, D); break;
                case 0x53: bit(2, E); break;
                case 0x54: bit(2, H); break;
                case 0x55: bit(2, L); break;
                case 0x56: bit(2, read(HL)); break;
                case 0x57: bit(2, A); break;

                case 0x58: bit(3, B); break;
                case 0x59: bit(3, C); break;
                case 0x5a: bit(3, D); break;
                case 0x5b: bit(3, E); break;
                case 0x5c: bit(3, H); break;
                case 0x5d: bit(3, L); break;
                case 0x5e: bit(3, read(HL)); break;
                case 0x5f: bit(3, A); break;

                case 0x60: bit(4, B); break;
                case 0x61: bit(4, C); break;
                case 0x62: bit(4, D); break;
                case 0x63: bit(4, E); break;
                case 0x64: bit(4, H); break;
                case 0x65: bit(4, L); break;
                case 0x66: bit(4, read(HL)); break;
                case 0x67: bit(4, A); break;

                case 0x68: bit(5, B); break;
                case 0x69: bit(5, C); break;
                case 0x6a: bit(5, D); break;
                case 0x6b: bit(5, E); break;
                case 0x6c: bit(5, H); break;
                case 0x6d: bit(5, L); break;
                case 0x6e: bit(5, read(HL)); break;
                case 0x6f: bit(5, A); break;

                case 0x70: bit(6, B); break;
                case 0x71: bit(6, C); break;
                case 0x72: bit(6, D); break;
                case 0x73: bit(6, E); break;
                case 0x74: bit(6, H); break;
                case 0x75: bit(6, L); break;
                case 0x76: bit(6, read(HL)); break;
                case 0x77: bit(6, A); break;

                case 0x78: bit(7, B); break;
                case 0x79: bit(7, C); break;
                case 0x7a: bit(7, D); break;
                case 0x7b: bit(7, E); break;
                case 0x7c: bit(7, H); break;
                case 0x7d: bit(7, L); break;
                case 0x7e: bit(7, read(HL)); break;
                case 0x7f: bit(7, A); break;

                case 0x80: res(0, B); break;
                case 0x81: res(0, C); break;
                case 0x82: res(0, D); break;
                case 0x83: res(0, E); break;
                case 0x84: res(0, H); break;
                case 0x85: res(0, L); break;
                case 0x86: res_i(0, HL); break;
                case 0x87: res(0, A); break;

                case 0x88: res(1, B); break;
                case 0x89: res(1, C); break;
                case 0x8a: res(1, D); break;
                case 0x8b: res(1, E); break;
                case 0x8c: res(1, H); break;
                case 0x8d: res(1, L); break;
                case 0x8e: res_i(1, HL); break;
                case 0x8f: res(1, A); break;

                case 0x90: res(2, B); break;
                case 0x91: res(2, C); break;
                case 0x92: res(2, D); break;
                case 0x93: res(2, E); break;
                case 0x94: res(2, H); break;
                case 0x95: res(2, L); break;
                case 0x96: res_i(2, HL); break;
                case 0x97: res(2, A); break;

                case 0x98: res(3, B); break;
                case 0x99: res(3, C); break;
                case 0x9a: res(3, D); break;
                case 0x9b: res(3, E); break;
                case 0x9c: res(3, H); break;
                case 0x9d: res(3, L); break;
                case 0x9e: res_i(3, HL); break;
                case 0x9f: res(3, A); break;

                case 0xa0: res(4, B); break;
                case 0xa1: res(4, C); break;
                case 0xa2: res(4, D); break;
                case 0xa3: res(4, E); break;
                case 0xa4: res(4, H); break;
                case 0xa5: res(4, L); break;
                case 0xa6: res_i(4, HL); break;
                case 0xa7: res(4, A); break;

                case 0xa8: res(5, B); break;
                case 0xa9: res(5, C); break;
                case 0xaa: res(5, D); break;
                case 0xab: res(5, E); break;
                case 0xac: res(5, H); break;
                case 0xad: res(5, L); break;
                case 0xae: res_i(5, HL); break;
                case 0xaf: res(5, A); break;

                case 0xb0: res(6, B); break;
                case 0xb1: res(6, C); break;
                case 0xb2: res(6, D); break;
                case 0xb3: res(6, E); break;
                case 0xb4: res(6, H); break;
                case 0xb5: res(6, L); break;
                case 0xb6: res_i(6, HL); break;
                case 0xb7: res(6, A); break;

                case 0xb8: res(7, B); break;
                case 0xb9: res(7, C); break;
                case 0xba: res(7, D); break;
                case 0xbb: res(7, E); break;
                case 0xbc: res(7, H); break;
                case 0xbd: res(7, L); break;
                case 0xbe: res_i(7, HL); break;
                case 0xbf: res(7, A); break;

                case 0xc0: set(0, B); break;
                case 0xc1: set(0, C); break;
                case 0xc2: set(0, D); break;
                case 0xc3: set(0, E); break;
                case 0xc4: set(0, H); break;
                case 0xc5: set(0, L); break;
                case 0xc6: set_i(0, HL); break;
                case 0xc7: set(0, A); break;

                case 0xc8: set(1, B); break;
                case 0xc9: set(1, C); break;
                case 0xca: set(1, D); break;
                case 0xcb: set(1, E); break;
                case 0xcc: set(1, H); break;
                case 0xcd: set(1, L); break;
                case 0xce: set_i(1, HL); break;
                case 0xcf: set(1, A); break;

                case 0xd0: set(2, B); break;
                case 0xd1: set(2, C); break;
                case 0xd2: set(2, D); break;
                case 0xd3: set(2, E); break;
                case 0xd4: set(2, H); break;
                case 0xd5: set(2, L); break;
                case 0xd6: set_i(2, HL); break;
                case 0xd7: set(2, A); break;

                case 0xd8: set(3, B); break;
                case 0xd9: set(3, C); break;
                case 0xda: set(3, D); break;
                case 0xdb: set(3, E); break;
                case 0xdc: set(3, H); break;
                case 0xdd: set(3, L); break;
                case 0xde: set_i(3, HL); break;
                case 0xdf: set(3, A); break;

                case 0xe0: set(4, B); break;
                case 0xe1: set(4, C); break;
                case 0xe2: set(4, D); break;
                case 0xe3: set(4, E); break;
                case 0xe4: set(4, H); break;
                case 0xe5: set(4, L); break;
                case 0xe6: set_i(4, HL); break;
                case 0xe7: set(4, A); break;

                case 0xe8: set(5, B); break;
                case 0xe9: set(5, C); break;
                case 0xea: set(5, D); break;
                case 0xeb: set(5, E); break;
                case 0xec: set(5, H); break;
                case 0xed: set(5, L); break;
                case 0xee: set_i(5, HL); break;
                case 0xef: set(5, A); break;

                case 0xf0: set(6, B); break;
                case 0xf1: set(6, C); break;
                case 0xf2: set(6, D); break;
                case 0xf3: set(6, E); break;
                case 0xf4: set(6, H); break;
                case 0xf5: set(6, L); break;
                case 0xf6: set_i(6, HL); break;
                case 0xf7: set(6, A); break;

                case 0xf8: set(7, B); break;
                case 0xf9: set(7, C); break;
                case 0xfa: set(7, D); break;
                case 0xfb: set(7, E); break;
                case 0xfc: set(7, H); break;
                case 0xfd: set(7, L); break;
                case 0xfe: set_i(7, HL); break;
                case 0xff: set(7, A); break;
            }
        } break;
        default:
            std::ostringstream error {};
            error << "Unimplemented opcode: " << std::hex << static_cast<int>(op);
            throw std::runtime_error {error.str()};
    }
    // increment cycles
    size_t cycles_passed;
    // handle cycle timings of CB prefix instructions
    if (op == 0xcb)
    {
        // get op after prefix
        uint8_t op2 = read(PC-1);
        cycles_passed = cb_instructions[op2].cycles;

    }
    // handle all other instructions
    else
    {
        cycles_passed = use_branch_cycles_
            ? instructions[op].branch_cycles
            : instructions[op].cycles;
    }
    // CGB double speed mode, CPU clock ticks twice as fast
    cycles_ += (double_speed_) ? cycles_passed/2 : cycles_passed;
    use_branch_cycles_ = false;
    return;
}

Cpu_dump Processor::dump() const noexcept
{
    return {af_, bc_, de_, hl_, sp_, pc_, cycles_, ime_,
            {read(pc_), read(pc_+1), read(pc_+2)}};
}

}
