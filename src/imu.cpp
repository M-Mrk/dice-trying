#include "imu.h"

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

Adafruit_MPU6050 mpu;

void init_mpu() {
    // Try to initialize!
    if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
        delay(10);
    }
    }
    Serial.println("MPU6050 Found!");

    //setupt motion detection
    mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
    mpu.setMotionDetectionThreshold(1);
    mpu.setMotionDetectionDuration(20);
    mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
    mpu.setInterruptPinPolarity(true);
    mpu.setMotionInterrupt(true);
}

bool is_shake(sensors_event_t *accel_event) {
    const float shakeThreshold = 15.0; // Example threshold
 
    float x = accel_event->acceleration.x;
    float y = accel_event->acceleration.y;
    float z = accel_event->acceleration.z;

    float magnitude = sqrt(x * x + y * y + z * z);

    return magnitude > shakeThreshold;
}

bool is_tilt_left(sensors_event_t *accel_event) {
    const float tiltThreshold = -5.0;

    float x = accel_event->acceleration.x;

    return x < tiltThreshold;
}

bool is_tilt_right(sensors_event_t *accel_event) {
    const float tiltThreshold = 5.0;

    float x = accel_event->acceleration.x;

    return x > tiltThreshold;
}

Gesture get_gesture() {
    if (!mpu.getMotionInterruptStatus()) {
        return Gesture::NONE;
    }

    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    if (is_shake(&a)) {
        return Gesture::SHAKE;
    } else if (is_tilt_left(&a)) {
        return Gesture::TILT_LEFT;
    } else if (is_tilt_right(&a)) {
        return Gesture::TILT_RIGHT;
    }
    
    return Gesture::NONE;
}