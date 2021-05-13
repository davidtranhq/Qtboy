#ifndef JOYPAD_H
#define JOYPAD_H

#include <cstdint>
#include <bitset>

namespace qtboy
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
    void reset();

    private:
    void update_button(Input, bool pressed);

    private:
    uint8_t directions_ {0};
    uint8_t buttons_ {0};
    bool select_button_ {0}, select_direction_ {0};
    Processor &cpu_;
};

}

#endif // JOYPAD_H
