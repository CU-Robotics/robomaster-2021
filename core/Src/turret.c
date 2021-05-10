#include "CAN_receive.h"
#include "math.h"

#include "constants.h"
#include "pid.h"

#include "turret.h"


void turretInit() {
    // PID Profiles containing tuning parameters.
    yawProfile.kP = 1 / (2 * M_PI);
    pitchProfile.kP = 1 / (2 * M_PI);
}

void turretLoop() {
    float yawSetpoint = (local_rc_ctrl->rc.ch[M_CONTROLLER_X_AXIS] / M_CONTROLLER_JOYSTICK_SCALE) * M_PI;
    float pitchSetpoint = (local_rc_ctrl->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE) * M_PI;

    Turret turret = calculateTurret(yawAngle, pitchAngle);

    CAN_cmd_gimbal(turret.yaw * M_MOTOR_GM6020_VOLTAGE_SCALE, 0, 0, 0);
}

Turret calculateTurret(float yawAngle, float pitchAngle) {
    float yawPosition = get_yaw_gimbal_motor_measure_point()->ecd);
    float pitchPosition = get_pitch_gimbal_motor_measure_point()->ecd);

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawProfile));
    turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchProfile));

    return turret;
}