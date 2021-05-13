#include "noise_channel.hpp"
#include <cstdint>

using namespace qtboy;

uint8_t Noise_channel::read_reg(uint16_t adr)
{
    uint8_t b = 0xff;
    switch (adr)
    {
        case 0xff20: b = length_; break;
        case 0xff21: b = envelope_; break;
        case 0xff22: b = poly_; break;
        case 0xff23: b = counter_; break;
    }
    return b;
}

void Noise_channel::write_reg(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff20: length_ = b; break;
        case 0xff21: envelope_ = b; break;
        case 0xff22: poly_ = b; break;
        case 0xff23:
        {
            counter_ = b;
            if (b & 1 << 7)
                restart_sound();
        } break;
    }
}

void Noise_channel::tick(size_t cycles)
{
    timer_ -= cycles;
    {
        uint8_t d = poly_ & 0x7;
        uint8_t shift = poly_ >> 4 & 0xf;
        bool width = poly_ & 0x8;
        timer_ = divisors[d] << shift;
        uint8_t res = (lfsr_ & 0x1) ^ ((lfsr_ >> 1) & 0x1);
        lfsr_ >>= 1;
        lfsr_ |= res << 14;
        if (width)
        {
            lfsr_ &= ~0x40;
            lfsr_ |= res << 6;
        }
        bool dac = (envelope_ & 0xf8) != 0;
        if (enabled_ && dac && (lfsr_ & 0x1) == 0)
            output_ = volume_;
        else
            output_ = 0;
    }
}


uint8_t Noise_channel::output()
{
    return output_;
}


void Noise_channel::length_tick()
{
    bool length_enable = counter_ & (1 << 6);
    if (length_timer_ > 0 && length_enable)
    {
        --length_timer_;
        if (length_timer_ == 0)
            enabled_ = false;
    }
}

void Noise_channel::envelope_tick()
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

void Noise_channel::restart_sound()
{
    enabled_ = true;
    if (length_timer_ == 0)
        length_timer_ = 64;
    uint8_t d = poly_ & 0x7;
    uint8_t shift = poly_ >> 4 & 0xf;
    timer_ = divisors[d] << shift;
    envelope_running_ = true;
    uint8_t env_initial = envelope_ & 0x7;
    envelope_timer_ = env_initial;
    uint8_t initial_volume = envelope_ >> 4 & 0xf;
    volume_ = initial_volume;
    lfsr_ = 0x7fff;
}
