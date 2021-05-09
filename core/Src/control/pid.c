#include "pid.h"
#include "math.h"

float calculateProportional(int currentPosition, int setpoint, PIDProfile profile) {
    // Calculate error
    float error = setpoint - currentPosition;
    // Calculate correction and return
    return error * profile.kP + profile.kF;
}

float value14BitToRadians(int angle) {
    return 2 * M_PI * angle / 16384.0;
}

int radiansToValue14Bit(float angle) {
    return (int) (16384 * angle / (2 * M_PI));
}

float value16BitToRadians(int angle) {
    return 2 * M_PI * angle / 65536.0;
}

int radiansToValue16Bit(float angle) {
    return (int) (65536 * angle / (2 * M_PI));
}