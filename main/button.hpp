#pragma once

#include <functional>
#include <types.hpp>

class Button {
public:
    static const u32 DEBOUNCE_TIME_DEFAULT = 20;

    Button(u8 pin, u32 debounce_time = DEBOUNCE_TIME_DEFAULT);

    void attach_press_callback(std::function<void(void*)> callback, void* arg = nullptr);
    void attach_release_callback(std::function<void(void*)> callback, void* arg = nullptr);

    void update();
protected:
    u8 _pin;
    u8 _state;
    u32 _debounce_time;
    u32 _toggle_time = 0;

    std::function<void(void*)> _press_callback = nullptr;
    void* _press_callback_arg = nullptr;

    std::function<void(void*)> _release_callback = nullptr;
    void* _release_callback_arg = nullptr;
};