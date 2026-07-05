#include <Arduino.h>
#include "display.h"
#include "imu.h"
#include "dice.h"

Dice dice;

void setup(void) {
    Serial.begin(115200);
    init_display();
    init_mpu();
    
    dice.init();

    dice.drawCurrentDice(u8g2);
    u8g2.sendBuffer();

    Serial.println("");
    delay(100);
}

void loop() {
    Gesture gesture = get_gesture();
    switch (gesture) {
        case Gesture::SHAKE:
            Serial.println("SHAKE detected");
            dice.rollDice();
            dice.drawCurrentDice(u8g2);
            u8g2.sendBuffer();
            break;
        case Gesture::TILT_LEFT:
            Serial.println("TILT LEFT detected");
            dice.previousDice();
            dice.drawCurrentDice(u8g2);
            u8g2.sendBuffer();
            break;
        case Gesture::TILT_RIGHT:
            Serial.println("TILT RIGHT detected");
            dice.nextDice();
            dice.drawCurrentDice(u8g2);
            u8g2.sendBuffer();
            break;
        default:
            break;
    }

    delay(10);
}