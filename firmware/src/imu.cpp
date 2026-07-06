#include "imu.h"

#include <Arduino.h>
#include <MPU6500_WE.h>
#include <Wire.h>
#include <math.h>

#include <Wire.h>
#include <MPU6500_WE.h>
#include "pins.h"

MPU6500_WE imu(&Wire, 0x68);

class imuData {
public:
    xyzFloat accel;
    xyzFloat gyro;
};

// ---------------------------------------------------------------------------
// Calibration (call calibrate_imu() once at startup, dice sitting still & level)
// ---------------------------------------------------------------------------
static float pitchOffset = 0.0f;

void init_mpu() {
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    if (!imu.init()) {
        //Serialprintln("Failed to initialize MPU6500");
        return;
    }

    imu.enableGyrDLPF();
    imu.setGyrDLPF(MPU9250_DLPF_2);

    // Optional but recommended
    imu.enableAccDLPF(true);
}

float rawPitch(xyzFloat accel) {
    return atan2(-accel.y, sqrt(accel.x * accel.x + accel.z * accel.z)) * 180.0f / PI;
}

// Call this once at startup while the dice is sitting still and level.
// Averages a handful of samples and stores the offset so mounting
// misalignment of the IMU inside the dice gets zeroed out.
void calibrate_imu() {
    const int SAMPLES = 50;
    float sum = 0.0f;
    for (int i = 0; i < SAMPLES; i++) {
        xyzFloat a = imu.getGValues();
        sum += rawPitch(a);
        delay(10);
    }
    pitchOffset = sum / SAMPLES;

    if (DEBUG_RAW_VALUES) {
        //Serialprint("Calibrated pitch offset: ");
        //Serialprintln(pitchOffset);
    }
}

// ---------------------------------------------------------------------------
// Shake detection (accel in g, gyro in deg/s)
// ---------------------------------------------------------------------------
const float SHAKE_THRESHOLD = 2.0f;        // g - acceleration magnitude for shake
const float GYRO_SHAKE_THRESHOLD = 250.0f; // degrees/second

bool is_shake(imuData data) {
    float accelMagnitude = sqrt(
        data.accel.x * data.accel.x + 
        data.accel.y * data.accel.y + 
        data.accel.z * data.accel.z
    );
    
    if (DEBUG_RAW_VALUES) {
        //Serialprint("Accel Magnitude (g): ");
        //Serialprintln(accelMagnitude);
    }

    float gyroMagnitude = sqrt(
        data.gyro.x * data.gyro.x + 
        data.gyro.y * data.gyro.y + 
        data.gyro.z * data.gyro.z
    );
    
    // Require BOTH a hard jolt AND fast rotation - avoids classifying a
    // deliberate hand-rotation (see checkRotation below) as a shake.
    return (accelMagnitude > SHAKE_THRESHOLD) && (gyroMagnitude > GYRO_SHAKE_THRESHOLD);
}

// ---------------------------------------------------------------------------
// Tilt up/down - still gravity/pitch based, now using the calibrated baseline.
// Works while the dice is resting or held reasonably steady.
// ---------------------------------------------------------------------------
const float GRAVITY = 1.0f;            
const float TILT_ANGLE_THRESHOLD = 20.0f;  // degrees from calibrated level
const float STABLE_TOLERANCE = 0.35f;      // tolerance on gravity magnitude

float getPitch(imuData data) {
    return rawPitch(data.accel) - pitchOffset;
}

bool isStable(imuData data) {
    float accelMagnitude = sqrt(
        data.accel.x * data.accel.x +
        data.accel.y * data.accel.y +
        data.accel.z * data.accel.z
    );
    return fabs(accelMagnitude - GRAVITY) < (GRAVITY * STABLE_TOLERANCE);
}

bool is_tilt_up(imuData data) {
    if (!isStable(data)) return false;
    return getPitch(data) < -TILT_ANGLE_THRESHOLD;
}

bool is_tilt_down(imuData data) {
    if (!isStable(data)) return false;
    return getPitch(data) > TILT_ANGLE_THRESHOLD;
}

// ---------------------------------------------------------------------------
// Tilt left/right - now a ROTATION gesture, detected purely from the
// gyroscope's Z axis (yaw rate). This works fine mid-air since gyro doesn't
// depend on gravity direction the way accel does.
//
// We accumulate rotation over time and fire once cumulative rotation passes
// a threshold, rather than firing on instantaneous gyro speed - this avoids
// false triggers from brief noise spikes and only fires on a real, deliberate
// twist of the dice.
// ---------------------------------------------------------------------------
const float ROTATE_ANGLE_THRESHOLD = 45.0f; // cumulative degrees to trigger
const float ROTATE_RATE_DEADZONE = 5.0f;    // deg/s - ignore drift/noise below this
const float ROTATE_MAX_DT = 0.5f;           // seconds - reset if we haven't polled in a while

static float rotationAccum = 0.0f;
static unsigned long lastGyroTime = 0;

void resetRotation() {
    rotationAccum = 0.0f;
    lastGyroTime = 0;
}

// Returns +1 once a clockwise rotation gesture completes, -1 for
// counterclockwise, 0 otherwise.
//
// IMPORTANT: verify the sign on your hardware! Rotate the dice clockwise
// (as you're holding it, front facing you) with DEBUG_RAW_VALUES on and
// check the sign of gyro.z. If clockwise gives negative values instead,
// just flip the two return branches below.
int checkRotation(imuData data) {
    unsigned long now = millis();
    float dt = (lastGyroTime == 0) ? 0.0f : (now - lastGyroTime) / 1000.0f;
    lastGyroTime = now;

    if (dt <= 0.0f || dt > ROTATE_MAX_DT) {
        // First sample after a gap - don't integrate this cycle.
        return 0;
    }

    float gz = data.gyro.z;

    if (fabs(gz) < ROTATE_RATE_DEADZONE) {
        // Sitting still (or drifting) - decay any stale accumulation
        // instead of holding onto it forever.
        rotationAccum *= 0.9f;
        return 0;
    }

    rotationAccum += gz * dt;

    if (rotationAccum > ROTATE_ANGLE_THRESHOLD) {
        rotationAccum = 0.0f;
        return 1; // clockwise
    }
    if (rotationAccum < -ROTATE_ANGLE_THRESHOLD) {
        rotationAccum = 0.0f;
        return -1; // counterclockwise
    }
    return 0;
}

// ---------------------------------------------------------------------------
Gesture get_gesture() {
    imuData data = {imu.getGValues(), imu.getGyrValues()};

    if (DEBUG_RAW_VALUES) {
        //Serialprint("Accel: X=");
        //Serialprint(data.accel.x);
        //Serialprint(" Y=");
        //Serialprint(data.accel.y);
        //Serialprint(" Z=");
        //Serialprint(data.accel.z);
        //Serialprint(" | Gyro: X=");
        //Serialprint(data.gyro.x);
        //Serialprint(" Y=");
        //Serialprint(data.gyro.y);
        //Serialprint(" Z=");
        //Serialprintln(data.gyro.z);
    }

    if (is_shake(data)) {
        resetRotation(); // don't let a shake leave residual rotation behind
        return Gesture::SHAKE;
    }
    return Gesture::NONE;
}