#ifndef __subsystems_chassis_H
#define __subsystems_chassis_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
#include "pid.h"


// PID Profiles containing tuning parameters.
PIDProfile yawProfile;
PIDProfile pitchProfile;

// Stores power values for the two turret motors.
// Turret motor IDs are permanently set to 5 and 6, yaw and pitch respectively.
struct Turret {
   float yaw; // ID 5
   float pitch; // ID 6
};

// Calculates turret output state from desired pitch and yaw angles
Turret calculateTurret(float yawAngle, float pitchAngle);


#ifdef __cplusplus
}
#endif
#endif