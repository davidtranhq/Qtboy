#ifndef APU_HPP
#define APU_HPP

#include <cstdint>
#include <bitset>
#include <array>
#include <vector>

#include "square_channel.hpp"
#include "audio_types.hpp"

namespace gameboy
{

class Speaker;



class Apu
{
    public:
    void tick(size_t cycles);
    uint8_t read_reg(uint16_t adr);
    void write_reg(uint8_t b, uint16_t adr);
    void set_speaker(Speaker *s);
    void reset();

    private:
    // clockfreq/samplingrate
    // 4.93MHz/44100
    static constexpr uint8_t DOWNSAMPLE_FREQ {95};
    static constexpr size_t SAMPLE_SIZE {4096};

    Speaker *speaker_ {nullptr};
    Square_channel square1_ {};
    Square_channel square2_ {};
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
