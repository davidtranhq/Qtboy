#ifndef QT_SPEAKER_H
#define QT_SPEAKER_H

#include "raw_audio.hpp"
#include "speaker.hpp"

#include <QObject>
#include <QAudioOutput>


class QAudioOutput;
class QIODevice;

class Qt_speaker : public QObject, public gameboy::Speaker
{
    Q_OBJECT

    public:
    explicit Qt_speaker();

    void push_samples(const gameboy::Raw_audio &a) override;
    int samples_queued() override;

    private slots:
    void output_state_changed(const QAudio::State &);

    private:
    int buffer_size_ {};
    QAudioOutput *output_ {nullptr};
    QIODevice *device_ {nullptr};
};

#endif // QT_SPEAKER_H
