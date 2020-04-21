#include "apu.hpp"
#include "system.hpp"
#include "speaker.hpp"

#include <vector>

using namespace gameboy;

void Apu::reset()
{
    square1_ = {};
    square2_ = {};
    audio_ = Raw_audio<uint8_t>(SAMPLE_SIZE);
    volume_ = 0x77;
    output_ = 0xf3;
    enable_ = 0xf1;
    downsample_cnt_ = DOWNSAMPLE_FREQ;
}

void Apu::tick(size_t cycles)
{
    while (cycles-- > 0)
    {
        if (--frame_sequence_cnt <= 0)
        {
            frame_sequence_cnt = 8192;
            switch (frame_sequencer_)
            {
                case 2:
                case 6:
                    square1_.sweep_tick();
                case 0:
                case 4:
                    square1_.length_tick();
                    square2_.length_tick();
                    break;
                case 7:
                    square1_.envelope_tick();
                    square2_.envelope_tick();
                    break;
            }
            ++frame_sequencer_;
            if (frame_sequencer_ >= 8)
            {
                frame_sequencer_ = 0;
            }
        }
        square1_.tick(1);
        square2_.tick(1);
        // take a sample only once ever DOWNSAMPLE_FREQ cycles
        if (--downsample_cnt_<= 0)
        {
            downsample_cnt_ = DOWNSAMPLE_FREQ;
            uint16_t mix = square1_.output() + square2_.output();
            audio_.push(mix/2);
            if (audio_.size() >= SAMPLE_SIZE)
                speaker_->push_samples(audio_);
        }
    }
}

uint8_t Apu::read_reg(uint16_t adr)
{
    uint8_t b = 0xff;
    if (adr > 0xff09 && adr < 0xff15)
        b = square1_.read_reg(adr);
    else if (adr > 0xff15 && adr < 0xff1a)
        b = square2_.read_reg(adr);
    else if (adr == 0xff24)
        b = volume_;
    else if (adr == 0xff25)
        b = output_;
    else if (adr == 0xff26)
        b = enable_;
    return b;
}

void Apu::write_reg(uint8_t b, uint16_t adr)
{
    if (adr > 0xff09 && adr < 0xff15)
       square1_.write_reg(b, adr);
    else if (adr > 0xff15 && adr < 0xff1a)
        square2_.write_reg(b, adr);
    else if (adr == 0xff24)
        volume_ = b;
    else if (adr == 0xff25)
        output_ = b;
    else if (adr == 0xff26)
        enable_ = (b >> 7) ? 0xff : enable_; // only bit 7 is writable, sets all
}

void Apu::set_speaker(Speaker *s)
{
    speaker_ = s;
}

