#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"

#include "constants.h"
#include "pid.h"

#include "turret.h"


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
    float switchValue = control_input->rc.s[M_MOTOR_SNAIL_SWITCH_STATES]; // 1 2 or 3

    if(switchValue == control_input->rc.s[M_MOTOR_SNAIL_Z]) {
        //if switchValue = 1, input max value to CAN_cmd
 CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_CURRENT_SCALE, 0, M_MOTOR_SNAIL_MAX, 0);
        //else, call function with 0 as input value for snail motor
        else{
             CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_CURRENT_SCALE, 0, 0, 0);
        }
    }


    CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_CURRENT_SCALE, 0, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile) {
    float ecdVal = get_yaw_gimbal_motor_measure_point()->ecd;
    float yawPosition = 2.0 * M_PI * (get_yaw_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);
    float pitchPosition = 2.0 * M_PI * (get_pitch_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawPIDProfile);
    //turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchPIDProfile);

    return turret;
}
