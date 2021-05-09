#ifndef __subsystems_chassis_H
#define __subsystems_chassis_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"


// Stores power values for the four chassis motors.
// Chassis motor IDs are permanently set to 1-4, starting with the front right motor and going clockwise.
struct Chassis {
   float frontRight;
   float backRight;
   float backLeft;
   float frontLeft;
};

// Calculates mecanum drive output state from controller input
Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle);


#ifdef __cplusplus
}
#endif
#endif