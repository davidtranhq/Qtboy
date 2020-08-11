#ifndef SPEAKER_HPP
#define SPEAKER_HPP

#include "raw_audio.hpp"

namespace gameboy
{

class Speaker
{
    public:
    virtual void push_samples(const Raw_audio &a) = 0;
    virtual int samples_queued() = 0;
    void toggle(bool);
    bool enabled() const noexcept;

    private:
    bool enabled_ {true};
};

}

#endif // SPEAKER_HPP
