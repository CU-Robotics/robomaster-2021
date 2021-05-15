#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"
#include "bsp_fric.h"

#include "constants.h"
#include "pid.h"

#include "turret.h"

float ecdVal = 3000;
PIDProfile yawProfile;
PIDProfile pitchProfile;

void turretInit() {
    // PID Profiles containing tuning parameters.
    yawProfile.kP = 1.0f / (8 * M_PI);
    pitchProfile.kP = 1.0f / (8 * M_PI);
}

void turretLoop(RC_ctrl_t* control_input) {
    float yawSetpoint = 2.0 * M_PI * (control_input->rc.ch[M_CONTROLLER_X_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
    float pitchSetpoint = 2.0 * M_PI * (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);

    Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile);


    // If else statement for shooting turret:
    int switchValue = control_input->rc.s[M_MOTOR_SNAIL_SWITCH]; // 1 2 or 3

    if(switchValue == M_MOTOR_SNAIL_Z) {
        //run snail motor
			fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET + M_MOTOR_SNAIL_MAX));
    }
		else{
			//turn off snail motor
			fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET));
		}

		int16_t ballFeedSpeed = control_input->rc.ch[M_CONTROLLER_X_AXIS];
    //CAN_cmd_gimbal(0, 0, ballFeedSpeed, 0);
    CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_VOLTAGE_SCALE, 0, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile) {
    ecdVal = get_yaw_gimbal_motor_measure_point()->ecd;
    float yawPosition = 2.0 * M_PI * (get_yaw_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);
    float pitchPosition = 2.0 * M_PI * (get_pitch_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawPIDProfile);
    //turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchPIDProfile);

    return turret;
}