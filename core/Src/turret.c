#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"
#include "bsp_fric.h"

#include "constants.h"
#include "pid.h"
#include "utils.h"

#include "turret.h"

PIDProfile yawProfile;
PIDProfile pitchProfile;
PIDState yawState;
PIDState pitchState;

bool zeroed;
int pitchRotations;
int prevPitchPosition;

void turretInit() {
    // PID Profiles containing tuning parameters.
    yawProfile.kP = 16.0f / (8 * M_PI);
    pitchProfile.kP = 4.0f / (8 * M_PI);

    yawProfile.kI = 0.00025f / (8 * M_PI);
    pitchProfile.kI = 0.01f / (8 * M_PI);
	
    yawProfile.kD = 2.0f / (8 * M_PI);
    pitchProfile.kD = 0.0f / (8 * M_PI);
	
		//PID States
		
		yawState.lastError = 0;
		pitchState.lastError = 0;

    zeroed = false;
    pitchRotations = 0;
    prevPitchPosition = 0;
}

void turretLoop(const RC_ctrl_t* control_input) {
    if (zeroed) {
        float yawSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_X_AXIS] / (M_CONTROLLER_JOYSTICK_SCALE));
        float pitchSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);

        Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile, &yawState, &pitchState);

        /*if(control_input->rc.s[0] == 1) {
            // Run snail motor
            fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET + M_MOTOR_SNAIL_MAX));
        } else {
            // Turn off snail motor
            fric_on((uint16_t)(M_MOTOR_SNAIL_OFFSET));
        }*/

        //int16_t ballFeedSpeed = control_input->rc.ch[M_CONTROLLER_X_AXIS];

        CAN_cmd_gimbal_working(turret.yaw * M_GM6020_VOLTAGE_SCALE, turret.pitch * M_M3508_CURRENT_SCALE, 0, 0);
    } else {
        
    }
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile, PIDState *yawPIDState, PIDState *pitchPIDState) {
    float yawPosition = (get_yaw_gimbal_motor_measure_point()->ecd / M_GM6020_ENCODER_SCALE);
    float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd / M_M3508_ENCODER_SCALE);

    pitchRotations += countRotationsM3508(pitchPosition, prevPitchPosition);
    prevPitchPosition = pitchPosition;
    pitchPosition = M_M3508_REDUCTION_RATIO * (pitchPosition + pitchRotations * M_M3508_ENCODER_SCALE);

    Turret turret;
    turret.yaw = calculateProportional(2.0f * M_PI * yawPosition, yawAngle, yawPIDProfile, yawPIDState);
    turret.pitch = calculateProportional(2.0f * M_PI * pitchPosition, pitchAngle, pitchPIDProfile, pitchPIDState);

    return turret;
}
