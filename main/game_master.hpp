#pragma once

#include <types.hpp>
#include <game.hpp>
#include <button.hpp>
#include <led.hpp>
#include <network.hpp>
#include <hid_keyboard.hpp>

namespace Game {
    class Master {
    public:
        Master() = default;

        void begin(Button* button, LED* led, Net::Network* network, HIDKeyboard* keyboard);
    protected:
        Button* _button = nullptr;
        LED* _led = nullptr;
        Net::Network* _network = nullptr;
        HIDKeyboard* _keyboard = nullptr;

        State _state = State::WAITING;

        void _on_press();

        void _on_connected();
        void _on_disconnected();

        void _on_trigger(const u8* trigger_mac);

        void _on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size);
    };
}