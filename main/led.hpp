#pragma once

#include "types.hpp"

class LED {
public:
    enum class State {
        OFF = 0,
        ON = 1,
        BLINK = 2,
        PULSE = 3,
    };

    LED(u8 pin);

    void set(State state, u32 duration = 0);
    void update();
protected:
    u8 _pin;
    
    State _state = State::OFF;
    u32 _duration = 0;
    u32 _offset = 0;

    void _write(u32 value, u32 scale = 1);
};