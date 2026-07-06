#include <Arduino.h>
#include "display.h"
#include "imu.h"
#include "dice.h"
#include "pins.h"

Dice dice;
RTC_DATA_ATTR DiceType last_dice_type = DiceType::D6;

void setup(void) {
    //Serialbegin(115200);

    pinMode(PREVIOUS_BTN_PIN, INPUT_PULLUP);
    pinMode(NEXT_BTN_PIN, INPUT_PULLUP);
    
    init_display();
    u8g2.setPowerSave(0);
    u8g2.clearDisplay();
    //Serialprintln("Display initialized");
    u8g2.setDrawColor(1);
    
    init_mpu();
    //Serialprintln("MPU initialized");
    calibrate_imu();
    //Serialprintln("IMU calibrated");
    
    dice.set_dice(last_dice_type);
    //Serialprintln("Dice initialized");

    dice.drawCurrentDice(u8g2);
    u8g2.sendBuffer();

    // Wake up when IMU interrupt gets triggered
    esp_deep_sleep_enable_gpio_wakeup(IMU_INT_PIN, ESP_GPIO_WAKEUP_GPIO_LOW);

    //Serialprintln("Init finished");
    delay(100);
}

void sleep() {
    //Serialprintln("Going to sleep");
    u8g2.clearDisplay();
    u8g2.clearBuffer();
    u8g2.setPowerSave(1);
    last_dice_type = dice.current_dice();
    delay(100);
    esp_deep_sleep_start();
}

void awake() {
}

long g_last_action = 0;
const long timeout_till_sleep = 15000;
// Button debouncing state
int prevBtnLastState = HIGH;
int nextBtnLastState = HIGH;
unsigned long prevLastDebounce = 0;
unsigned long nextLastDebounce = 0;
const unsigned long debounceDelay = 50;
void loop() {
    // Only use SHAKE gesture for rolling the dice
    Gesture gesture = get_gesture();
    if (gesture == Gesture::SHAKE) {
        //Serialprintln("SHAKE detected");
        dice.rollDice();
        dice.drawCurrentDice(u8g2);
        u8g2.sendBuffer();
        g_last_action = millis();
        delay(200); // give time for animation
        return;
    }

    // Handle previous/next buttons to switch dice type (active LOW)
    int prevState = digitalRead(PREVIOUS_BTN_PIN);
    int nextState = digitalRead(NEXT_BTN_PIN);
    
    if (prevState == LOW) {
        //Serialprintln("Previous button pressed");
        dice.previousDice();
        dice.drawCurrentDice(u8g2);
        u8g2.sendBuffer();
        g_last_action = millis();
        delay(200); // simple debounce
        return;
    }

    if (nextState == LOW) {
        //Serialprintln("Next button pressed");
        dice.nextDice();
        dice.drawCurrentDice(u8g2);
        u8g2.sendBuffer();
        g_last_action = millis();
        delay(200); // simple debounce
        return;
    }

    // Nothing else to do
    if (millis() - g_last_action > timeout_till_sleep) {
        // sleep();
    }
    delay(50);
}