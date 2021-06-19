#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "math.h"
#include "CAN_receive.h"

#include "pid.h"
#include "utils.h"
#include "referee.h"
#include "constants.h"

#include "chassis.h"

PIDProfile chassisPID_Profile;
PIDState frontRightState;
PIDState backRightState;
PIDState backLeftState;
PIDState frontLeftState;

float xThrottle = 0;
float yThrottle = 0;
float rotation = 0;

float chassisSpeed = 1.0f;

void chassisInit() {
    // PID Profiles containing tuning parameters.
    chassisPID_Profile.kP = 0.0003f; // 0.0008f This is overtuned
    chassisPID_Profile.kI = 0.000000f; // 0.000001f This is overtuned
    chassisPID_Profile.kD = 0.0001f;

	frontRightState.lastError = 0;
    backRightState.lastError = 0;
	backLeftState.lastError = 0;
	frontLeftState.lastError = 0;
}

void chassisLoop(const RC_ctrl_t* control_input, int deltaTime) {
	//Collect Controller input
	if (control_input->key.v & M_SHIFT_BITMASK){
		chassisSpeed = CONF_SLOW_WALK_SPEED;
	} else{
		chassisSpeed = CONF_FULL_SPEED;
	}
	// Turret-Relative Movement
	float yawPosition = (get_yaw_gimbal_motor_measure_point()->ecd);
	float relativeAngle = (2.0f * M_PI * yawPosition) / M_GM6020_ENCODER_SCALE;

	// Collect Controller Input
	xThrottle = control_input->rc.ch[1] * 1000;
	yThrottle = control_input->rc.ch[0] * 1000;
	
	if (control_input->key.v & M_W_BITMASK) {
		xThrottle += 1.0f;//chassisSpeed*cos(relativeAngle);
	}
	if (control_input->key.v & M_S_BITMASK) {
		xThrottle -= 1.0f;//chassisSpeed*cos(relativeAngle);
	}
	if (control_input->key.v & M_A_BITMASK) {
		yThrottle -= M_HORIZONTAL_MULTIPLIER*1.0f;//chassisSpeed*sin(relativeAngle);
	}
	if (control_input->key.v & M_D_BITMASK) {
		yThrottle += M_HORIZONTAL_MULTIPLIER*1.0f;//chassisSpeed*sin(relativeAngle);

	}
	
	rotation = control_input->rc.ch[4] * 1000;
	if (control_input->key.v & M_Q_BITMASK) {
		rotation -= 1.0f;//chassisSpeed;
	}
	if (control_input->key.v & M_E_BITMASK) {	
		rotation += 1.0f;//chassisSpeed;
	}

	// Calculate mecanum wheel velocities for target vector
 	Chassis chassis = calculateMecanum(xThrottle, yThrottle, rotation);
	
	// Convert read RPM (before gearing) to true output RPM
	float speed_1 = get_chassis_motor_measure_point(1)->speed_rpm * M_M3508_REDUCTION_RATIO;
	float speed_2 = get_chassis_motor_measure_point(2)->speed_rpm * M_M3508_REDUCTION_RATIO;
	float speed_3 = get_chassis_motor_measure_point(3)->speed_rpm * M_M3508_REDUCTION_RATIO;
	float speed_4 = get_chassis_motor_measure_point(4)->speed_rpm * M_M3508_REDUCTION_RATIO;
	
	// Calculate PID based on current state, and desired wheel velocities
	chassis.frontRight = calculatePID_Speed(speed_1, chassis.frontRight * M_CHASSIS_MAX_RPM, chassisPID_Profile, &frontRightState);
	chassis.backRight = calculatePID_Speed(speed_2, chassis.backRight * M_CHASSIS_MAX_RPM, chassisPID_Profile, &backRightState);
	chassis.backLeft = calculatePID_Speed(speed_3, chassis.backLeft * M_CHASSIS_MAX_RPM, chassisPID_Profile, &backLeftState);
	chassis.frontLeft = calculatePID_Speed(speed_4, chassis.frontLeft * M_CHASSIS_MAX_RPM, chassisPID_Profile, &frontLeftState);

	/*
	//Read robot power limit in watts? from ref struct
	uint16_t powerLimit = refData.REF_robot_status.chassis_power_limit;
	
	//Get real power usage, could be used
	//float realWattage = refData.REF_heat_data.chassis_power;
	
	//Calculate chassis power usage in watts
	fp32 totalAmperage = M_M3508_CURRENT_MAX*(absValueFloat(chassis.frontRight) + absValueFloat(chassis.backRight) + absValueFloat(chassis.frontLeft) + absValueFloat(chassis.backLeft));
	fp32 totalWattage = totalAmperage* M_BATTERY_VOLTAGE;
	//add 5% buffer to usage
	totalWattage *= 1.05f;
	
	//reduce each motor by scaling factor
	chassis.frontRight = chassis.frontRight * (fp32) powerLimit / totalWattage;
	chassis.backRight = chassis.backRight * (fp32) powerLimit / totalWattage;
	chassis.backLeft = chassis.backLeft * (fp32) powerLimit / totalWattage;
	chassis.frontLeft = chassis.frontLeft * (fp32) powerLimit / totalWattage;
	*/
	
 	CAN_cmd_chassis((int16_t) (M_MOTOR_1_MULTIPLIER*chassis.frontRight * M_M3508_CURRENT_SCALE), (int16_t) (M_MOTOR_2_MULTIPLIER*chassis.backRight * M_M3508_CURRENT_SCALE), (int16_t) (M_MOTOR_3_MULTIPLIER*chassis.backLeft * M_M3508_CURRENT_SCALE), (int16_t) (M_MOTOR_4_MULTIPLIER*chassis.frontLeft * M_M3508_CURRENT_SCALE));
}

Chassis calculateMecanum(float xThrottle, float yThrottle, float rotationThrottle) {
	// Calculate the power for each wheel
	float frontRight = xThrottle - yThrottle - rotationThrottle;
	float frontLeft = xThrottle + yThrottle + rotationThrottle;
	float backRight = xThrottle + yThrottle - rotationThrottle;
	float backLeft = xThrottle - yThrottle + rotationThrottle;

	// Find the highest magnitude (max) value in the four power variables.
	float max = absValueFloat(frontRight);
	if (absValueFloat(frontLeft) > max) max = absValueFloat(frontLeft);
	if (absValueFloat(backRight) > max) max = absValueFloat(backRight);
	if (absValueFloat(backLeft) > max) max = absValueFloat(backLeft);

	// Normalize power to scale from -1 to 1
	// If the max value is greater than 1.0, divide all the motor power variables by the max value, forcing all magnitudes to be less than or equal to 1.0.
	if (max > 1.0f) {
		frontRight /= max;
		frontLeft /= max;
		backRight /= max;
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
