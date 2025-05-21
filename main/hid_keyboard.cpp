#include "hid_keyboard.hpp"

#include "Arduino.h"
#include "tinyusb.h"

#define TUSB_DESC_TOTAL_LEN (TUD_CONFIG_DESC_LEN + CFG_TUD_HID * TUD_HID_DESC_LEN)

enum {
    HID_REPORT_ID_NONE = 0,
    HID_REPORT_ID_KEYBOARD = 1,
    HID_REPORT_ID_MOUSE = 2,
    HID_REPORT_ID_CONSUMER = 3,
};

const u8 hid_report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD)),
    TUD_HID_REPORT_DESC_CONSUMER(HID_REPORT_ID(HID_REPORT_ID_CONSUMER)),
};

static const u8 hid_configuration_descriptor[] = {
    TUD_CONFIG_DESCRIPTOR(1, 1, 0, TUSB_DESC_TOTAL_LEN, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100),
    TUD_HID_DESCRIPTOR(0, 4, false, sizeof(hid_report_descriptor), 0x81, 16, 10),
};

extern "C" {
    u8 const* tud_hid_descriptor_report_cb(u8 instance) {
        return hid_report_descriptor;
    }

    u16 tud_hid_get_report_cb(u8 instance, u8 report_id, hid_report_type_t report_type, u8* buffer, u16 reqlen) {
        return 0;
    }

    void tud_hid_set_report_cb(u8 instance, u8 report_id, hid_report_type_t report_type, u8 const* buffer, u16 bufsize) {
        return;
    }
}

u8 HIDKeyboard::_init = false;

void HIDKeyboard::begin() {
    if (HIDKeyboard::_init) {
        return;
    }

    const tinyusb_config_t tusb_cfg = {
        .device_descriptor = NULL,
        .string_descriptor = NULL,
        .string_descriptor_count = 0,
        .external_phy = false,
        .configuration_descriptor = hid_configuration_descriptor,
    };

    HIDKeyboard::_init = tinyusb_driver_install(&tusb_cfg) == ESP_OK;
}

void HIDKeyboard::send_key(HIDKey key, HIDModifier modifier) {
    this->_key_queue[this->_key_queue_index] = key;
    this->_key_modifier_queue[this->_key_queue_index] = modifier;
    ++this->_key_queue_index %= KEY_QUEUE_SIZE;
}

void HIDKeyboard::send_media(HIDMediaKey key) {
    this->_media_key_queue[this->_media_key_queue_index] = key;
    ++this->_media_key_queue_index %= KEY_QUEUE_SIZE;
}

void HIDKeyboard::_send_raport_keyboard(HIDKey key, HIDModifier modifier) {
    if (key == HIDKey::KEY_NONE) {
        tud_hid_keyboard_report(HID_REPORT_ID_KEYBOARD, 0, NULL);
    } else {
        u8 key_code[6] = {static_cast<u8>(key)};
        u8 modifier_code = static_cast<u8>(modifier);
        tud_hid_keyboard_report(HID_REPORT_ID_KEYBOARD, modifier_code, key_code);
    }
}

void HIDKeyboard::_send_raport_media(HIDMediaKey media_key) {
    uint16_t media_keycode = static_cast<uint16_t>(media_key);
    tud_hid_report(HID_REPORT_ID_CONSUMER, &media_keycode, sizeof(media_keycode));
}

void HIDKeyboard::_update_raport_keyboard(usize index) {
    u32 now = millis();
    if (this->_key_pressed && now - this->_key_pressed_timer > PRESS_DURATION) {
        this->_send_raport_keyboard(HIDKey::KEY_NONE, HIDModifier::NONE);
        this->_key_pressed = false;
    } else if (this->_key_queue[index] != HIDKey::KEY_NONE) {
        this->_send_raport_keyboard(this->_key_queue[index], this->_key_modifier_queue[index]);
        this->_key_queue[index] = HIDKey::KEY_NONE;
        this->_key_modifier_queue[index] = HIDModifier::NONE;
        this->_key_pressed = true;
        this->_key_pressed_timer = now;
    }
}

void HIDKeyboard::_update_raport_media(usize index) {
    u32 now = millis();
    if (this->_media_key_pressed && now - this->_media_key_pressed_timer > PRESS_DURATION) {
        this->_send_raport_media(HIDMediaKey::NONE);
        this->_media_key_pressed = false;
    } else if (this->_media_key_queue[index] != HIDMediaKey::NONE) {
        this->_send_raport_media(this->_media_key_queue[index]);
        this->_media_key_queue[index] = HIDMediaKey::NONE;
        this->_media_key_pressed = true;
        this->_media_key_pressed_timer = now;
    }
}

// This queue code is a mess. I should be separated from HID logic, but who cares? Sometimes I need to sleep...
void HIDKeyboard::update() {
    if (HIDKeyboard::_init && tud_mounted() && tud_hid_ready()) {
        for(usize i = 0; i < KEY_QUEUE_SIZE; i++) {
            if (this->_key_pressed || this->_key_queue[i] != HIDKey::KEY_NONE) {
                this->_update_raport_keyboard(i);
                break;
            }
            if (this->_media_key_pressed || this->_media_key_queue[i] != HIDMediaKey::NONE) {
                this->_update_raport_media(i);
                break;
            }
        }
    }
}