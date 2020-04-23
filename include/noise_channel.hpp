#ifndef NOISE_CHANNEL_HPP
#define NOISE_CHANNEL_HPP

#include <cstdint>
#include <array>

namespace gameboy
{

class Noise_channel
{
    public:
    void tick(size_t cycles);
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    uint8_t output();
    void length_tick();
    void envelope_tick();

    private:
    void restart_sound();
    uint8_t length_ {0x3f}, envelope_ {0x00},
        poly_ {}, counter_ {0xbf};
    uint8_t volume_ {0}, output_ {0}; // current volume and final volume output
    uint16_t timer_ {0};
    uint16_t length_timer_ {0};
    uint16_t envelope_timer_ {0};
    bool envelope_running_ {false};
    bool enabled_ {false};
    uint16_t lfsr_ {0};

    static constexpr std::array<uint8_t, 8> divisors
        {{8, 16, 32, 48, 64, 80, 96, 112}};
};

}
#endif // NOISE_CHANNEL_HPP
