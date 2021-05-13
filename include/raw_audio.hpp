#ifndef RAW_AUDIO_HPP
#define RAW_AUDIO_HPP

#include <vector>
#include <cstdint>

namespace qtboy
{

// PCM codec

class Raw_audio
{
    public:
    Raw_audio(std::size_t buffer_size);

    void push(uint8_t);
    void clear() { samples_.clear(); }
    std::size_t size() const;
    const uint8_t *data() const;

    private:
    std::vector<uint8_t> samples_ {};

};


}

#endif // RAW_AUDIO_HPP
