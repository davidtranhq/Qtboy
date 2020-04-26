#pragma once

#include <cstdint>
#include <functional>

namespace gameboy
{

class Processor;

class Timer
{
    public:
    Timer(Processor &p);

    void update(size_t cycles);
    uint8_t read(uint16_t adr);
    void write(uint8_t b, uint16_t adr);
    void reset();

    private:
    void tima_overflow();

    Processor &cpu_;
    int ticks_ {0};
    uint8_t tima_ {0}, tma_ {0}; // TIMA ff05, ff06
    uint8_t tac_ {0}; // ff07
    uint16_t div_ {0};

};

}
