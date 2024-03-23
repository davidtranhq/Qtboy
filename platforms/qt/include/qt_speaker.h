#ifndef QT_SPEAKER_H
#define QT_SPEAKER_H

#include "raw_audio.hpp"
#include "speaker.hpp"

#include <QObject>
#include <SDL2/SDL_audio.h>

class Qt_speaker : public QObject, public qtboy::Speaker
{
    Q_OBJECT

    public:
    explicit Qt_speaker(QObject *parent = nullptr);
    virtual ~Qt_speaker();

    void queue_samples(const qtboy::Raw_audio &a) override;
    int samples_queued() override;
    void clear_samples() override;

    private:
    int buffer_size_ {};
    SDL_AudioDeviceID device_id_;
};

#endif // QT_SPEAKER_


