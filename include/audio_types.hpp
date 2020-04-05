#ifndef AUDIO_TYPES_HPP
#define AUDIO_TYPES_HPP

#include <vector>
#include <cstdint>

namespace gameboy
{

// PCM codec

template <typename T>
class Raw_audio
{
    public:
    using sample_type = T;
    Raw_audio(size_t buffer_size);

    void push(sample_type);
    void reset() { samples_.clear(); }
    size_t size() const;
    sample_type *data();

    private:
    std::vector<sample_type> samples_ {};

};

template <typename T>
Raw_audio<T>::Raw_audio(size_t buffer_size)
{
    samples_.reserve(buffer_size+1); // +1 to prevent unecessary resizing
                                     // when reaching capacity
}

template <typename T>
void Raw_audio<T>::push(sample_type s)
{
    samples_.push_back(s);
}

template <typename T>
T *Raw_audio<T>::data()
{
    return samples_.data();
}

template <typename T>
size_t Raw_audio<T>::size() const
{
    return samples_.size();
}

}

#endif // AUDIO_TYPES_HPP
