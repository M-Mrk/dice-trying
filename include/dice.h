#pragma once
#include <Arduino.h>
#include "U8g2lib.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int TOP_LEFT_PIP[2] = {10, 10};
constexpr int PIP_DIAMETER = 2;
constexpr int PIP_SPACING = 4;

enum class DiceType {
    D4,
    D6,
    D8,
    D10,
    D12,
    D20,
};

class Dice {
public:
    void init();
    void drawCurrentDice(U8G2 &u8g2);
    void rollDice();
    void nextDice();
    void previousDice();

private:
    DiceType current_dice();
    void drawPips(int value, U8G2 &u8g2);
    void drawNumber(int value, U8G2 &u8g2);
    void clearDiceFace(U8G2 &u8g2);

    int pip_positions[9][2];
    int current_roll;
    int current_dice_index;
    DiceType dice_order[6] = {DiceType::D4, DiceType::D6, DiceType::D8, DiceType::D10, DiceType::D12, DiceType::D20};
};