#pragma once
#include <stdint.h>

#define DEBUG_RAW_VALUES false

enum class Gesture {
    NONE,
    SHAKE,
    TILT_LEFT,
    TILT_RIGHT,
    TILT_UP,
    TILT_DOWN,
};

void calibrate_imu();
void init_mpu();
bool test();
Gesture get_gesture();