#include "pid.h"
#include "math.h"
#include "utils.h"
#include "constants.h"

float calculateProportional(float currentPosition, float setpoint, PIDProfile profile, PIDState *state) {
	float error;

	// Calculate error
	if (setpoint > currentPosition) {
		if (absValueFloat(setpoint-currentPosition) < absValueFloat(-2*M_PI + setpoint - currentPosition)) 
			error = setpoint - currentPosition;
		else  
			error = -2 * M_PI - setpoint + currentPosition;
	} else {
		if (absValueFloat(setpoint-currentPosition)< absValueFloat(2*M_PI - currentPosition + setpoint)) 
			error = setpoint - currentPosition;
		else  
			error = 2 * M_PI + setpoint - currentPosition;
	}
	
	//Calculate derivative
	float derivative = error - state->lastError;
	
	//Calculate integral
	float integral = error;
	for(int i = 0; i < 100; i++){
		state->errorBuffer[i] = state->errorBuffer[i+1];
		integral += state->errorBuffer[i];
	}
	state->errorBuffer[100] = error;
		
	// Calculate correction and return
    return error * profile.kP + derivative * profile.kD + integral * profile.kI + profile.kF;
}

