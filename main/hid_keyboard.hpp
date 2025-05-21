#pragma once

#include <types.hpp>
#include "hid_keys.hpp"

class HIDKeyboard {
public:
    const u32 PRESS_DURATION = 50;

    HIDKeyboard() = default;

    void begin();

    void send_key(HIDKey key, HIDModifier modifier = HIDModifier::NONE);
    void send_media(HIDMediaKey key);

    void update();
private:
    static u8 _init;

    void _send_raport_keyboard(HIDKey key, HIDModifier modifier);
    void _send_raport_media(HIDMediaKey media_key);

    void _update_raport_keyboard();
    void _update_raport_media();

    HIDKey _key = HIDKey::KEY_NONE;
    HIDModifier _key_modifier = HIDModifier::NONE;
    u8 _key_pressed = false;
    u32 _key_pressed_timer = 0;

    HIDMediaKey _media_key = HIDMediaKey::NONE;
    u8 _media_key_pressed = false;
    u32 _media_key_pressed_timer = 0;

};