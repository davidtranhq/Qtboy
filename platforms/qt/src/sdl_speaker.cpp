#include "sdl_speaker.h"


void SDL_speaker::push_samples(gameboy::Raw_audio<float> s)
{/*
    SDL_AudioSpec desired, obtained;
    desired.format = AUDIO_F32SYS;
    desired.freq = s.rate();
    desired.channels = s.channels();
    desired.samples = static_cast<uint16_t>(s.sample_size());
    desired.callback = nullptr;
    desired.userdata = nullptr;
    SDL_OpenAudio(&desired, &obtained);
    SDL_QueueAudio(1, s.data(), s.sample_size()*sizeof(float));
*/}
