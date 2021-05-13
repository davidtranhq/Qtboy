#ifndef APU_HPP
#define APU_HPP

#include <cstdint>
#include <bitset>
#include <vector>
#include <cstddef>
#include <memory>
#include <utility> // std::pair

#include "square_channel.hpp"
#include "wave_channel.hpp"
#include "noise_channel.hpp"
#include "raw_audio.hpp"
#include "speaker.hpp"

namespace qtboy
{

class Speaker;

class Apu
{
    public:
    explicit Apu();

    void tick(std::size_t cycles);
    int samples_queued();
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);

    // Set the speaker used for audio output.
    void set_speaker(std::shared_ptr<Speaker> s);
    void reset();

    // When disabled, all samples are reduced to 0 before being pushed to speaker.
    void toggle_sound(bool b);

    public:

    static constexpr int SAMPLE_RATE {65536};
    static constexpr int SAMPLE_SIZE {1024};

    private:
    // Mix the samples from the 4 audio channels to generate a left/right sample pair.
    std::pair<uint8_t, uint8_t> mix_samples(uint8_t square1,
                                            uint8_t square2,
                                            uint8_t wave,
                                            uint8_t noise);
    // downsample = clockfreq/samplingrate
    // 4.93MHz/44100
    static constexpr int DOWNSAMPLE_FREQ {4194304/SAMPLE_RATE};

    std::shared_ptr<Speaker> speaker_ {nullptr};
    Square_channel square1_ {};
    Square_channel square2_ {};
    Wave_channel wave_ {};
    Noise_channel noise_ {};
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
