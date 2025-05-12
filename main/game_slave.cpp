#include "game_slave.hpp"
#include "esp_log.h"

#include <cstring>

using namespace Game;

void Slave::begin(Button* button, LED* led, Net::Network* network, const u8* master_mac) {
    this->_button = button;
    this->_led = led;
    this->_network = network;

    this->_state = State::WAITING;

    this->_network->get_mac(this->_mac);
    this->_network->add_peer(master_mac);

    this->_led->set(LED::State::PULSE, 1000);

    this->_button->attach_press_callback([this](void* arg) {
        this->_on_press();
    }, nullptr);

    this->_network->attach_connected_callback([this](void* arg) {
        this->_on_connected();
    }, nullptr);

    this->_network->attach_disconnected_callback([this](void* arg) {
        this->_on_disconnected();
    }, nullptr);

    this->_network->attach_data_callback([this](void* arg, const u8* peer_mac, u16 message_type, const u8* data, usize size) {
        this->_on_data(peer_mac, message_type, data, size);
    }, nullptr);
}

void Slave::_on_connected() {
    ESP_LOGI("GameSlave", "Connected");
    this->_led->set(LED::State::OFF);
}

void Slave::_on_disconnected() {
    ESP_LOGI("GameSlave", "Disconnected");
    this->_state = State::WAITING;
    this->_led->set(LED::State::PULSE, 1000);
}

void Slave::_on_press() {
    if (this->_state == State::PLAYING) {
        this->_network->send(static_cast<u8>(MessageType::TRIGGER));
    }
}

void Slave::_on_start() {
    ESP_LOGI("GameSlave", "Game started");
    this->_state = State::PLAYING;
    this->_led->set(LED::State::ON);
}

void Slave::_on_winner(const u8* winner_mac) {
    if (this->_state == State::PLAYING) {
        this->_state = State::WINNER;
        if (std::memcmp(this->_mac, winner_mac, 6) == 0) {
            ESP_LOGI("GameSlave", "I am the winner");
            this->_led->set(LED::State::BLINK, LED_DURATION_WINNING);
        } else {
            ESP_LOGI("GameSlave", "I am not the winner");
            this->_led->set(LED::State::OFF);
        }
    }
}

void Slave::_on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size) {
    switch (static_cast<MessageType>(message_type)) {
        case MessageType::TRIGGER: break;
        case MessageType::START : {
            this->_on_start();
            break;
        };
        case MessageType::WINNER : {
            if (size == 6) {
                this->_on_winner(data);
            }
            break;
        };
    }
} 