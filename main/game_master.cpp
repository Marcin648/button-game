#include "game_master.hpp"

#include "esp_log.h"

using namespace Game;

void Master::begin(Button* button, LED* led, Net::Network* network) {
    this->_button = button;
    this->_led = led;
    this->_network = network;

    this->_state = State::WAITING;
    this->_pressed = false;

    this->_led->set(LED::State::PULSE, LED_DURATION_WAITING);

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

void Master::_on_connected() {
    ESP_LOGI("GameMaster", "Connected");
    this->_led->set(LED::State::OFF);
}

void Master::_on_disconnected() {
    ESP_LOGI("GameMaster", "Disconnected");
    this->_state = State::WAITING;
    this->_led->set(LED::State::PULSE, LED_DURATION_WAITING);
}

void Master::_on_press() {
    this->_state = State::PLAYING;
    this->_network->send(static_cast<u8>(MessageType::START));
}

void Master::_on_trigger(const u8* trigger_mac) {
    if (this->_state == State::PLAYING) {
        ESP_LOGI("GameMaster", "Winner %02X:%02X:%02X:%02X:%02X:%02X", trigger_mac[0], trigger_mac[1], trigger_mac[2], trigger_mac[3], trigger_mac[4], trigger_mac[5]);
        this->_state = State::WINNER;
        this->_network->send(static_cast<u8>(MessageType::WINNER), trigger_mac, 6);
    }
}

void Master::_on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size) {
    switch (static_cast<MessageType>(message_type)) {
        case MessageType::TRIGGER: {
            this->_on_trigger(peer_mac);
            break;
        }

        case MessageType::START : break;
        case MessageType::WINNER : break;
    }
}