#include "speaker.hpp"

using namespace gameboy;

void Speaker::toggle(bool b)
{
    enabled_ = b;
}

bool Speaker::enabled() const noexcept
{
    return enabled_;
}
