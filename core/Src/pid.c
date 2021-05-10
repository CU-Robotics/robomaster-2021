#include "pid.h"
#include "math.h"

float calculateProportional(float currentPosition, float setpoint, PIDProfile profile) {
    // Calculate error
    float error = setpoint - currentPosition;
    // Calculate correction and return
    return error * 3.14 + profile.kF;
}