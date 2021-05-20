#ifndef __constants_H
#define __constants_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

// -- MATH --
#define M_PI = 3.1415926f;

// -- CONTROLS --
#define M_CONTROLLER_JOYSTICK_SCALE = 660.0f;
#define M_CONTROLLER_SWITCH_STATES[] = {0, 1, 2};
#define M_CONTROLLER_X_AXIS = 2;
#define M_CONTROLLER_Y_AXIS = 3;
#define M_CONTROLLER_ROTATION_AXIS = 1;

// -- MOTORS --
// M3508
#define M_M3508_CURRENT_SCALE = 16384.0f; 
#define M_M3508_ENCODER_SCALE = 8191.0f;
#define M_M3508_REDUCTION_RATIO = 3591.0f / 187.0f;
// M2006
#define M_M2006_CURRENT_SCALE = 16384.0f; 
#define M_M2006_ENCODER_SCALE = 8191.0f;
#define M_M2006_REDUCTION_RATIO = 36.0f;
// GM6020
#define M_GM6020_VOLTAGE_SCALE = 30000.0f;
#define M_GM6020_ENCODER_SCALE = 8191.0f;
// Snail
#define M_SNAIL_SPEED_SCALE = 1000.0f;
#define M_SNAIL_SPEED_OFFSET = 1000.0f;

// -- MISC --
#define M_MASTER_LOOP_PERIOD = 1.0f; // Code runs at 1000Hz (1ms period)
#define M_ZERO_HARDSTOP_TIME_THRESHOLD = 50.0f; // Motor must be held stationary for at least 50ms to detect a hardstop
#define M_ZERO_HARDSTOP_RANGE_THRESHOLD = M_PI / 128.0f // All encoder values must be this close to each other in order to count as stationary

#ifdef __cplusplus
}
#endif
#endif