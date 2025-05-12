#include "button.hpp"

#include <Arduino.h>

Button::Button(u8 pin, u32 debounce_time) {
    this->_pin = pin;
    this->_debounce_time = debounce_time;
    pinMode(pin, INPUT_PULLUP);
}

void Button::attach_press_callback(std::function<void(void*)> callback, void* arg) {
    this->_press_callback = callback;
    this->_press_callback_arg = arg;
}

void Button::attach_release_callback(std::function<void(void*)> callback, void* arg) {
    this->_release_callback = callback;
    this->_release_callback_arg = arg;
}

void Button::update() {
    u8 state = !digitalRead(this->_pin);
    u32 now = millis();
    
    if (this->_state != state && now - this->_toggle_time > this->_debounce_time) {
        this->_toggle_time = now;
        this->_state = state;

        switch (state) {
            case HIGH: {
                if (this->_press_callback) {
                    this->_press_callback(this->_press_callback_arg);
                }
                break;
            }
            case LOW: {
                if (this->_release_callback) {
                    this->_release_callback(this->_release_callback_arg);
                }
                break;
            }
        }
    }
}