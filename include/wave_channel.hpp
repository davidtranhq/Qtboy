#ifndef WAVE_CHANNEL_HPP
#define WAVE_CHANNEL_HPP

#include <cstdint>
#include <array>

namespace qtboy
{

class Wave_channel
{
    public:
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    void tick(size_t cycles);
    uint8_t output();
    void length_tick();

    private:
    uint16_t freq();
    void restart_sound();

    uint8_t enable_ {0x7f}; // ff1a
    uint8_t length_ {0xff}; // ff1b
    uint8_t output_level_ {0x9f}; // ff1c
    uint8_t freq_lo_ {0xbf}; // ff1d
    uint8_t freq_hi_ {0}; // ff1e
    std::array<uint8_t, 0x10> pattern_;
    uint8_t pattern_index_ {0};

    uint16_t timer_ {0};
    uint16_t length_timer_ {0};
    bool enabled_ {false};
    uint8_t output_ {0};
};

}


#endif // WAVE_CHANNEL_HPP
