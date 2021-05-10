#include "turret.h"
#include "pid.h"
#include "CAN_receive.h"
#include "math.h"

PIDProfile yawProfile;
PIDProfile pitchProfile;

extern CAN_HandleTypeDef hcan1;
extern CAN_HandleTypeDef hcan2;

Turret calculateTurret(int16_t yawAngle, int16_t pitchAngle) {
		yawProfile.kP = 0.5;
		yawProfile.kF = 0;
		pitchProfile.kP = 0;
		pitchProfile.kF = 0;

	
    int16_t yawPosition = get_pitch_gimbal_motor_measure_point()->ecd;
    int16_t pitchPosition = get_yaw_gimbal_motor_measure_point()->ecd;

    Turret turret;
    turret.yaw = calculateProportional(yawPosition, yawAngle, yawProfile);
    turret.pitch = calculateProportional(pitchPosition, pitchAngle, pitchProfile);

    return turret;
}