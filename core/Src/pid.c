#include "pid.h"
#include "math.h"
#include "constants.h"

float calculateProportional(float currentPosition, float setpoint, PIDProfile profile, PIDState *state) {
	float error;

    // Calculate error
	if (setpoint > currentPosition) {
		if (absValueFloat(setpoint-currentPosition) < absValueFloat(-2*M_PI + setpoint - currentPosition)) 
			error = setpoint - currentPosition;
		else  
			error = -2*M_PI - setpoint + currentPosition;
	} else {
		if (absValueFloat(setpoint-currentPosition)< absValueFloat(2*M_PI - currentPosition + setpoint)) 
			error = setpoint - currentPosition;
		else  
			error = 2*M_PI + setpoint - currentPosition;
	}
	
	//Calculate derivative
	float derivative = error - state->lastError;
	
	//Calculate integral
	float integral = (state->integralSum += error);
		
	// Calculate correction and return
    return error * profile.kP + derivative * profile.kD + integral * profile.kI + profile.kF;
}

float absValueFloat(float value) {
    if (value < 0) {
        return -value;
    }
    return value;
}