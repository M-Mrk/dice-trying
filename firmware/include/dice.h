#pragma once
#include <Arduino.h>
#include "U8g2lib.h"
#include "display.h"

constexpr int MIDDLE_PIP[2] = {(SCREEN_WIDTH)/2, MIDDLE_Y_BLUE};
constexpr int PIP_DIAMETER = 4;
constexpr int PIP_SPACING = PIP_DIAMETER * 3;  // = 8 (gap = PIP_DIAMETER)

struct Point { int x; int y; };
constexpr Point PIP_POSITIONS[9] = {
    {MIDDLE_PIP[0] - PIP_SPACING, MIDDLE_PIP[1] - PIP_SPACING}, // 0: top-left
    {MIDDLE_PIP[0],                MIDDLE_PIP[1] - PIP_SPACING}, // 1: top-middle
    {MIDDLE_PIP[0] + PIP_SPACING, MIDDLE_PIP[1] - PIP_SPACING}, // 2: top-right
    {MIDDLE_PIP[0] - PIP_SPACING, MIDDLE_PIP[1]},                // 3: middle-left
    {MIDDLE_PIP[0],                MIDDLE_PIP[1]},                // 4: center
    {MIDDLE_PIP[0] + PIP_SPACING, MIDDLE_PIP[1]},                // 5: middle-right
    {MIDDLE_PIP[0] - PIP_SPACING, MIDDLE_PIP[1] + PIP_SPACING}, // 6: bottom-left
    {MIDDLE_PIP[0],                MIDDLE_PIP[1] + PIP_SPACING}, // 7: bottom-middle
    {MIDDLE_PIP[0] + PIP_SPACING, MIDDLE_PIP[1] + PIP_SPACING}, // 8: bottom-right
};

enum class DiceType {
    D2,
    D4,
    D6,
    D8,
    D10,
    D12,
    D20,
};

class Dice {
public:
    void drawCurrentDice(U8G2 &u8g2);
    void rollDice();
    void nextDice();
    void previousDice();
    void rolling_animation(U8G2 &u8g2);
    int max_val();
    int current_roll;
    DiceType current_dice();
    void set_dice(DiceType dice_type);

private:
    void drawPips(int value, U8G2 &u8g2);
    void drawNumber(int value, U8G2 &u8g2);
    void clearDiceFace(U8G2 &u8g2);
    
    int current_dice_index;
    DiceType dice_order[7] = {DiceType::D2, DiceType::D4, DiceType::D6, DiceType::D8, DiceType::D10, DiceType::D12, DiceType::D20};
};