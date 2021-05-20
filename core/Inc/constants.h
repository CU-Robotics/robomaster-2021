#ifndef __constants_H
#define __constants_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

// Mathematical Constants
extern const float M_PI;

// Controller
extern const float M_CONTROLLER_JOYSTICK_SCALE;
extern const int M_CONTROLLER_SWITCH_STATES[];
extern const int M_CONTROLLER_X_AXIS;
extern const int M_CONTROLLER_Y_AXIS;
extern const int M_CONTROLLER_ROTATION_AXIS;

// M3508 Motor
extern const float M_MOTOR_M3508_VOLTAGE_SCALE;
extern const float M_ENCODER_M3508_SCALE;
extern const float M3508_REDUCTION_RATIO;

// M2006 Motor
extern const float M_MOTOR_M2006_VOLTAGE_SCALE; 
extern const float M_ENCODER_M2006_SCALE;
extern const float M2006_REDUCTION_RATIO;

// GM6020 Motor
extern const float M_MOTOR_GM6020_VOLTAGE_SCALE;
extern const float M_ENCODER_GM6020_SCALE;

//SNAIL Motor
const float M_MOTOR_SNAIL_MAX;
const float M_MOTOR_SNAIL_OFFSET;

#ifdef __cplusplus
}
#endif
#endif