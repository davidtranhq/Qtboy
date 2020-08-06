#ifndef APU_HPP
#define APU_HPP

#include <cstdint>
#include <bitset>
#include <array>
#include <vector>
#include <cstddef>

#include "square_channel.hpp"
#include "wave_channel.hpp"
#include "noise_channel.hpp"
#include "audio_types.hpp"

namespace gameboy
{

class Speaker;



class Apu
{
    public:
    void tick(std::size_t cycles);
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    void set_speaker(Speaker *s);
    void reset();
    void toggle_sound(bool b);

    private:
    // clockfreq/samplingrate
    // 4.93MHz/44100
    static constexpr uint8_t DOWNSAMPLE_FREQ {95};
    static constexpr std::size_t SAMPLE_SIZE {2048};

    Speaker *speaker_ {nullptr};
    Square_channel square1_ {};
    Square_channel square2_ {};
    Noise_channel noise_ {};
    Wave_channel wave_ {};
    Raw_audio<uint8_t> audio_ {SAMPLE_SIZE};
    uint8_t volume_ {0x77}; // ff24
    uint8_t output_ {0xf3}; // ff25
    uint8_t enable_ {0xf1}; // ff26
    uint8_t downsample_cnt_ {DOWNSAMPLE_FREQ};
    int frame_sequence_cnt {8192};
    uint8_t frame_sequencer_ {0};


};

}

#endif // APU_HPP
