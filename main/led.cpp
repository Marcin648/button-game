#include "led.hpp"
#include <Arduino.h>

LED::LED(u8 pin) {
    this->_pin = pin;
    pinMode(pin, OUTPUT);
}

void LED::_write(u32 value, u32 scale) {
    analogWrite(this->_pin, map(value, 0, scale, 0, 255));
}

void LED::set(State state, u32 duration) {
    this->_state = state;
    this->_duration = duration;
    this->_offset = millis();
}

void LED::update() {
    u32 now = millis() - this->_offset;
    u32 period = now / this->_duration;
    u32 phase = now % this->_duration;

    switch (this->_state) {
        case State::OFF:
            this->_write(LOW);
            break;
        case State::ON:
            this->_write(HIGH);
            break;
        case State::BLINK:
            this->_write((period % 2 == 0) ? HIGH : LOW);
            break;
        case State::PULSE:
            this->_write((period % 2 == 0) ? phase : this->_duration - phase, this->_duration);
            break;
    }
}