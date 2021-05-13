#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "raw_audio.hpp"

namespace qtboy
{

class Speaker
{
    public:
    // Queue raw PCM samples in the audio buffer.
    virtual void queue_samples(const Raw_audio &a) = 0;

    // Get how many samples are queued in the audio buffer.
    virtual int samples_queued() = 0;

    // Clear the audio buffer.
    virtual void clear_samples() = 0;

    void toggle(bool);
    bool enabled() const noexcept;

    private:
    bool enabled_ {true};
};

}

#endif // SPEAKER_HPP
