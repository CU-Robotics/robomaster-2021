#ifndef __pid_H
#define __pid_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
typedef struct {
   float kP;
   float kI;
   float kD;
   float kF;
} PIDProfile;

typedef struct {
    float integralSum;
} PIDState;

// Calculates mecanum drive output state from controller input
float calculateProportional(int currentPosition, int setpoint, PIDProfile profile);

float encoder14BitToRadians(int angle);
int radiansToEncoder14Bit(float angle);

float value16BitToRadians(int angle);
int radiansToValue16Bit(float angle);

#ifdef __cplusplus
}
#endif
#endif