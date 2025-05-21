#include <Arduino.h>

#include "button.hpp"
#include "led.hpp"
#include "network.hpp"
#include "hid_keyboard.hpp"
#include "game_master.hpp"

Button button(CONFIG_BUTTON_GAME_PIN_BUTTON);
LED led_power(CONFIG_BUTTON_GAME_PIN_LED_POWER);
LED led_button(CONFIG_BUTTON_GAME_PIN_LED_BUTTON);
Net::Network network;
Game::Master master;
HIDKeyboard keyboard;

void setup() {
    Serial.begin(115200);
    network.begin();
    master.begin(&button, &led_button, &network, &keyboard);
    led_power.set(LED::State::ON);

    if (button.read() == LOW) {
        keyboard.begin();
    }
}

void loop() {
    network.update();
    led_power.update();
    led_button.update();
    button.update();
    keyboard.update();
    delay(1);
}