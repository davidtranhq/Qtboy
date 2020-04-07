#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "audio_types.hpp"

namespace gameboy
{

class Speaker
{
    public:
    virtual void push_samples(Raw_audio<uint8_t> &a) = 0;
};

}

#endif // SPEAKER_HPP
