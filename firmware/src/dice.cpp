#include "dice.h"

bool dice_to_text(DiceType dice, char* buffer, size_t buffer_size) {
    // Validate inputs
    if (buffer == nullptr || buffer_size == 0) {
        return false;
    }
    
    const char* dice_type_str;
    switch (dice) {
        case DiceType::D2:
            dice_type_str = "D2";
            break;
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
            dice_type_str = "UNKNOWN";
            break;
    }
    
    // snprintf returns the number of characters that would have been written
    int result = snprintf(buffer, buffer_size, "%s", dice_type_str);
    
    // Return false if buffer was too small or snprintf failed
    return result >= 0 && (size_t)result < buffer_size;
}


int Dice::max_val() {
    switch (this->current_dice()) {
        case DiceType::D2:
            return 2;
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

void Dice::set_dice(DiceType dice_type) {
    for (int i = 0; i < 7; ++i) {
        if (this->dice_order[i] == dice_type) {
            this->current_dice_index = i;
            this->current_roll = this->max_val();
            return;
        }
    }
}

void Dice::nextDice() {
    this->current_dice_index = (this->current_dice_index + 1) % 7;
    this->current_roll = this->max_val(); // Reset roll to max value of new dice
}

void Dice::previousDice() {
    this->current_dice_index = (this->current_dice_index - 1 + 7) % 7;
    this->current_roll = this->max_val(); // Reset roll to max value of new dice
}

void Dice::rollDice() {
    uint32_t random = esp_random();
    this->current_roll = random % this->max_val() + 1;
    //Serialprint("Rolled a ");
    //Serialprint(this->max_val());
    //Serialprint(": ");
    //Serialprint(this->current_roll);
    //Serialprint(" on a ");
    const char* dice_type_str;
    dice_to_text(this->current_dice(), (char*)dice_type_str, 4);
    //Serialprintln(dice_type_str);
}

void Dice::drawCurrentDice(U8G2 &u8g2) {
    // Draw the dice type
    char dice_type_str[5];
    if (!dice_to_text(this->current_dice(), (char*)dice_type_str, 4)) {
        // Handle error, e.g., set a default value
        snprintf((char*)dice_type_str, 4, "???");
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

    if (this->current_dice_index <= 2) { // D2, D4, D6
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
        
    }

    for (int i = 0; i < 9; ++i) {
        if (draw_pip[i]) {
            u8g2.drawDisc(PIP_POSITIONS[i].x, PIP_POSITIONS[i].y, PIP_DIAMETER);
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
    const int clear_margin = 5;
    
    // Leftmost and topmost pip positions (plus radius + margin)
    const int top_left_x = MIDDLE_PIP[0] - PIP_SPACING - PIP_DIAMETER/2 - clear_margin;
    const int top_left_y = MIDDLE_PIP[1] - PIP_SPACING - PIP_DIAMETER/2 - clear_margin;

    // Distance from leftmost to rightmost pip, plus their diameters and margins
    const int width = 2 * PIP_SPACING + PIP_DIAMETER + 2 * clear_margin;
    const int height = 2 * PIP_SPACING + PIP_DIAMETER + 2 * clear_margin;

    u8g2.setDrawColor(0);
    u8g2.drawBox(top_left_x, top_left_y, width, height);
    u8g2.setDrawColor(1);
}


void Dice::rolling_animation(U8G2 &u8g2) {
    clearDiceFace(u8g2);
    int num_pips = random(1, 9+1);
    drawPips(num_pips, u8g2);
}