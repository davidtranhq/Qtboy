#include "wave_channel.hpp"

using namespace gameboy;

uint8_t Wave_channel::read_reg(uint16_t adr)
{
    uint8_t b {0xff};
    switch (adr)
    {
        case 0xff1a: b = enable_; break;
        case 0xff1b: b = length_; break;
        case 0xff1c: b = output_level_; break;
        case 0xff1d: b = freq_lo_; break;
        case 0xff1e: b = freq_hi_; break;
        default:
        {
            if (adr > 0xff2a && adr < 0xff40)
                b = pattern_[adr-0xff30];
        } break;
    }
    return b;
}

void Wave_channel::write_reg(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff1a: enable_ = b; break;
        case 0xff1b: length_ = b; break;
        case 0xff1c: output_level_ = b; break;
        case 0xff1d: freq_lo_ = b; break;
        case 0xff1e:
        {
            freq_hi_ = b;
            if (b & 0x80)
                restart_sound();
        } break;
        default:
        {
            if (adr > 0xff2a && adr < 0xff40)
                pattern_[adr-0xff30] = b;
        } break;
    }
}

void Wave_channel::tick(size_t cycles)
{
    timer_ -= cycles;
    if (timer_ <= 0) {
        timer_ = freq();
        pattern_index_ = (pattern_index_ + 1) & 0x1F;
        if (enabled_ && enable_ & 0x80)
        {
            uint8_t pos = pattern_index_ / 2;
            uint8_t wave_byte = pattern_[pos];
            bool hi_bit = (pattern_index_ & 0x1) == 0;
            if (hi_bit)
                wave_byte >>= 4;
            wave_byte &= 0xF;
            uint8_t vol = output_level_ >> 5 & 0x3;
            if (vol > 0)
                wave_byte >>= vol - 1;
            else
                wave_byte = 0;
            output_ = wave_byte;
        }
        else
        {
            output_ = 0;
        }
    }
}

uint16_t Wave_channel::freq()
{
    uint16_t f = static_cast<uint16_t>((freq_hi_ & 7) << 8 | freq_lo_);
    return (2048-f)*2;
}

void Wave_channel::length_tick()
{
    if (length_timer_ > 0 && (freq_hi_ & 1 << 6))
    {
        --length_timer_;
        if (length_timer_ == 0)
            enabled_ = true;
    }
}

uint8_t Wave_channel::output()
{
    return output_;
}

void Wave_channel::restart_sound()
{
    enabled_ = true;
    if (length_timer_ == 0)
        length_timer_ = 256;
    timer_ = freq();
    pattern_index_ = 0;
}
