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
    explicit Raw_audio(int sample_size = 2048,
                       int rate = 48000,
                       uint8_t channels = 2);

    void push(sample_type);
    void reset() { samples_.clear(); }
    size_t size() const;
    sample_type *data();
    inline int sample_size() const noexcept { return sample_size_; }
    inline int rate() const noexcept { return rate_; }
    inline uint8_t channels() const noexcept { return channels_; }

    private:
    std::vector<sample_type> samples_ {};
    int sample_size_;
    int rate_;
    int channels_;

};

template <typename T>
Raw_audio<T>::Raw_audio(int sample_size, int rate, uint8_t channels)
    : sample_size_ {sample_size},
      rate_ {rate},
      channels_ {channels}
{}

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
