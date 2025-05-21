#include <Arduino.h>

#include "types.hpp"
#include "button.hpp"
#include "led.hpp"
#include "network.hpp"
#include "game_slave.hpp"

Button button(CONFIG_BUTTON_GAME_PIN_BUTTON);
LED led_power(CONFIG_BUTTON_GAME_PIN_LED_POWER);
LED led_button(CONFIG_BUTTON_GAME_PIN_LED_BUTTON);
Net::Network network;
Game::Slave slave;

const u8 master_mac[6] = {0xD8, 0x3B, 0xDA, 0x73, 0xC8, 0x1C};

void setup() {
    Serial.begin(115200);
    network.begin();
    slave.begin(&button, &led_button, &network, master_mac);
    led_power.set(LED::State::ON);
}

void loop() {
    network.update();
    led_power.update();
    led_button.update();
    button.update();
    delay(1);
}