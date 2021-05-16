#include "pid.h"
#include "math.h"
#include "constants.h"

    float error; 
float calculateProportional(float currentPosition, float setpoint, PIDProfile profile) {
    // Calculate error
		if (setpoint>currentPosition) {
				 if (absValueFloat(setpoint-currentPosition) < absValueFloat(-2*M_PI + setpoint - currentPosition)) 
					 error = setpoint - currentPosition;
				 else  
					 error = -2*M_PI - setpoint + currentPosition;
		}
		else{
				 if(absValueFloat(setpoint-currentPosition)< absValueFloat(2*M_PI - currentPosition + setpoint)) 
					 error = setpoint - currentPosition;
				 else  
					 error = 2*M_PI + setpoint - currentPosition;
		}
	  // Calculate correction and return
    return error * profile.kP + profile.kF;
}

float absValueFloat(float value) {
    if (value < 0) {
        return -value;
    }
    return value;
}