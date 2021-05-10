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

    CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_VOLTAGE_SCALE, 0, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile) {
    float ecdVal = get_yaw_gimbal_motor_measure_point()->ecd;
    float yawPosition = 2.0 * M_PI * (get_yaw_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);
    float pitchPosition = 2.0 * M_PI * (get_pitch_gimbal_motor_measure_point()->ecd / M_ENCODER_GM6020_SCALE);

    CAN_cmd_chassis(0, 0, get_yaw_gimbal_motor_measure_point()->ecd, 0);

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawPIDProfile);
    //turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchPIDProfile);

    return turret;
}