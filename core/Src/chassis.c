#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"

#include "constants.h"

#include "chassis.h"


void chassisInit() {

}

void chassisLoop(RC_ctrl_t* control_input) {
	float xThrottle = (control_input->rc.ch[M_CONTROLLER_X_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
    float yThrottle = (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
    float rotation = (control_input->rc.ch[M_CONTROLLER_ROTATION_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);

    Chassis chassis = calculateMecanum(xThrottle, yThrottle, rotation);

    CAN_cmd_chassis((int16_t) (chassis.frontRight), (int16_t) (chassis.backRight), (int16_t) (chassis.backLeft), (int16_t) (chassis.frontLeft));
}

Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle) {
	// Calculate the power for each wheel (on a scale of -1.0 to 1.0)
	float frontRight = xThrottle - yThrottle - rotationThrottle;
	float frontLeft = xThrottle + yThrottle + rotationThrottle;
	float backRight = xThrottle + yThrottle - rotationThrottle;
	float backLeft = xThrottle - yThrottle + rotationThrottle;

	// Find the highest magnitude (max) value in the four power variables.
	float max = abs(frontRight);
	if (abs(frontLeft) > max) max = abs(frontLeft);
	if (abs(backRight) > max) max = abs(backRight);
	if (abs(backLeft) > max) max = abs(backLeft);

	// If the max value is greater than 1.0, divide all the motor power variables by the max value, forcing all magnitudes to be less than or equal to 1.0.
	if (max > 1.0) {
		frontRight /= -max;
		frontLeft /= max;
		backRight /= -max;
		backLeft /= max;
	}

	// Invert right side to make "forward" consistent for all motors.
	// Scales the values for the voltage, now ready for CAN transmition
	Chassis chassis;
	chassis.frontRight = -frontRight * M_MOTOR_M3508_CURRENT_SCALE;
	chassis.frontLeft = frontLeft * M_MOTOR_M3508_CURRENT_SCALE;
	chassis.backRight = -backRight * M_MOTOR_M3508_CURRENT_SCALE;
	chassis.backLeft = backLeft * M_MOTOR_M3508_CURRENT_SCALE;

	return chassis;
}
