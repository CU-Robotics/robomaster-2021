#ifndef __turret_H
#define __turret_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"

#include "main.h"
#include "pid.h"

// Stores power values for the two turret motors.
// Turret motor IDs are permanently set to 5 and 6, yaw and pitch respectively.
typedef struct {
   float yaw; // ID 5
   float pitch; // ID 6
} Turret;

void turretInit(void);
void turretLoop(const RC_ctrl_t* control_input);

// Calculates turret output state from desired pitch and yaw angles
Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile, PIDState *yawPIDState, PIDState *pitchPIDState);


#ifdef __cplusplus
}
#endif
#endif
