#ifndef __control_pid_H
#define __control_pid_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
struct PIDProfile {
   float kP;
   float kI;
   float kD;
   float kF;
};

struct PIDState {
    float sum;
};

// Calculates mecanum drive output state from controller input
float calculateProportional(float currentPosition, float setpoint, PIDProfile profile, double deltaTime);


#ifdef __cplusplus
}
#endif
#endif