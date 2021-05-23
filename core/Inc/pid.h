#ifndef __pid_H
#define __pid_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "constants.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
typedef struct {
   float kP;
   float kI;
   float kD;
   float kF;
} PIDProfile;

typedef struct {
    float errorBuffer [M_PID_INTEGRAL_BUFFER_SIZE + 1];
    float lastError;
} PIDState;

// Calculates mecanum drive output state from controller input given an input in radians
float calculatePID(float currentPosition, float setpoint, PIDProfile profile, PIDState *state);

#ifdef __cplusplus
}
#endif
#endif
