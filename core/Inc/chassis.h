#ifndef __chassis_H
#define __chassis_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"

#include "main.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
typedef struct {
   float frontRight; // ID 1
   float backRight; // ID 2
   float backLeft; // ID 3
   float frontLeft; // ID 4
} Chassis;

void chassisInit(void);
void chassisLoop(const RC_ctrl_t* control_input);

// Calculates mecanum drive output state from controller input
Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle);


#ifdef __cplusplus
}
#endif
#endif
