#pragma once

#include <types.hpp>
#include "hid_keys.hpp"

class HIDKeyboard {
public:
    static const u32 PRESS_DURATION = 50;
    static const usize KEY_QUEUE_SIZE = 16;

    HIDKeyboard() = default;

    void begin();

    void send_key(HIDKey key, HIDModifier modifier = HIDModifier::NONE);
    void send_media(HIDMediaKey key);

    void update();
private:
    static u8 _init;

    void _send_raport_keyboard(HIDKey key, HIDModifier modifier);
    void _send_raport_media(HIDMediaKey media_key);

    void _update_raport_keyboard(usize index);
    void _update_raport_media(usize index);

    HIDKey _key_queue[HIDKeyboard::KEY_QUEUE_SIZE] = {HIDKey::KEY_NONE};
    HIDModifier _key_modifier_queue[HIDKeyboard::KEY_QUEUE_SIZE] = {HIDModifier::NONE};
    usize _key_queue_index = 0;
    u8 _key_pressed = false;
    u32 _key_pressed_timer = 0;

    HIDMediaKey _media_key_queue[HIDKeyboard::KEY_QUEUE_SIZE] = {HIDMediaKey::NONE};
    usize _media_key_queue_index = 0;
    u8 _media_key_pressed = false;
    u32 _media_key_pressed_timer = 0;

};