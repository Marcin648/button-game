#include "network.hpp"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_mac.h>

Net::Network* Net::Network::_network = nullptr;

void Net::Network::_on_recv(const esp_now_recv_info_t* esp_now_info, const u8* data, int size) {
    if (size < sizeof(Packet)) {
        return;
    }

    const Packet* packet = reinterpret_cast<const Packet*>(data);
    if (packet->magic != MAGIC) {
        return;
    }

    if (_network) {
        switch (static_cast<PacketType>(packet->type)) {
            case PacketType::HEARTBEAT:
                _network->_on_heartbeat(esp_now_info->src_addr);
                break;
            case PacketType::DATA:
                _network->_on_data(esp_now_info->src_addr, packet->message_type, packet->payload, size - sizeof(Packet));
                break;
        }
    }
}

void Net::Network::_on_heartbeat(const u8* peer_mac) {
    this->_last_heartbeat = millis();

    if (this->_state != NetworkState::CONNECTED) {
       this->_on_connected();
    }

    this->add_peer(peer_mac);

    if (this->_heartbeat_callback) {
        this->_heartbeat_callback(this->_heartbeat_callback_arg);
    }
}

void Net::Network::_on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size) {
    if (this->_data_callback) {
        this->_data_callback(this->_data_callback_arg, message_type, data, size);
    }
}

void Net::Network::_on_connected() {
    this->_state = NetworkState::CONNECTED;
    if(this->_connected_callback) {
        this->_connected_callback(this->_connected_callback_arg);
    }
}

void Net::Network::_on_disconnected() {
    this->_state = NetworkState::DISCONNECTED;
    if(this->_disconnected_callback) {
        this->_disconnected_callback(this->_disconnected_callback_arg);
    }
}

void Net::Network::_send(PacketType type, u16 message_type, const u8* data, usize size, const u8* peer_mac) {
    Packet packet = {0};
    packet.magic = MAGIC;
    packet.type = type;
    packet.message_type = message_type;

    usize payload_size = std::min(size, MAX_PAYLOAD_SIZE);

    memcpy(packet.payload, data, payload_size);

    esp_now_send(peer_mac, reinterpret_cast<const u8*>(&packet), sizeof(packet) + payload_size);
}

void Net::Network::begin() {
    WiFi.mode(WIFI_STA);
    esp_now_init();

    _network = this;
    esp_now_register_recv_cb(_on_recv);
}

void Net::Network::get_mac(u8* mac) {
    if (mac == nullptr) {
        return;
    }

    esp_read_mac(mac, ESP_MAC_WIFI_STA);
}

void Net::Network::add_peer(const u8* peer_mac) {
    if (esp_now_is_peer_exist(peer_mac)) {
        return;
    }

    esp_now_peer_info_t peer_info = {0};
    memcpy(peer_info.peer_addr, peer_mac, 6);
    peer_info.channel = 0;
    peer_info.ifidx = WIFI_IF_STA;
    peer_info.encrypt = false;

    esp_now_add_peer(&peer_info);
}

void Net::Network::delete_peer(const u8* peer_mac) {
    esp_now_del_peer(peer_mac);
}

void Net::Network::send(u8 message_type, const u8* data, usize size, const u8* peer_mac) {
    if(this->_state != NetworkState::CONNECTED) {
        return;
    }

    this->_send(PacketType::DATA, message_type, data, size, peer_mac);
}

void Net::Network::attach_heartbeat_callback(std::function<void(void*)> callback, void* arg) {
    this->_heartbeat_callback = callback;
    this->_heartbeat_callback_arg = arg;
}

void Net::Network::attach_data_callback(std::function<void(void*, u16, const u8*, usize)> callback, void* arg) {
    this->_data_callback = callback;
    this->_data_callback_arg = arg;
}

void Net::Network::attach_connected_callback(std::function<void(void*)> callback, void* arg) {
    this->_connected_callback = callback;
    this->_connected_callback_arg = arg;
}

void Net::Network::attach_disconnected_callback(std::function<void(void*)> callback, void* arg) {
    this->_disconnected_callback = callback;
    this->_disconnected_callback_arg = arg;
}

void Net::Network::update() {
    u32 now = millis();
    if (now - this->_heartbeat_timer > HEARTBEAT_INTERVAL) {
        this->_heartbeat_timer = now;
        this->_send(PacketType::HEARTBEAT);
    }

    if (this->_state == NetworkState::CONNECTED && now - this->_last_heartbeat > HEARTBEAT_TIMEOUT) {
        this->_on_disconnected();
    }
}