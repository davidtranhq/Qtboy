#ifndef APU_HPP
#define APU_HPP

#include <cstdint>
#include <bitset>
#include <vector>
#include <cstddef>

#include "square_channel.hpp"
#include "wave_channel.hpp"
#include "noise_channel.hpp"
#include "raw_audio.hpp"

namespace gameboy
{

class Speaker;

class Apu
{
    public:
    explicit Apu();

    void tick(std::size_t cycles);
    void queue_samples();
    int samples_queued();
    int samples_buffered();
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    void set_speaker(Speaker *s);
    void reset();
    void toggle_sound(bool b);

    public:

    static constexpr int SAMPLE_RATE {65536};
    static constexpr int SAMPLE_SIZE {1024};

    private:
    // downsample = clockfreq/samplingrate
    // 4.93MHz/44100
    static constexpr int DOWNSAMPLE_FREQ {4194304/SAMPLE_RATE};

    Speaker *speaker_ {nullptr};
    Square_channel square1_ {};
    Square_channel square2_ {};
    Noise_channel noise_ {};
    Wave_channel wave_ {};
    Raw_audio samples_;
    uint8_t volume_ {0x77}; // ff24
    uint8_t output_ {0xf3}; // ff25
    uint8_t enable_ {0xf1}; // ff26
    uint8_t downsample_cnt_ {DOWNSAMPLE_FREQ};
    int frame_sequence_cnt {8192};
    uint8_t frame_sequencer_ {0};


};

}

#endif // APU_HPP
