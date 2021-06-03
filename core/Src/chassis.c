#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"

#include "pid.h"
#include "utils.h"
#include "constants.h"

#include "chassis.h"

PIDProfile profile;
PIDState frontRightState;
PIDState backRightState;
PIDState backLeftState;
PIDState frontLeftState;

void chassisInit() {
    // PID Profiles containing tuning parameters.
    profile.kP = 0.10f;
    profile.kI = 0.00f;
    profile.kD = 0.0f;

	frontRightState.lastError = 0;
    backRightState.lastError = 0;
	backLeftState.lastError = 0;
	frontLeftState.lastError = 0;
}

void chassisLoop(const RC_ctrl_t* control_input) {
	float xThrottle = (control_input->rc.ch[M_CONTROLLER_X_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
    float yThrottle = (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
    float rotation = (control_input->rc.ch[M_CONTROLLER_ROTATION_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);

    Chassis chassis = calculateMecanum(xThrottle, yThrottle, rotation);
	chassis.frontRight = calculatePID(get_chassis_motor_measure_point(1)->speed_rpm * M_M3508_REDUCTION_RATIO, chassis.frontRight * M_CHASSIS_MAX_RPM, profile, &frontRightState);
	chassis.backRight = calculatePID(get_chassis_motor_measure_point(2)->speed_rpm * M_M3508_REDUCTION_RATIO, chassis.backRight * M_CHASSIS_MAX_RPM * (1 / M_M3508_REDUCTION_RATIO), profile, &backRightState);
	chassis.backLeft = calculatePID(get_chassis_motor_measure_point(3)->speed_rpm * M_M3508_REDUCTION_RATIO, chassis.backLeft * M_CHASSIS_MAX_RPM * (1 / M_M3508_REDUCTION_RATIO), profile, &backLeftState);
	chassis.frontLeft = calculatePID(get_chassis_motor_measure_point(4)->speed_rpm * M_M3508_REDUCTION_RATIO, chassis.frontLeft * M_CHASSIS_MAX_RPM * (1 / M_M3508_REDUCTION_RATIO), profile, &frontLeftState);

    CAN_cmd_chassis((int16_t) (chassis.frontRight * M_M3508_CURRENT_SCALE), (int16_t) (chassis.backRight * M_M3508_CURRENT_SCALE), (int16_t) (chassis.backLeft * M_M3508_CURRENT_SCALE), (int16_t) (chassis.frontLeft * M_M3508_CURRENT_SCALE));
}

Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle) {
	// Calculate the power for each wheel (on a scale of -1.0 to 1.0)
	float frontRight = xThrottle - yThrottle - rotationThrottle;
	float frontLeft = xThrottle + yThrottle + rotationThrottle;
	float backRight = xThrottle + yThrottle - rotationThrottle;
	float backLeft = xThrottle - yThrottle + rotationThrottle;

	// Find the highest magnitude (max) value in the four power variables.
	float max = absValueFloat(frontRight);
	if (absValueFloat(frontLeft) > max) max = absValueFloat(frontLeft);
	if (absValueFloat(backRight) > max) max = absValueFloat(backRight);
	if (absValueFloat(backLeft) > max) max = absValueFloat(backLeft);

	// If the max value is greater than 1.0, divide all the motor power variables by the max value, forcing all magnitudes to be less than or equal to 1.0.
	if (max > 1.0f) {
		frontRight /= -max;
		frontLeft /= max;
		backRight /= -max;
		backLeft /= max;
	}

	// Invert right side to make "forward" consistent for all motors.
	Chassis chassis;
	chassis.frontRight = -frontRight;
	chassis.frontLeft = frontLeft;
	chassis.backRight = -backRight;
	chassis.backLeft = backLeft;

	return chassis;
}
