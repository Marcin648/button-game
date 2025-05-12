#include <Arduino.h>

#include "types.hpp"
#include "button.hpp"
#include "led.hpp"
#include "network.hpp"
#include "game_master.hpp"
#include "game_slave.hpp"

Button button(CONFIG_BUTTON_GAME_PIN_BUTTON);
LED led_power(CONFIG_BUTTON_GAME_PIN_LED_POWER);
LED led_button(CONFIG_BUTTON_GAME_PIN_LED_BUTTON);
Net::Network network;
Game::Master master;

void setup() {
    Serial.begin(115200);
    network.begin();
    master.begin(&button, &led_button, &network);
    led_power.set(LED::State::ON);
}

void loop() {
    network.update();
    led_power.update();
    led_button.update();
    button.update();
    delay(1);
}