#ifndef __control_pid_H
#define __control_pid_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
struct PIDProfile {
   float kP = 0.0;
   float kI = 0.0;
   float kD = 0.0;
   float kF = 0.0;
};

struct PIDState {
    float integralSum;
};

// Calculates mecanum drive output state from controller input
float calculateProportional(int currentPosition, float setpoint, PIDProfile profile, double deltaTime);

float encoder14BitToRadians(int angle);
int radiansToEncoder14Bit(float angle);

float value16BitToRadians(int angle);
int radiansToValue16Bit(float angle);

#ifdef __cplusplus
}
#endif
#endif