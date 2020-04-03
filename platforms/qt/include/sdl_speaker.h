#ifndef SDL_SPEAKER_H
#define SDL_SPEAKER_H

#include "audio_types.hpp"
#include "speaker.hpp"

class SDL_speaker : public gameboy::Speaker
{
    public:
    void push_samples(gameboy::Raw_audio<float> a) override;
};

#endif // QT_SPEAKER_H
