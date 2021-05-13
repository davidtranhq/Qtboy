#include "qt_speaker.h"
#include "raw_audio.hpp"
#include <iostream>

#include <QAudioOutput>
#include <QBuffer>
#include <QTimer>
#include <QDebug>

#include <SDL_audio.h>

Qt_speaker::Qt_speaker(QObject *parent)
    : QObject {parent}
{
    SDL_AudioSpec desired_spec, actual_spec;

    SDL_zero(desired_spec);
    desired_spec.freq = 65536;
    desired_spec.format = AUDIO_U8;
    desired_spec.channels = 2;
    desired_spec.samples = 1024;
    desired_spec.callback = nullptr;
    // allow changing the sample rate for turbo mode
    device_id_ = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &actual_spec, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);

    if (device_id_ == 0)
    {
        qWarning() << "Could not initialize audio device: " << SDL_GetError();
    }

    // Start the audio queue.
    SDL_PauseAudioDevice(device_id_, 0);
}

Qt_speaker::~Qt_speaker()
{
    SDL_CloseAudioDevice(device_id_);
}

void Qt_speaker::queue_samples(const qtboy::Raw_audio &a)
{
    // device_->write(reinterpret_cast<const char *>(a.data()), a.size());
    SDL_QueueAudio(device_id_, a.data(), a.size());
}

int Qt_speaker::samples_queued()
{
    return SDL_GetQueuedAudioSize(device_id_);
}

void Qt_speaker::clear_samples()
{

}
