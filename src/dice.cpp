#include "dice.h"

int max_val_dice(DiceType dice) {
    switch (dice) {
        case DiceType::D4:
            return 4;
        case DiceType::D6:
            return 6;
        case DiceType::D8:
            return 8;
        case DiceType::D10:
            return 10;
        case DiceType::D12:
            return 12;
        case DiceType::D20:
            return 20;
        default:
            return 6; // Default to D6 if unknown
    }
}

DiceType Dice::current_dice() {
    return this->dice_order[this->current_dice_index];
}

void Dice::init() {
    this->current_dice_index = 1; // Start with D6

    // top row of pips
    for (int i = 0; i < 3; ++i) {
        this->pip_positions[i][0] = TOP_LEFT_PIP[0] + (i * (PIP_DIAMETER + PIP_SPACING));
        this->pip_positions[i][1] = TOP_LEFT_PIP[1];
    }

    // middle row of pips
    for (int i = 3; i < 6; ++i) {
        this->pip_positions[i][0] = TOP_LEFT_PIP[0] + ((i - 3) * (PIP_DIAMETER + PIP_SPACING));
        this->pip_positions[i][1] = TOP_LEFT_PIP[1] + (PIP_DIAMETER + PIP_SPACING);
    }

    // bottom row of pips
    for (int i = 6; i < 9; ++i) {
        this->pip_positions[i][0] = TOP_LEFT_PIP[0] + ((i - 6) * (PIP_DIAMETER + PIP_SPACING));
        this->pip_positions[i][1] = TOP_LEFT_PIP[1] + 2 * (PIP_DIAMETER + PIP_SPACING);
    }
}

void Dice::nextDice() {
    this->current_dice_index = (this->current_dice_index + 1) % 6;
    this->current_roll = max_val_dice(this->current_dice()); // Reset roll to max value of new dice
}

void Dice::previousDice() {
    this->current_dice_index = (this->current_dice_index - 1 + 6) % 6;
    this->current_roll = max_val_dice(this->current_dice()); // Reset roll to max value of new dice
}

void Dice::rollDice() {
    this->current_roll = random(1, max_val_dice(this->current_dice())+1);
}

void Dice::drawCurrentDice(U8G2 &u8g2) {
    // Draw the dice type
    const char* dice_type_str;
    switch (this->current_dice()) {
        case DiceType::D4:
            dice_type_str = "D4";
            break;
        case DiceType::D6:
            dice_type_str = "D6";
            break;
        case DiceType::D8:
            dice_type_str = "D8";
            break;
        case DiceType::D10:
            dice_type_str = "D10";
            break;
        case DiceType::D12:
            dice_type_str = "D12";
            break;
        case DiceType::D20:
            dice_type_str = "D20";
            break;
        default:
            dice_type_str = "??"; // Default to D6 if unknown
    }
    u8g2.setDrawColor(0); // Clear area
    u8g2.drawBox(0, 0, SCREEN_WIDTH, 10);
    u8g2.setDrawColor(1);
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(0, 10);
    u8g2.print(dice_type_str);


    // Draw value
    int value = this->current_roll;
    if (value == 0) {
        value = 1; // Default to 1 if not rolled yet
    }

    if (this->current_dice_index <= 2) { // D4, D6, D8
        drawPips(value, u8g2);
    } else {
        drawNumber(value, u8g2);
    }
}

void Dice::drawPips(int value, U8G2 &u8g2) {
    clearDiceFace(u8g2);

    bool draw_pip[9] = {false};

    switch (value) {
        case 1:
            draw_pip[4] = true;
            break;

        case 2:
            draw_pip[0] = true;
            draw_pip[8] = true;
            break;

        case 3:
            draw_pip[0] = true;
            draw_pip[4] = true;
            draw_pip[8] = true;
            break;

        case 4:
            draw_pip[0] = true;
            draw_pip[2] = true;
            draw_pip[6] = true;
            draw_pip[8] = true;
            break;

        case 5:
            draw_pip[0] = true;
            draw_pip[2] = true;
            draw_pip[4] = true;
            draw_pip[6] = true;
            draw_pip[8] = true;
            break;
        
        case 6:
            draw_pip[0] = true;
            draw_pip[2] = true;
            draw_pip[3] = true;
            draw_pip[5] = true;
            draw_pip[6] = true;
            draw_pip[8] = true;
            break;
        
        default:
            for (int i = 0; i < value && i < 9; ++i) {
                draw_pip[i] = true;
            }
    }

    for (int i = 0; i < 9; ++i) {
        if (draw_pip[i]) {
            u8g2.drawDisc(this->pip_positions[i][0], this->pip_positions[i][1], PIP_DIAMETER);
        }
    }
}

void Dice::drawNumber(int value, U8G2 &u8g2) {
    clearDiceFace(u8g2);

    char buffer[4];
    snprintf(buffer, sizeof(buffer), "%d", value);
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.setCursor(SCREEN_WIDTH / 2 - 7, SCREEN_HEIGHT / 2 + 7);
    u8g2.print(buffer);
}

void Dice::clearDiceFace(U8G2 &u8g2) {
    const int top_left_y = TOP_LEFT_PIP[1] + PIP_DIAMETER + 2;
    const int top_left_x = TOP_LEFT_PIP[0] - PIP_DIAMETER - 2;

    const int width = 3 * (PIP_DIAMETER + PIP_SPACING) + 4;
    const int height = 3 * (PIP_DIAMETER + PIP_SPACING) + 4;

    u8g2.setDrawColor(0);
    u8g2.drawBox(top_left_x, top_left_y, width, height);
    u8g2.setDrawColor(1);
}