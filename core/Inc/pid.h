#ifndef __pid_H
#define __pid_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "constants.h"


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

float calculatePID_Positional(float currentPosition, float setpoint, PIDProfile profile, PIDState *state);

float calculatePID_SinFeedforward(float currentPosition, float setpoint, PIDProfile profile, PIDState *state);

float calculatePID_Speed(float currentPosition, float setpoint, PIDProfile profile, PIDState *state);


#ifdef __cplusplus
}
#endif
#endif
