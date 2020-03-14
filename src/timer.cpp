#include "timer.hpp"
#include "processor.hpp"

using gameboy::Timer;

Timer::Timer(Processor &p)
    : cpu_ {p}
{}

void Timer::update(size_t cycles)
{
    size_t ticks = cycles*4;
    div_ += ticks;
    const uint16_t freq[] = {1024, 16, 64, 256};
    // bit 2 is enable bit
    // bit 0-1 determine update frequency
    if ((tac_ & 4) && (div_ % freq[tac_ & 3] == 0))
    {
        ++tima_;
        if (tima_ == 0) // overflow
            tima_overflow();
    }

}

void Timer::tima_overflow()
{
    tima_ = tma_;
    cpu_.request_interrupt(Processor::TIMER);
}

uint8_t Timer::read(uint16_t adr)
{
    uint8_t b {0xff};
    switch (adr)
    {
        case 0xff04: // DIV
            b = (div_ >> 8); // only upper 8-bits
            break;
        case 0xff05: // TIMA
            b = tima_;
            break;
        case 0xff06: // TMA
            b = tma_;
            break;
        case 0xff07: // TAC
            b = tac_;
            break;
    }
    return b;
}

void Timer::write(uint8_t b, uint16_t adr)
{
    switch (adr)
    {
        case 0xff04: // DIV
            div_ = 0;
            break;
        case 0xff05: // TIMA
            tima_ = b;
            break;
        case 0xff06: // TMA
            tma_ = b;
            break;
        case 0xff07: // TAC
            tac_ = b;
            break;
    }
}
