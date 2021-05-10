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
    yawProfile.kP = 1 / (2 * M_PI);
    pitchProfile.kP = 1 / (2 * M_PI);
}

void turretLoop(RC_ctrl_t* control_input) {
    float yawSetpoint = (control_input->rc.ch[M_CONTROLLER_X_AXIS] / M_CONTROLLER_JOYSTICK_SCALE) * M_PI;
    float pitchSetpoint = (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE) * M_PI;

    Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile);

    CAN_cmd_gimbal_working(turret.yaw * M_MOTOR_GM6020_VOLTAGE_SCALE, 0, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile) {
    float yawPosition = get_yaw_gimbal_motor_measure_point()->ecd;
    float pitchPosition = get_pitch_gimbal_motor_measure_point()->ecd;

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawPIDProfile);
    turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchPIDProfile);

    return turret;
}