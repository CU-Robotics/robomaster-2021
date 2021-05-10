#include "pid.h"
#include "math.h"
#include "constants.h"

float calculateProportional(float currentPosition, float setpoint, PIDProfile profile) {
    // Calculate error
    float error = setpoint - currentPosition;
    float error2 = M_ENCODER_GM6020_SCALE + error;
	
<<<<<<< HEAD
    if (abs(error2) < (error)){
        error = error2;
    }
=======
		if(abs(error2) < abs(error)){
			error = error2;
		}
>>>>>>> bc3490223c17b24004b727bec086b483b1fa7b87
    // Calculate correction and return
    return error * profile.kP + profile.kF;
}