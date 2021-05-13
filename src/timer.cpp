#include "timer.hpp"
#include "processor.hpp"

using qtboy::Timer;

Timer::Timer(Processor &p)
    : cpu_ {p}
{}

void Timer::update(std::size_t cycles)
{
    div_ticks_ += cycles;
    if (div_ticks_ >= 0xff)
    {
        div_ticks_ -= 0xff;
        ++div_;
    }

    // bit 2 is enable bit
    // bit 0-1 determine update frequency
    if (tac_ & 4)
    {
        tima_ticks_ += cycles;
        int freq = FREQUENCIES[tac_ & 3];
        while (tima_ticks_ >= freq)
        {
            tima_ticks_ -= freq;
            ++tima_;
            if (tima_ == 0)
            {
                tima_overflow();
            }
        }
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

void Timer::reset()
{
    div_ticks_ = 0;
    tima_ticks_ = 0;
    tima_ = 0;
    tma_ = 0;
    tac_ = 0;
    div_ = 0;
}
