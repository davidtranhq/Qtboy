#include "memory_bank_controller.hpp"

#include "rom.hpp"
#include "ram.hpp"
#include "exception.hpp"

namespace gameboy
{

Mbc3::Mbc3(Rom *rom, External_ram *ram)
    : rom_ {rom}, ram_ {ram}
{}

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
    else if (adr < 0xc000) // Read/Write RAM Bank 00-03 OR RTC register 08-0C
    {
        if (ram_ && ram_rtc_enable_)
            b = ram_->read(ram_bank_, adr - 0xc000);
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
    }
    else if (adr < 0x6000) // RAM Bank Number or RTC register select
    {
        if (!ram_rtc_enable_)
            return;
        // writing a value between 08h-0Ch maps a coressponding RTC register
        // to a000-bfff
        if (b >= 0x8 && b <= 0xc)
        {
            mapped_rtc_ = b-0x8;
            is_rtc_ = true;
        }
        // otherwise it selects the RAM bank
        else
        {
            ram_bank_ = (b & 7); // only banks 00-07 are addressable
            is_rtc_ = false;
        }
    }
    else if (adr < 0x8000) // latch current time into RTC registers
    {

    }
}

}
