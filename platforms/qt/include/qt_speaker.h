#ifndef QT_SPEAKER_H
#define QT_SPEAKER_H

#include "raw_audio.hpp"
#include "speaker.hpp"

#include <QObject>
#include <SDL_audio.h>

class Qt_speaker : public QObject, public gameboy::Speaker
{
    Q_OBJECT

    public:
    explicit Qt_speaker();

    void queue_samples(const gameboy::Raw_audio &a) override;
    int samples_queued() override;

    private:
    int buffer_size_ {};
    SDL_AudioDeviceID device_id_;
};

#endif // QT_SPEAKER_


