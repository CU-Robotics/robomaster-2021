#include "pid.h"
#include "math.h"

int16_t calculateProportional(int16_t currentPosition, int16_t setpoint, PIDProfile profile) {
    // Calculate error
    int16_t error = setpoint - currentPosition;
    // Calculate correction and return
    return error * profile.kP + profile.kF;
}

