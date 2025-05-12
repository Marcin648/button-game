#pragma once

#include <functional>

#include <types.hpp>
#include <esp_now.h>

namespace Net {
    static const u32 MAGIC = 0xC0FFEE;
    static const usize MAX_PAYLOAD_SIZE = 250;

    static const u32 HEARTBEAT_INTERVAL = 1000;
    static const u32 HEARTBEAT_TIMEOUT = 5000;

    enum class PacketType : u8 {
        HEARTBEAT = 0,
        DATA = 1,
    };

    struct Packet {
        u32 magic;
        PacketType type;
        u8 message_type;
        u8 payload[MAX_PAYLOAD_SIZE];
    };

    enum class NetworkState : u8 {
        DISCONNECTED = 0,
        CONNECTED = 1,
    };

    class Network {
    public:
        Network() = default;

        void begin();

        void get_mac(u8* mac);

        void add_peer(const u8* peer_mac);
        void delete_peer(const u8* peer_mac);

        void send(u8 message_type, const u8* data = nullptr, usize size = 0, const u8* peer_mac = nullptr);

        void attach_heartbeat_callback(std::function<void(void*)> callback, void* arg = nullptr);
        void attach_data_callback(std::function<void(void*, const u8*, u16, const u8*, usize)> callback, void* arg = nullptr);
        void attach_connected_callback(std::function<void(void*)> callback, void* arg = nullptr);
        void attach_disconnected_callback(std::function<void(void*)> callback, void* arg = nullptr);

        void update();
    protected:
        static Network* _network;
        NetworkState _state = NetworkState::DISCONNECTED;
        
        u32 _last_heartbeat = 0;
        u32 _heartbeat_timer = 0;

        std::function<void(void*)> _heartbeat_callback = nullptr;
        void* _heartbeat_callback_arg = nullptr;

        std::function<void(void*, const u8*, u16, const u8*, usize)> _data_callback = nullptr;
        void* _data_callback_arg = nullptr;

        std::function<void(void*)> _connected_callback = nullptr;
        void* _connected_callback_arg = nullptr;

        std::function<void(void*)> _disconnected_callback = nullptr;
        void* _disconnected_callback_arg = nullptr;

        void _send(PacketType type, u16 message_type = 0, const u8* data = nullptr, usize size = 0, const u8* peer_mac = nullptr);
        
        static void _on_recv(const esp_now_recv_info_t* esp_now_info, const u8* data, int size);
        void _on_heartbeat(const u8* peer_mac);
        void _on_data(const u8* peer_mac, u16 message_type, const u8* data, usize size);
        void _on_connected();
        void _on_disconnected();
    };
}