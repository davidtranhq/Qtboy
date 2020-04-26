#include "joypad.hpp"
#include "processor.hpp"

using gameboy::Joypad;

#define CHANGE_BIT(b, n, x) b ^= (-x ^ b) & (1UL << n)
#define CLEAR_BIT(b, n) b &= ~(1UL << n)
#define SET_BIT(b, n) b |= (1UL << n)

Joypad::Joypad(Processor &p)
    : cpu_ {p}
{}

void Joypad::press(Input i)
{
    update_button(i, true);
}

void Joypad::release(Input i)
{
    update_button(i, false);
}

void Joypad::update_button(Input i, bool pressed)
{
    uint8_t old_buttons_ {buttons_};
    uint8_t old_directions_ {directions_};
    switch (i)
    {
        case Input::A:
            CHANGE_BIT(buttons_, 0, pressed);
            break;
        case Input::B:
            CHANGE_BIT(buttons_, 1, pressed);
            break;
        case Input::Select:
            CHANGE_BIT(buttons_, 2, pressed);
            break;
        case Input::Start:
            CHANGE_BIT(buttons_, 3, pressed);
            break;
        case Input::Right:
            CHANGE_BIT(directions_, 0, pressed);
            break;
        case Input::Left:
            CHANGE_BIT(directions_, 1, pressed);
            break;
        case Input::Up:
            CHANGE_BIT(directions_, 2, pressed);
            break;
        case Input::Down:
            CHANGE_BIT(directions_, 3, pressed);
            break;
    }

    /* JOYPAD INTERRUPT IS FINNICKY AND NOT WELL-DOCUMENTED. Luckily
     * most games don't use it. Revisit later if it causes problems. */
    /*
    if (old_buttons_ != buttons_ || old_directions_ != directions_)
        cpu_.request_interrupt(Processor::Interrupt::JOYPAD);
    */
}

uint8_t Joypad::read_reg()
{
    uint8_t reg {0xc0}; // bits 6-7 are always 1
    CHANGE_BIT(reg, 5, select_button_);
    CHANGE_BIT(reg, 4, select_direction_);
    if (select_button_)
        reg |= buttons_;
    if (select_direction_)
        reg |= directions_;
    return ~reg;
}

void Joypad::write_reg(uint8_t b)
{
    select_button_ = (b & 1 << 4);
    select_direction_ = (b & 1 << 5);
    // clear button/direction if either are unselected
}

void Joypad::reset()
{
    directions_ = 0;
    buttons_ = 0;
    select_button_ = 0;
    select_direction_ = 0;
}
