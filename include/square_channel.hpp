#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cstdint>
#include <array>

namespace qtboy
{

class Square_channel
{
    public:
    void tick(size_t cycles);
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    uint8_t output();
    void length_tick();
    void envelope_tick();
    void sweep_tick();

    private:
    uint16_t freq();
    uint8_t duty_pattern();
    uint16_t new_sweep_freq(); // sweep

    private:
    void restart_sound();
    uint8_t sweep_ {0x80}, length_ {0x3f}, envelope_ {0x00},
        freq_lo_ {}, freq_hi_ {0xbf};
    uint8_t volume_ {0}, output_ {0}; // current volume and final volume output
    uint8_t duty_ptr_ {0}; // current bit of the duty cycle
    uint16_t timer_ {freq()};
    uint16_t length_timer_ {0};
    uint16_t envelope_timer_ {0};
    uint16_t sweep_timer_ {0};
    uint16_t sweep_shadow_ {0};
    bool sweep_enable_ {false};
    bool envelope_running_ {false};
    bool enabled_ {false};
    static constexpr std::array<std::array<bool, 8>, 4> DUTY_PATTERNS
    {{
        {0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 0}

    }};
};

}
#endif // CHANNEL_HPP
