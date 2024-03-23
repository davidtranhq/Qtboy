#include "apu.hpp"
#include "system.hpp"
#include "speaker.hpp"

#include <vector>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <SDL2/SDL.h>
#include <chrono>

using namespace qtboy;


Apu::Apu()
    : samples_(SAMPLE_SIZE)
{}

void Apu::reset()
{
    square1_ = {};
    square2_ = {};
    wave_ = {};
    noise_ = {};
    samples_.clear();
    volume_ = 0x77;
    output_ = 0xf3;
    enable_ = 0xf1;
    downsample_cnt_ = DOWNSAMPLE_FREQ;
}

void Apu::tick(std::size_t cycles)
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
                    wave_.length_tick();
                    noise_.length_tick();
                    break;
                case 7:
                    square1_.envelope_tick();
                    square2_.envelope_tick();
                    noise_.envelope_tick();
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
        wave_.tick(1);
        noise_.tick(1);
        // take a sample only once ever DOWNSAMPLE_FREQ cycles
        if (--downsample_cnt_ <= 0)
        {
            downsample_cnt_ = DOWNSAMPLE_FREQ;
            uint16_t left_mix = 0, right_mix = 0;
            // generate empty samples if the speaker is disabled (leave left/right mix at 0)
            // (we still want samples to be pushed to the speaker because the CPU is synced to audio)
            if (speaker_->enabled())
            {
                // Generate a pair of samples, one for the left channel, one for the right
                std::pair<uint8_t, uint8_t> samples = mix_samples(square1_.output(),
                                                                  square2_.output(),
                                                                  wave_.output(),
                                                                  noise_.output());
                left_mix = samples.first;
                right_mix = samples.second;
            }
            // only push if there is less than 0.1 seconds of audio samples
            // (prevents excessive audio delay during turbo mode)
            if (samples_queued() < SAMPLE_RATE/10)
            {
                samples_.push(left_mix);
                samples_.push(right_mix);
            }
            if (samples_.size() >= SAMPLE_SIZE)
            {
                speaker_->queue_samples(std::move(samples_));
                samples_.clear();
            }
        }
    }
}

int Apu::samples_queued()
{
    return speaker_->samples_queued();
}

uint8_t Apu::read_reg(uint16_t adr)
{
    uint8_t b = 0xff;
    if (adr > 0xff09 && adr < 0xff15)
        b = square1_.read_reg(adr);
    else if (adr > 0xff15 && adr < 0xff1a)
        b = square2_.read_reg(adr);
    else if (adr > 0xff19 && adr < 0xff1f)
        b = wave_.read_reg(adr);
    else if (adr > 0xff1f && adr < 0xff24)
        b = noise_.read_reg(adr);
    else if (adr == 0xff24)
        b = volume_;
    else if (adr == 0xff25)
        b = output_;
    else if (adr == 0xff26)
        b = enable_;
    else if (adr > 0xff2f && adr < 0xff40)
        b = wave_.read_reg(adr);
    return b;
}

void Apu::write_reg(uint8_t b, uint16_t adr)
{
    if (adr > 0xff09 && adr < 0xff15)
       square1_.write_reg(b, adr);
    else if (adr > 0xff15 && adr < 0xff1a)
        square2_.write_reg(b, adr);
    else if (adr > 0xff19 && adr < 0xff1f)
        wave_.write_reg(b, adr);
    else if (adr > 0xff1f && adr < 0xff24)
        noise_.write_reg(b, adr);
    else if (adr == 0xff24)
        volume_ = b;
    else if (adr == 0xff25)
        output_ = b;
    else if (adr == 0xff26)
        enable_ = (b >> 7) ? 0xff : enable_; // only bit 7 is writable, sets all
    else if (adr > 0xff2f && adr < 0xff40)
        wave_.write_reg(b, adr);
}

void Apu::set_speaker(std::shared_ptr<Speaker> s)
{
    speaker_ = std::move(s);
}

void Apu::toggle_sound(bool b)
{
    speaker_->toggle(b);
}

std::pair<uint8_t, uint8_t> Apu::mix_samples(uint8_t square1,
                                        uint8_t square2,
                                        uint8_t wave,
                                        uint8_t noise)
{
    // bits 0-2 of NR50 (volume_) give the volume of the left channel (0-7)
    float left_volume = volume_ & 0x7;
    // bits 4-6 give volume of right channel (0-7)
    float right_volume = (volume_ >> 4) & 0x7;

    // NR51 (output_)
    // Bit 7 - Output sound 4 to SO2 terminal
    // Bit 6 - Output sound 3 to SO2 terminal
    // Bit 5 - Output sound 2 to SO2 terminal
    // Bit 4 - Output sound 1 to SO2 terminal
    // Bit 3 - Output sound 4 to SO1 terminal
    // Bit 2 - Output sound 3 to SO1 terminal
    // Bit 1 - Output sound 2 to SO1 terminal
    // Bit 0 - Output sound 1 to SO1 terminal

    std::bitset<8> output {output_};
    // determine which channels are enabled in the left/right speakers
    uint8_t left_square1 = output[0] ? square1 : 0,
            left_square2 = output[1] ? square2 : 0,
            left_wave = output[2] ? wave : 0,
            left_noise = output[3] ? noise : 0,
            right_square1 = output[4] ? square1 : 0,
            right_square2 = output[5] ? square2 : 0,
            right_wave = output[6] ? wave : 0,
            right_noise = output[7] ? wave : 0;
    // add channel outputs in corresponding speakers
    uint16_t left_sample = left_square1 + left_square2 + left_wave + left_noise;
    uint16_t right_sample = right_square1 + right_square2 + right_wave + right_noise;
    left_sample *= (left_volume/7);
    right_sample *= (right_volume/7);
    // normalize audio
    if (left_sample > 0xff)
        left_sample /= 4;
    if (right_sample > 0xff)
        right_sample /= 4;
    return {left_sample, right_sample};
}



