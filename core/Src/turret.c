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
#include "BMI088Driver.h"

#include "turret.h"
fp32 gyro[3], accel[3], temp;
fp32 prevGyroVertical;

PIDProfile yawProfile;
PIDProfile pitchProfile;
PIDProfile feederProfile;
PIDState yawState;
PIDState pitchState;
PIDState feederState;

int pitchRotations;
int prevPitchPosition;
int pitchOffset;

float yawSetpoint = 0;
float pitchSetpoint = 0;
float tempPitchPosition = 0;

int flywheelSpeed = 0;
	
bool oddLoop;
bool idleOn;

bool idleDebounce;

void turretInit() {
  // PID Profiles containing tuning parameters.
  yawProfile.kP = 5.0f;
  yawProfile.kI = 0.00001f;
  yawProfile.kD = 1.0f;

  pitchProfile.kP = 35.0f / (8 * M_PI);
  pitchProfile.kI = 0.00005f / (8 * M_PI);
  pitchProfile.kD = 15.0f / (8 * M_PI);
  pitchProfile.kF = 4.20f / (8 * M_PI);

	feederProfile.kP = 0.1f;
	feederProfile.kI = 0.01f;
	feederProfile.kD = 0.0f;
	
	
  // PID States
  yawState.lastError = 0;
  pitchState.lastError = 0;
	feederState.lastError = 0;

  pitchRotations = 0;
  prevPitchPosition = 0;
	

	float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd);
	pitchOffset = pitchPosition;
	
	oddLoop = false;
	idleOn = 0;
	idleDebounce = 0;
	
	prevGyroVertical = 0;
}

void turretLoop(const RC_ctrl_t* control_input, int deltaTime) {
	
	// can re-initialization
	if(control_input->key.v & M_F_BITMASK){
		MX_CAN1_Init();
		MX_CAN2_Init();
	}
	
	oddLoop = !oddLoop;
	
	yawSetpoint += control_input->rc.ch[2] * 1000;
	pitchSetpoint += control_input->rc.ch[3] * 1000;

	yawSetpoint = -deltaTime * M_PI * (control_input->mouse.x / (M_MOUSE_X_SCALE))*9;
	pitchSetpoint += deltaTime * M_PI * (control_input->mouse.y / (M_MOUSE_Y_SCALE));

	// Apply pitch soft limits
	/*if (pitchSetpoint < M_TURRET_PITCH_LOWER_LIMIT) {
		pitchSetpoint = M_TURRET_PITCH_LOWER_LIMIT;
	} else if (pitchSetpoint > M_TURRET_PITCH_UPPER_LIMIT) {
		pitchSetpoint = M_TURRET_PITCH_UPPER_LIMIT;
	}*/

	
	//read IMU
	/*
	BMI088_read(gyro, accel, &temp);
	float gyroMultiplier = 10.0f;
	yawSetpoint += ((((prevGyroVertical + gyro[2]) / 2.0f) * deltaTime) / 1000.0f) * gyroMultiplier;
	prevGyroVertical = gyro[2];
	*/
	
	Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile, &yawState, &pitchState);

	// Shooter Code
	float feederSpeed = 0.0f;

	//check for fire toggle
	if(control_input->key.v == M_C_BITMASK && !idleDebounce){
		if(!idleOn)
			idleOn = true;	//toggle idle state
		else
			idleOn = false;
		idleDebounce = true;
	} else{
		idleDebounce = false;
	}

	// Fire
	//have idle state on if chosen
	if(idleOn)
		fric_on((uint16_t) (1030));
	
	if (control_input->mouse.press_l) {
		fric_on((uint16_t) (1030)); //28
		//fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET + (M_SNAIL_SPEED_SCALE * M_SHOOTER_CURRENT_PERCENT)));
		feederSpeed = -M_M2006_CURRENT_SCALE * 0.05; // * CONF_SHOOTER_FIRERATE_BURST;
			if(control_input->key.v & M_V_BITMASK){
				feederSpeed = -M_M2006_CURRENT_SCALE * 0.1;
	}
	// Prespin
	} else if(control_input->mouse.press_r) {
	fric_on((uint16_t) (1030));
		//fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET + (M_SNAIL_SPEED_SCALE * M_SHOOTER_CURRENT_PERCENT)));
	// Unjam
	} else if (control_input->key.v & M_R_BITMASK) {
		feederSpeed = M_M2006_CURRENT_SCALE * 0.05;
		fric_on((uint16_t) (1100));
	}else {
		fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET));
	}

	if (get_pitch_gimbal_motor_measure_point()->temperate > M_TURRET_PITCH_TEMP_LIMIT) {
		turret.pitch = 0.0f;
	}

	if (turret.yaw > M_GM6020_VOLTAGE_MAX) {
		turret.yaw = M_GM6020_VOLTAGE_MAX;
	}
	
  float feederRPM = (get_trigger_motor_measure_point()->speed_rpm);
	
	//feeder PID
	feederSpeed = calculatePID_Speed(feederRPM,feederSpeed,feederProfile,&feederState);
	
	// Set motor output values 
	CAN_cmd_gimbal_working(turret.yaw * M_GM6020_VOLTAGE_SCALE, turret.pitch * M_M3508_CURRENT_SCALE, feederSpeed, 0);
	//CAN_cmd_yaw(turret.yaw * M_GM6020_VOLTAGE_SCALE);
}

Turret calculateTurret(float yawAngle, float pitchAngle, PIDProfile yawPIDProfile, PIDProfile pitchPIDProfile, PIDState *yawPIDState, PIDState *pitchPIDState) {
  // Captures positions in encoders native format
  float yawPosition = (get_yaw_gimbal_motor_measure_point()->speed_rpm);
  float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd);

  // Keeps track of true position including reduction
  pitchRotations += countRotationsM3508(pitchPosition, prevPitchPosition);
  prevPitchPosition = pitchPosition;
  pitchPosition = (pitchPosition + pitchRotations * M_M3508_ENCODER_SCALE - pitchOffset) * M_M3508_REDUCTION_RATIO;
  tempPitchPosition = (2.0f * M_PI * pitchPosition) / M_M3508_ENCODER_SCALE;

  // Calculate turret thrusts using PID with input of radians
  Turret turret;
  turret.yaw = calculatePID_Speed((2.0f * M_PI * yawPosition) / M_GM6020_ENCODER_SCALE, yawAngle, yawPIDProfile, yawPIDState);
  turret.pitch = calculatePID_SinFeedforward((2.0f * M_PI * pitchPosition) / M_M3508_ENCODER_SCALE, pitchAngle, pitchPIDProfile, pitchPIDState);

  return turret;
}
