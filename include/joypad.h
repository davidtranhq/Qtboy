#ifndef JOYPAD_H
#define JOYPAD_H

#include <cstdint>
#include <bitset>

namespace gameboy
{

class Processor;

class Joypad
{
    public:
    enum class Input
    {
        A, B, Right, Left, Up, Down, Start, Select
    };

    explicit Joypad(Processor &p);

    void press(Input);
    void release(Input);
    uint8_t read_reg();
    void write_reg(uint8_t b);

    private:
    void update_button(Input, bool pressed);

    private:
    uint8_t directions_;
    uint8_t buttons_;
    bool select_button_, select_direction_;
    Processor &cpu_;
};

}

#endif // JOYPAD_H
