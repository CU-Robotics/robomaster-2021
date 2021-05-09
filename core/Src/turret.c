#include "turret.h"
#include "pid.h"
#include "CAN_receive.h"
#include "math.h"

PIDProfile yawProfile;
PIDProfile pitchProfile;

Turret calculateTurret(float yawAngle, float pitchAngle) {
		yawProfile.kP = 1 / (4 * 3.14159);
		pitchProfile.kP = 1 / (4 * 3.14159);
	
    float yawPosition = value16BitToRadians(get_yaw_gimbal_motor_measure_point()->ecd);
    float pitchPosition = value16BitToRadians(get_yaw_gimbal_motor_measure_point()->ecd);

    Turret turret;
    turret.yaw = radiansToValue14Bit(calculateProportional(yawPosition, yawAngle, yawProfile));
    turret.pitch = radiansToValue14Bit(calculateProportional(pitchPosition, pitchAngle, pitchProfile));

    return turret;
}