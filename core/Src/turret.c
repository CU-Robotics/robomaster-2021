#include "dma.h"
#include "usart.h"
#include "remote_control.h"
#include "can.h"
#include "CAN_receive.h"
#include "bsp_fric.h"

#include "constants.h"
#include "pid.h"
#include "utils.h"
#include "referee.h"
#include "jetson.h"

#include "turret.h"


PIDProfile yawProfile;
PIDProfile pitchProfile;
PIDState yawState;
PIDState pitchState;

int pitchRotations;
int prevPitchPosition;
int pitchOffset;

float yawSetpoint = 0;
float pitchSetpoint = 0;
float tempPitchPosition = 0;

void turretInit() {
  // PID Profiles containing tuning parameters.
  yawProfile.kP = 16.0f / (8 * M_PI);
  yawProfile.kI = 0.00025f / (8 * M_PI);
  yawProfile.kD = 4.0f / (8 * M_PI);

  pitchProfile.kP = 50.0f / (8 * M_PI);
  pitchProfile.kI = 0.0f / (8 * M_PI);
  pitchProfile.kD = 5.0f / (8 * M_PI);
  pitchProfile.kF = 4.20f / (8 * M_PI);

  // PID States
  yawState.lastError = 0;
  pitchState.lastError = 0;

  pitchRotations = 0;
  prevPitchPosition = 0;
	
	float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd);
	pitchOffset = pitchPosition;
}

void turretLoop(const RC_ctrl_t* control_input, int deltaTime) {
	yawSetpoint += control_input->rc.ch[2] * 1000;
	pitchSetpoint += control_input->rc.ch[3] * 1000;

	yawSetpoint += deltaTime * M_PI * (control_input->mouse.x / (M_MOUSE_X_SCALE));
	pitchSetpoint += deltaTime * M_PI * (control_input->mouse.y / (M_MOUSE_Y_SCALE));

	// Apply pitch soft limits
	/*if (pitchSetpoint < M_TURRET_PITCH_LOWER_LIMIT) {
		pitchSetpoint = M_TURRET_PITCH_LOWER_LIMIT;
	} else if (pitchSetpoint > M_TURRET_PITCH_UPPER_LIMIT) {
		pitchSetpoint = M_TURRET_PITCH_UPPER_LIMIT;
	}*/

	// Keep yaw setpoints in -2pi to 2pi range for stability
	if (yawSetpoint > 2 * M_PI) {
		yawSetpoint -= 2 * M_PI;
	} else if (yawSetpoint < -2 * M_PI) {
		yawSetpoint += 2 * M_PI;
	}

	Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile, &yawState, &pitchState);

	// Shooter Code
	float feederSpeed = 0.0f;
	
	// Fire
	if (control_input->mouse.press_l) {
		fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET + (M_SNAIL_SPEED_SCALE * M_SHOOTER_CURRENT_PERCENT)));
		feederSpeed = -M_M2006_CURRENT_SCALE * CONF_SHOOTER_FIRERATE_BURST;
	// Prespin
	} else if(control_input->mouse.press_r){
		fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET + (M_SNAIL_SPEED_SCALE * M_SHOOTER_CURRENT_PERCENT)));
	// Unjam
	} else {
		fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET));
	}

	if (get_pitch_gimbal_motor_measure_point()->temperate > M_TURRET_PITCH_TEMP_LIMIT) {
		turret.pitch = 0.0f;
	}

	// Set motor output values 
	CAN_cmd_gimbal_working(turret.yaw * M_GM6020_VOLTAGE_SCALE, turret.pitch * M_M3508_CURRENT_SCALE, feederSpeed, 0);
	//CAN_cmd_yaw(turret.yaw * M_GM6020_VOLTAGE_SCALE);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile, PIDState *yawPIDState, PIDState *pitchPIDState) {
  // Captures positions in encoders native format
  float yawPosition = (get_yaw_gimbal_motor_measure_point()->ecd);
  float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd);

  // Keeps track of true position including reduction
  pitchRotations += countRotationsM3508(pitchPosition, prevPitchPosition);
  prevPitchPosition = pitchPosition;
  pitchPosition = (pitchPosition + pitchRotations * M_M3508_ENCODER_SCALE - pitchOffset) * M_M3508_REDUCTION_RATIO;
  tempPitchPosition = (2.0f * M_PI * pitchPosition) / M_M3508_ENCODER_SCALE;

  // Calculate turret thrusts using PID with input of radians
  Turret turret;
  turret.yaw = calculatePID_Positional((2.0f * M_PI * yawPosition) / M_GM6020_ENCODER_SCALE, yawAngle, yawPIDProfile, yawPIDState);
  turret.pitch = calculatePID_SinFeedforward((2.0f * M_PI * pitchPosition) / M_M3508_ENCODER_SCALE, pitchAngle, pitchPIDProfile, pitchPIDState);

  return turret;
}
