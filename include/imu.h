#pragma once
#include <stdint.h>

enum class Gesture {
    NONE,
    SHAKE,
    TILT_LEFT,
    TILT_RIGHT,
};

void init_mpu();
Gesture get_gesture();