#pragma once

#include <Arduino.h>
#include "U8g2lib.h"

constexpr int SCREEN_WIDTH = 128;
constexpr int SCREEN_HEIGHT = 64;
constexpr int BOTTOM_Y_YELLOW = 25;
constexpr int MIDDLE_Y_BLUE = (SCREEN_HEIGHT - BOTTOM_Y_YELLOW)/2 + BOTTOM_Y_YELLOW;

extern U8G2 u8g2;

void init_display();