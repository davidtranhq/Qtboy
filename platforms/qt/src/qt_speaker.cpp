#include "qt_speaker.h"
#include "audio_types.hpp"

#include <QAudioOutput>
#include <QBuffer>
#include <QFile>
#include <QDebug>

Qt_speaker::Qt_speaker()
{
    QAudioFormat fmt;
    fmt.setSampleRate(44100);
    fmt.setChannelCount(1);
    fmt.setSampleSize(sizeof(uint8_t)*8);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::UnSignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(fmt)) {
        qWarning() << "raw audio format not supported by backend, cannot play audio.";
        return;
    }
    output_ = new QAudioOutput(fmt, nullptr);
    output_->setNotifyInterval(15);
    output_->setBufferSize(44100);
    connect(output_, SIGNAL(notify()), this, SLOT(output_notify()));
    connect(output_, SIGNAL(stateChanged(QAudio::State)),
            this, SLOT(output_state_changed(QAudio::State)));
    device_ = output_->start();

}

void Qt_speaker::push_samples(gameboy::Raw_audio<uint8_t> &a)
{
    device_->write(reinterpret_cast<const char *>(a.data()), a.size());
    a.reset();
}

void Qt_speaker::output_notify()
{}

void Qt_speaker::output_state_changed(const QAudio::State &)
{}
