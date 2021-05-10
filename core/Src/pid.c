#include "pid.h"
#include "math.h"
#include "constants.h"

float calculateProportional(float currentPosition, float setpoint, PIDProfile profile) {
    // Calculate error
    float error = setpoint - currentPosition;
    float error2 = M_ENCODER_GM6020_SCALE - error;
	
		if(error2 > error){
			error = error2;
		}
    // Calculate correction and return
    return error * profile.kP + profile.kF;
}