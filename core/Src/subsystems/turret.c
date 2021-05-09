#include "turret.h"
#include "pid.h"
#include "CAN_receive.h"
#include "math.h"

yawProfile.kP = 1 / (4 * M_PI);
pitchProfile.kP = 1 / (4 * M_PI);

Turret calculateTurret(float yawAngle, float pitchAngle) {
    float yawPosition = value16BitToRadians(get_yaw_gimbal_motor_measure_point()->ecd);
    float pitchPosition = value16BitToRadians(get_yaw_gimbal_motor_measure_point()->ecd);

    Turret turret;
    turret.yaw = radiansToValue14Bit(calculateProportional(yawPosition, yawAngle, yawProfile));
    turret.pitch = radiansToValue14Bit(calculateProportional(pitchPosition, pitchAngle, pitchProfile));

    return turret;
}