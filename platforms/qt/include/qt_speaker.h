#ifndef QT_SPEAKER_H
#define QT_SPEAKER_H

#include "audio_types.hpp"
#include "speaker.hpp"

#include <QObject>

class QAudioOutput;
class QIODevice;

class Qt_speaker : public QObject, public gameboy::Speaker
{
    Q_OBJECT

    public:
    explicit Qt_speaker();

    void push_samples(gameboy::Raw_audio<uint8_t> a) override;

    private:
    QAudioOutput *output_ {nullptr};
    QIODevice *device_ {nullptr};
};

#endif // QT_SPEAKER_H