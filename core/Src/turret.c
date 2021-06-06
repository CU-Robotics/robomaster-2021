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
int flywheelSpinupTracker;
int unjamTracker;
int unjamDirection;
int pitchRotations;
int prevPitchPosition;
int pitchHistory[M_ZERO_HARDSTOP_TIME_THRESHOLD];
int pitchOffset;

float yawSetpoint = 0;
float pitchSetpoint = 0;
float tempPitchPosition = 0;

void turretInit() {
    // PID Profiles containing tuning parameters.
    yawProfile.kP = 16.0f / (8 * M_PI);
    yawProfile.kI = 0.00025f / (8 * M_PI);
    yawProfile.kD = 2.0f / (8 * M_PI);

    pitchProfile.kP = 10.0f / (8 * M_PI);
    pitchProfile.kI = 0.0f / (8 * M_PI);
    pitchProfile.kD = 0.0f / (8 * M_PI);
		pitchProfile.kF = 4.20f / (8 * M_PI);
	
    // PID States
    yawState.lastError = 0;
    pitchState.lastError = 0;

    zeroed = false;
    flywheelSpinupTracker = 0;
    unjamTracker = 0;
    unjamDirection = 1;

    pitchRotations = 0;
    prevPitchPosition = 0;
    //Fill pitch history with values preventing false positive hardstop reading
    for(int i = 0; i < M_ZERO_HARDSTOP_TIME_THRESHOLD; i++)
        pitchHistory[i] = 30 * i;
}

void turretLoop(const RC_ctrl_t* control_input, int deltaTime) {
    if (zeroed) {
        //float yawSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_X_AXIS] / (M_CONTROLLER_JOYSTICK_SCALE));
        //float pitchSetpoint = M_PI * (control_input->rc.ch[M_CONTROLLER_Y_AXIS] / M_CONTROLLER_JOYSTICK_SCALE);
				
				//experimental mouse gimbal control
				yawSetpoint += M_PI * (control_input->mouse.x / (M_MOUSE_X_SCALE));
        pitchSetpoint += M_PI * (control_input->mouse.y / (M_MOUSE_Y_SCALE));
			
				//keep setpoints in -2pi to 2pi range for stability
				if(yawSetpoint > 2*M_PI || yawSetpoint < -2*M_PI){
					yawSetpoint = 0;
				}
				if(pitchSetpoint > 2*M_PI || pitchSetpoint < -2*M_PI){
					pitchSetpoint = 0;
				}

        Turret turret = calculateTurret(yawSetpoint, pitchSetpoint, yawProfile, pitchProfile, &yawState, &pitchState);

        float feederSpeed = 0.0f;

        // Fire
        if (control_input->mouse.press_l) {
          fric_on((uint16_t) ((M_SNAIL_SPEED_OFFSET + M_SNAIL_SPEED_SCALE) * M_SHOOTER_CURRENT_PERCENT));
          flywheelSpinupTracker += deltaTime;
          if (flywheelSpinupTracker >= M_SHOOTER_DELAY) {
            feederSpeed = M_M2006_CURRENT_SCALE * -M_FEEDER_CURRENT_PERCENT;
            flywheelSpinupTracker = M_SHOOTER_DELAY;
          }
        // Prespin
        } else if(control_input->mouse.press_r){
					fric_on((uint16_t) ((M_SNAIL_SPEED_OFFSET + M_SNAIL_SPEED_SCALE) * M_SHOOTER_CURRENT_PERCENT));
        // Unjam
				} else if (control_input->key.v['r']) {
          if (unjamTracker >= M_SHOOTER_UNJAM_PERIOD || unjamTracker <= -M_SHOOTER_UNJAM_PERIOD)
            unjamDirection = -unjamDirection;
          fric_on((uint16_t) ((M_SNAIL_SPEED_OFFSET + M_SNAIL_SPEED_SCALE * unjamDirection) * M_SHOOTER_CURRENT_PERCENT));
          feederSpeed = M_M2006_CURRENT_SCALE * M_FEEDER_CURRENT_PERCENT * unjamDirection;
          unjamTracker += unjamDirection * deltaTime;
        // Base State
        } else {
          fric_on((uint16_t) (M_SNAIL_SPEED_OFFSET));
          flywheelSpinupTracker -= deltaTime;
          if (flywheelSpinupTracker < 0) { flywheelSpinupTracker = 0 };
          unjamTracker = 0;
          unjamDirection = 1;
        }

        CAN_cmd_gimbal_working(turret.yaw * M_GM6020_VOLTAGE_SCALE, turret.pitch * M_M3508_CURRENT_SCALE, feederSpeed, 0);
    /* Zero Sequence */
    } else {
				// Move pitch motor towards front endstop
        CAN_cmd_gimbal_working(0, 4000, 0, 0);
				
				// Capture encoder value
				float pitchPosition = (get_pitch_gimbal_motor_measure_point()->ecd);

				// Rotate pitch history
				for (int i = 0; i < M_ZERO_HARDSTOP_TIME_THRESHOLD - 1; i++){
					pitchHistory[i] = pitchHistory[i+1];
				}
				pitchHistory[M_ZERO_HARDSTOP_TIME_THRESHOLD - 1] = pitchPosition;

				// Check if at hardstop
				if(isM3508AtHardstop(pitchHistory)){
						zeroed = true;
						pitchOffset = pitchPosition;
				}
    }
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
	
    Turret turret;

		// Calculate turret thrusts using PID with input of radians
    turret.yaw = calculatePID((2.0f * M_PI * yawPosition) / M_GM6020_ENCODER_SCALE, yawAngle, yawPIDProfile, yawPIDState);
    turret.pitch = calculatePID_SinFeedforward((2.0f * M_PI * pitchPosition) / M_M3508_ENCODER_SCALE, pitchAngle, pitchPIDProfile, pitchPIDState);

    return turret;
}
