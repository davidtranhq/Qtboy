#include "square_channel.hpp"
#include <cstdint>

using namespace qtboy;

void Square_channel::tick(size_t cycles)
{
    timer_ -= cycles;
    if (timer_ <= 0)
    {
        timer_ = freq() + timer_; // reload timer when it hits zero
        duty_ptr_ = (duty_ptr_ + 1) & 7;
    }
    // check if enabled and DAC enabled
    if (enabled_ && (envelope_ & 0xf8) != 0)
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
        case 0xff10: b = sweep_; break;
        case 0xff11: case 0xff16: b = length_; break;
        case 0xff12: case 0xff17: b = envelope_; break;
        case 0xff13: case 0xff18: b = freq_lo_; break;
        case 0xff14: case 0xff19: b = freq_hi_; break;
    }
    return b;
}

void Square_channel::write_reg(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff10: sweep_ = b; break;
        case 0xff11: case 0xff16: length_ = b; break;
        case 0xff12: case 0xff17: envelope_ = b; break;
        case 0xff13: case 0xff18: freq_lo_ = b; break;
        case 0xff14: case 0xff19:
        {
            freq_hi_ = b;
            if (b & 1 << 7)
                restart_sound();
        } break;
    }
}


uint8_t Square_channel::output()
{
    return output_;
}


void Square_channel::length_tick()
{
    bool length_enable = freq_hi_ & (1 << 6);
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
        uint8_t env_initial = envelope_ & 0x7;
        envelope_timer_ = env_initial;
        if (envelope_timer_ == 0)
            envelope_timer_ = 8;
        bool envelope_add = envelope_ & 1 << 3;
        if (env_initial > 0 && envelope_running_)
        {
            if (envelope_add && volume_ < 15)
                ++volume_;
            else if (!envelope_add && volume_ > 0)
                --volume_;
        }
        if (volume_ == 0 || volume_ == 15)
           envelope_running_ = false;
    }
}

void Square_channel::sweep_tick()
{
    if (--sweep_timer_ <= 0)
    {
        // reload
        uint8_t sweep_initial = sweep_ >> 4 & 7;
        sweep_timer_ = sweep_initial;
        if (sweep_timer_ == 0)
            sweep_timer_ = 8;
        if (sweep_enable_ && sweep_initial > 0)
        {
            uint16_t new_freq = new_sweep_freq();
            uint8_t sweep_shift = sweep_ & 7;
            if (new_freq < 2048 && sweep_shift > 0)
            {
                sweep_shadow_ = new_freq;
                freq_lo_ = new_freq & 0xff;
                freq_hi_ = (freq_hi_ & ~7) | ((new_freq >> 8) & 7);
                new_sweep_freq();
            }
            new_sweep_freq();
        }
    }
}

uint16_t Square_channel::freq()
{
    uint16_t f = static_cast<uint16_t>((freq_hi_ & 7) << 8 | freq_lo_);
    return (2048-f)*4;
}

uint8_t Square_channel::duty_pattern()
{
    return length_ >> 6;
}

uint16_t Square_channel::new_sweep_freq()
{
    uint16_t new_freq {0};
    uint8_t sweep_shift = sweep_ & 7;
    new_freq = sweep_shadow_ >> sweep_shift;
    if (sweep_ & 8) // sweep negate
        new_freq = sweep_shadow_ - new_freq;
    else
        new_freq = sweep_shadow_ + new_freq;
    if (new_freq > 2047)
        enabled_ = false;
    return new_freq;
}

void Square_channel::restart_sound()
{
    enabled_ = true;
    if (length_timer_ == 0)
        length_timer_ = 64;
    timer_ = freq(); // reload timer
    envelope_running_ = true;
    uint8_t env_initial = envelope_ & 0x7;
    envelope_timer_ = env_initial;
    uint8_t initial_volume = envelope_ >> 4 & 0xf;
    volume_ = initial_volume;
    sweep_shadow_ = static_cast<uint16_t>((freq_hi_ & 7) << 8 | freq_lo_);
    uint8_t sweep_initial = sweep_ >> 4 & 7;
    sweep_timer_ = sweep_initial;
    uint8_t sweep_shift = sweep_ & 7;
    if (sweep_timer_ == 0)
        sweep_timer_ = 8;
    sweep_enable_ = sweep_timer_ > 0 || sweep_shift > 0;
    if (sweep_shift > 0)
        // check for overflow
        new_sweep_freq();
}
