#pragma once

#include <types.hpp>
#include <game.hpp>
#include <button.hpp>
#include <led.hpp>
#include <network.hpp>

namespace Game {
    class Slave {
    public:
        Slave() = default;

        void begin(Button* button, LED* led, Net::Network* network, const u8* master_mac);
        void update();
    protected:
        Button* _button = nullptr;
        LED* _led = nullptr;
        Net::Network* _network = nullptr;

        u8 _mac[6] = {0};

        State _state = State::WAITING;

        bool _pressed = false;
        void _on_press();

        void _on_connected();
        void _on_disconnected();

        void _on_start();
        void _on_winner(const u8* winner_mac);

        void _on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size);
    };
}