#include "button.hpp"

#include <Arduino.h>

Button::Button(u8 pin, u32 debounce_time) {
    this->_pin = pin;
    this->_debounce_time = debounce_time;
    pinMode(pin, INPUT_PULLUP);
    attachInterruptArg(digitalPinToInterrupt(pin), Button::_on_change, this, CHANGE);
}

void Button::attach_press_callback(std::function<void(void*)> callback, void* arg) {
    this->_press_callback = callback;
    this->_press_callback_arg = arg;
}

void Button::attach_release_callback(std::function<void(void*)> callback, void* arg) {
    this->_release_callback = callback;
    this->_release_callback_arg = arg;
}

void Button::_on_change(void* instance) {
    Button* button = static_cast<Button*>(instance);
    u32 now = millis();
    u8 state = !digitalRead(button->_pin);
    
    if (now - button->_toggle_time > button->_debounce_time) {
        button->_toggle_time = now;

        switch (state) {
            case HIGH: {
                if (button->_press_callback) {
                    button->_press_callback(button->_press_callback_arg);
                }
                break;
            }
            case LOW: {
                if (button->_release_callback) {
                    button->_release_callback(button->_release_callback_arg);
                }
                break;
            }
        }
    }
}