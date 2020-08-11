#include "raw_audio.hpp"

using namespace gameboy;

Raw_audio::Raw_audio(std::size_t buffer_size)
{
    samples_.reserve(buffer_size);
}

void Raw_audio::push(uint8_t s)
{
    samples_.push_back(s);
}

const uint8_t *Raw_audio::data() const
{
    return samples_.data();
}

std::size_t Raw_audio::size() const
{
    return samples_.size();
}
