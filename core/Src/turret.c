#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"
#include "bsp_fric.h"

#include "constants.h"
#include "pid.h"

#include "turret.h"

PIDProfile yawProfile;
PIDProfile pitchProfile;
PIDState yawState;
PIDState pitchState;

void turretInit() {
    // PID Profiles containing tuning parameters.
    yawProfile.kP = 16.0f / (8 * M_PI);
    pitchProfile.kP = 4.0f / (8 * M_PI);

    yawProfile.kI = 0.00025f / (8 * M_PI);
    pitchProfile.kI = 0.01f / (8 * M_PI);
	
    yawProfile.kD = 2.0f / (8 * M_PI);
    pitchProfile.kD = 0.0f / (8 * M_PI);
	
		//PID States
		yawState.integralSum = 0;
		pitchState.integralSum = 0;
		
		yawState.lastError = 0;
		pitchState.lastError = 0;
}

void turretLoop(RC_ctrl_t* control_input) {
    float yawSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_X_AXIS] / (M_CONTROLLER_JOYSTICK_SCALE));
    float pitchSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);

    Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile, &yawState, &pitchState);

    if(control_input->rc.s[0] == 1) {
      //run snail motor
			fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET + M_MOTOR_SNAIL_MAX));
    } else {
			//turn off snail motor
			fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET));
		}

		int16_t ballFeedSpeed = control_input->rc.ch[M_CONTROLLER_X_AXIS];
    //CAN_cmd_gimbal(0, 0, ballFeedSpeed, 0);
    CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_VOLTAGE_SCALE, turret.pitch * M_MOTOR_M3508_VOLTAGE_SCALE, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile, PIDState *yawPIDState, PIDState *pitchPIDState) {
    float ecdVal = get_yaw_gimbal_motor_measure_point()->ecd;
    float yawPosition = 2.0 * M_PI * (get_yaw_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);
    float pitchPosition = 2.0 * M_PI * (get_pitch_gimbal_motor_measure_point()->ecd / M_ENCODER_M3508_SCALE);
	
		//crude mapping of pitch motor
		/*
		pitchPosition = pitchPosition* M3508_REDUCTION_RATIO;
		while(pitchPosition >= 2.0 * M_PI)
			pitchPosition = pitchPosition - 2.0 * M_PI;
		while(pitchPosition <= -2.0 * M_PI)
			pitchPosition = pitchPosition + 2.0 * M_PI;
		*/
	
    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawPIDProfile, yawPIDState);
    turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchPIDProfile, pitchPIDState);

    return turret;
}