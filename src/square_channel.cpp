#include "square_channel.hpp"
#include <cstdint>

using namespace gameboy;

void Square_channel::tick(size_t cycles)
{
    timer_ -= cycles;
    if (timer_ <= 0)
    {
        timer_ = freq() + timer_; // reload timer when it hits zero
        duty_ptr_ = (duty_ptr_ + 1) & 7;
    }
    // check if enabled and DAC enabled
    if (enabled_ && (ff17_ & 0xf8) != 0)
        output_ = volume_;
    else
        output_ = 0;
    if (!DUTY_PATTERNS[duty_pattern()][duty_ptr_])
        output_ = 0;
}

uint8_t Square_channel::read_reg(uint16_t adr)
{
    uint8_t b = 0xff;
    switch (adr)
    {
        case 0xff16: b = ff16_; break;
        case 0xff17: b = ff17_; break;
        case 0xff18: b = ff18_; break;
        case 0xff19: b = ff19_; break;
    }
    return b;
}

void Square_channel::write_reg(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff16: ff16_ = b; break;
        case 0xff17: ff17_ = b; break;
        case 0xff18: ff18_ = b; break;
        case 0xff19:
        {
            ff19_ = b;
            if (b & 1 << 7)
                restart_sound();
        } break;
    }
}

void Square_channel::length_tick()
{
    bool length_enable = ff19_ & (1 << 6);
    if (length_timer_ > 0 && length_enable)
    {
        --length_timer_;
        if (length_timer_ == 0)
            enabled_ = false;
    }
}

void Square_channel::envelope_tick()
{
    if (--envelope_timer_ <= 0)
    {
        // reload
        uint8_t env_initial = ff17_ & 0x7;
        envelope_timer_ = env_initial;
        if (envelope_timer_ == 0)
            envelope_timer_ = 8;
        if (env_initial > 0 && envelope_running_)
        {
            if (envelope_add_ && volume_ < 15)
                ++volume_;
            else if (!envelope_add_ && volume_ > 0)
                --volume_;
        }
        if (volume_ == 0 || volume_ == 15)
           envelope_running_ = false;
    }
}

uint8_t Square_channel::output()
{
    return output_;
}

uint16_t Square_channel::freq()
{
    uint16_t f = static_cast<uint16_t>((ff19_ & 7) << 8 | ff18_);
    return (2048-f)*4;
}

uint8_t Square_channel::duty_pattern()
{
    return ff16_ >> 6;
}

void Square_channel::restart_sound()
{
    enabled_ = true;
    if (length_timer_ == 0)
        length_timer_ = 64;
    timer_ = freq(); // reload timer
    envelope_running_ = true;
    uint8_t env_initial = ff17_ & 0x7;
    envelope_timer_ = env_initial;
    uint8_t initial_volume = ff17_ >> 4 & 0xf;
    volume_ = initial_volume;
}
