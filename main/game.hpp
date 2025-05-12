#pragma once

namespace Game {
    const u32 LED_DURATION_WAITING = 1000;
    const u32 LED_DURATION_WINNING = 250;

    enum class MessageType : u8 {
        START = 0,
        TRIGGER = 1,
        WINNER = 2
    };

    enum class State : u8 {
        WAITING = 0,
        PLAYING = 1,
        WINNER = 2
    };
}