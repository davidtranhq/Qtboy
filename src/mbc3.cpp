#include "memory_bank_controller.hpp"

#include "rom.hpp"
#include "ram.hpp"
#include "exception.hpp"

#include <ctime>

namespace gameboy
{

Mbc3::Mbc3(Rom *rom, std::optional<External_ram> *ram)
    : rom_ {rom}, ram_ {ram}
{
    // initialize RTC
    rtc_.base_time = std::time(nullptr);
    rtc_.base_regs = {};
    rtc_.latched_regs = {};
    rtc_.latched = false;

}

uint8_t Mbc3::read(uint16_t adr) const
{
    uint8_t b {0xff};
    if (adr < 0x4000) // Read from ROM bank 00
    {
        b = rom_->read(0, adr);
    }
    else if (adr < 0x8000) // Read from ROM bank 01-7f
    {
        b = rom_->read(rom_bank_, adr - 0x4000);
    }
    else if (adr < 0xa000) // nothing defined here
    {

    }
    else if (adr < 0xc000) // Read/Write RAM Bank 00-03 OR RTC register 08-0C
    {
        if (ram_ && ram_rtc_enable_)
        {
            if (is_rtc_) // RTC registers are mapped
            {
                // return latched registers if RTC is latched
                if (rtc_.latched)
                {
                    b = rtc_.latched_regs[mapped_rtc_];
                }
                // otherwise, update the registers and return the base
                else
                {
                    update_rtc();
                    b = rtc_.base_regs[mapped_rtc_];
                }
            }
            else // RAM Bank 00-03 is mapped
            {
                b = ram_->value().read(ram_bank_, adr - 0xa000);
            }
        }
        else
            b = 0xff;
    }
    return b;
}

void Mbc3::write(uint8_t b, uint16_t adr)
{
    if (adr < 0x2000) // RAM and timer enable
    {
        // 00h = disable RAM and RTC registers
        // 0ah == enable RAM and RTC registers
        ram_rtc_enable_ = ((b & 0xa) == 0xa);
    }
    else if (adr < 0x4000) // ROM bank number
    {
        // b = rom bank, except when b == 0, then bank 1 is selected
        rom_bank_ = (b == 0) ? 1 : b;
        rom_bank_ &= 0x7f;
    }
    else if (adr < 0x6000) // RAM Bank Number or RTC register select
    {
        // writing a value between 08h-0Ch maps a coressponding RTC register
        // to a000-bfff
        if (b >= 0x8 && b <= 0xc)
        {
            mapped_rtc_ = b-0x8;
            is_rtc_ = true;
        }
        // otherwise it selects the RAM bank 00-03
        else if (b <= 0x3)
        {
            ram_bank_ = (b & 3); // only banks 00-03 are addressable
            is_rtc_ = false;
        }
    }
    else if (adr < 0x8000) // latch current time into RTC registers
    {
        // latch time into RTC registers, latch happens when 0->1
        if (b == 0 && rtc_.latched)
        {
            rtc_.latched = false;
        }
        else if (b == 1 && !rtc_.latched)
        {
            update_rtc();
            // copy base registers into latched registers
            for (uint8_t i = 0; i < 5; ++i)
                rtc_.latched_regs[i] = rtc_.base_regs[i];
            rtc_.latched = true;
        }
    }
    else if (adr < 0xa000)
    {
        // undefined
    }
    else if (adr < 0xc000) // Read/Write RAM Bank 00-03 OR RTC register 08-0C
    {
        if (ram_ && ram_rtc_enable_)
        {
            if (is_rtc_) // RTC registers are mapped
            {
                rtc_.base_regs[mapped_rtc_];
            }
            else // RAM Bank 00-07 is mapped
            {
                ram_->value().write(b, ram_bank_, adr - 0xa000);
            }
        }
        else
            b = 0xff;
    }
}

/*** RTC SAVE FORMAT
offset  size    desc
0       4       time seconds
4       4       time minutes
8       4       time hours
12      4       time days
16      4       time days high
20      4       latched time seconds
24      4       latched time minutes
28      4       latched time hours
32      4       latched time days
36      4       latched time days high
40      4       unix timestamp when saving
44      4       0   (probably the high dword of 64 bits time),
                absent in the 44 bytes version
***/

void Mbc3::load_sram(const std::vector<uint8_t> &sram)
{
    // if sram is > external ram size, RTC ram is included
    if (sram.size() > ram_->value().size())
    {
        std::vector<uint8_t> rtc_ram(sram.end()-48, sram.end());
        for (uint8_t i = 0; i < 5; ++i)
        {
            rtc_.base_regs[i] = rtc_ram[i*4];
            rtc_.latched_regs[i] = rtc_ram[i*4+20];
        }
        for (uint8_t i = 0; i < sizeof(std::time_t) && i < 8; ++i)
        {
            rtc_.base_time <<= 8;
            rtc_.base_time |= rtc_ram[47-i];
        }
        rtc_.latched = false;

    }
    ram_->value().load(sram);
}


std::vector<uint8_t> Mbc3::dump_ram() const
{
    std::vector<uint8_t> sram(ram_->value().dump());
    std::time_t now = std::time(nullptr);
    uint32_t now_lo = now & 0xffffffff;
    // if std::time_t isn't 64-bit, now_hi will be zero
    uint32_t now_hi = sizeof(now) > 4 ? now >> 32 : 0;
    std::vector<uint8_t> rtc_ram
    {
        rtc_.base_regs[0], 0, 0, 0,
        rtc_.base_regs[1], 0, 0, 0,
        rtc_.base_regs[2], 0, 0, 0,
        rtc_.base_regs[3], 0, 0, 0,
        rtc_.base_regs[4], 0, 0, 0,
        rtc_.latched_regs[0], 0, 0, 0,
        rtc_.latched_regs[1], 0, 0, 0,
        rtc_.latched_regs[2], 0, 0, 0,
        rtc_.latched_regs[3], 0, 0, 0,
        rtc_.latched_regs[4], 0, 0, 0,
        // append unix timestamp of current time, little-endian
        static_cast<uint8_t>(now_lo & 0xff),
        static_cast<uint8_t>(now_lo >> 8 & 0xff),
        static_cast<uint8_t>(now_lo >> 16 & 0xff),
        static_cast<uint8_t>(now_lo >> 24 & 0xff),
        static_cast<uint8_t>(now_hi & 0xff),
        static_cast<uint8_t>(now_hi >> 8 & 0xff),
        static_cast<uint8_t>(now_hi >> 16 & 0xff),
        static_cast<uint8_t>(now_hi >> 24 & 0xff),
    };
    // append rtc_ram to the end of sram
    sram.insert(sram.end(), rtc_ram.begin(), rtc_ram.end());
    return sram;
}

void Mbc3::load_rtc(const std::array<uint8_t, 5> &base)
{
    rtc_.base_regs = base;
}

void Mbc3::update_rtc() const
{
    // get current time
    std::time_t now = std::time(nullptr);
    std::time_t new_time = 0;
    // update time if RTC is enabled
    if ((rtc_.base_regs[4] & 0x40) == 0 && now > rtc_.base_time)
        new_time = now - rtc_.base_time;
    new_time += rtc_.base_regs[0]; // sec
    new_time += rtc_.base_regs[1] * 60; // min
    new_time += rtc_.base_regs[2] * 60 * 60; // hr
    new_time += rtc_.base_regs[3] * 60 * 60 * 24; // day
    new_time += (rtc_.base_regs[4] & 1) * 60 * 60 * 24 * 256; // day hi bit
    // 0-4 is sec, min, hr, lo 8-bits of day, hi-bit of day, halt, carry
    rtc_.base_regs[0] = new_time % 60;
    new_time /= 60;
    rtc_.base_regs[1] = new_time % 60;
    new_time /= 60;
    rtc_.base_regs[2] = new_time % 24;
    new_time /= 24;
    rtc_.base_regs[3] = new_time % 0xff;
    new_time /= 0xff;
    rtc_.base_regs[4] = (rtc_.base_regs[4] & 0xfe) | (new_time % 2);
    new_time /= 2;
    rtc_.base_regs[4] |= (new_time > 0 ? 0x80 : 0);
    // update base time in RTC
    rtc_.base_time = now;
}



}
