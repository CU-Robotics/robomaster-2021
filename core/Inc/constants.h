#ifndef __constants_H
#define __constants_H
#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"

// -- MATH --
#define M_PI 3.1415926f

// -- CONTROLS --
#define M_CONTROLLER_JOYSTICK_SCALE 660.0f
#define M_CONTROLLER_SWITCH_STATES {0, 1, 2}
#define M_CONTROLLER_X_AXIS 3
#define M_CONTROLLER_Y_AXIS 2
#define M_CONTROLLER_ROTATION_AXIS 4
#define M_MOUSE_X_SCALE 3000.0f
#define M_MOUSE_Y_SCALE 3000.0f

// -- PHYSICAL CONSTRAINTS --
#define M_CHASSIS_MAX_RPM 400.0f							// Chassis motors are capped at this speed
#define M_SHOOTER_CURRENT_PERCENT 1.0f                  	// What percent power to run the shooter at. Used to keep the projectile exit speed within regulations.
#define M_SHOOTER_DELAY 60
#define M_SHOOTER_UNJAM_PERIOD 200
#define M_FEEDER_UNJAM_CURRENT_PERCENT 0.5f                // What percent power to run the feeder when unjaming it
#define M_FEEDFORWARD_PHASE 1.0f
#define M_TURRET_PITCH_LOWER_LIMIT -0.3
#define M_TURRET_PITCH_UPPER_LIMIT 0.7
#define M_TURRET_PITCH_TEMP_LIMIT 70
#define M_BATTERY_VOLTAGE 24.0f

// -- GAME CONFIGURATION --
// Chassis
#define CONF_CHASSIS_CURRENT_LIMIT_0 (40.0f / M_BATTERY_VOLTAGE)                                                                  // Level 0
#define CONF_CHASSIS_CURRENT_LIMIT_POW { (60.0f / M_BATTERY_VOLTAGE), (80.0f / M_BATTERY_VOLTAGE), (100.0f / M_BATTERY_VOLTAGE) } // Power-Focused Chassis
#define CONF_CHASSIS_CURRENT_LIMIT_HP { (45.0f / M_BATTERY_VOLTAGE), (50.0f / M_BATTERY_VOLTAGE), (55.0f / M_BATTERY_VOLTAGE) }   // HP-Focused Chassis
// Shooter
#define CONF_SHOOTER_SPEED_PERCENTAGE_0 0.70f                               // Level 0
#define CONF_SHOOTER_SPEED_PERCENTAGE_BURST { (0.70f), (0.70f), (0.70f) }   // Burst-Focused Shooter
#define CONF_SHOOTER_SPEED_PERCENTAGE_COOLING { (0.70f), (0.80f), (0.80f) } // Cooling-Focused Shooter
#define CONF_SHOOTER_SPEED_PERCENTAGE_SPEED { (0.90f), (0.90f), (0.90f) }   //Projectile Speed-Focused Shooter
// Feeder
#define CONF_SHOOTER_FIRERATE_BURST 0.04f                 // Feeder speed in burst mode
#define CONF_SHOOTER_FIRERATE_HIGH 0.03f                  // Feeder speed in high firerate mode
#define CONF_SHOOTER_FIRERATE_LOW 0.02f                   // Feeder speed at low firerate mode

// -- MOTORS --
// M3508
#define M_M3508_CURRENT_MAX		20.0f												// Maximum current of motor in amps
#define M_M3508_CURRENT_SCALE 16384.0f                  	// Scale of M3508 motor input, where this value is max current
#define M_M3508_ENCODER_SCALE 8191.0f                   	// Scale of M3508 encoder output, where this value is one full rotation
#define M_M3508_REDUCTION_RATIO 1 / (3591.0f / 187.0f)  	// The gear ratio between the encoder and the output shaft in an M3508
// M2006
#define M_M2006_CURRENT_SCALE 16384.0f                  	// Scale of M2006 motor input, based on percent max current
#define M_M2006_ENCODER_SCALE 8191.0f                   	// Scale of M2006 encoder output, where this value is one full rotation
#define M_M2006_REDUCTION_RATIO 1 / (36.0f)             	// The gear ratio between the encoder and the output shaft in an M2006
// GM6020
#define M_GM6020_VOLTAGE_SCALE 30000.0f                 	// Scale of GM6020 motor input, based on percent max voltage
#define M_GM6020_ENCODER_SCALE 8191.0f                  	// Scale of GM6020 encoder output, where this value is one full rotation
// Snail
#define M_SNAIL_SPEED_SCALE 1000.0f                     	// Scale of snail motor input, based on percent max speed
#define M_SNAIL_SPEED_OFFSET 1000.0f             	        // PWM has no negative values; "0" starts at 1000 and values less than 1000 are negative

// -- MISC --
#define M_MASTER_LOOP_INTERVAL 2 					    						// Code runs at 500Hz (2ms period)
#define M_MASTER_LOOP_TIMEOUT 5
#define M_ZERO_HARDSTOP_TIME_THRESHOLD 50 			    			// Motor must be held stationary for at least 50ms to detect a hardstop
#define M_ZERO_HARDSTOP_RANGE_THRESHOLD M_PI / (128.0f)	  // All encoder values must be this close to each other in order to count as stationary
#define M_PID_INTEGRAL_BUFFER_SIZE 100                 	  // 100ms

// -- BITMASKS -- 
#define M_W_BITMASK BIT0
#define M_S_BITMASK BIT1
#define M_A_BITMASK BIT2
#define M_D_BITMASK BIT3
#define M_SHIFT_BITMASK BIT4
#define M_CTRL_BITMASK BIT5
#define M_Q_BITMASK BIT6
#define M_E_BITMASK BIT7
#define M_R_BITMASK BIT8
#define M_Z_BITMASK BIT11
#define M_X_BITMASK BIT12

#define BIT0  0x0001
#define BIT1  0x0002
#define BIT2  0x0004
#define BIT3  0x0008
#define BIT4  0x0010
#define BIT5  0x0020
#define BIT6  0x0040
#define BIT7  0x0080
#define BIT8  0x0100
#define BIT9  0x0200
#define BIT10 0x0400
#define BIT11 0x0800
#define BIT12 0x1000
#define BIT13 0x2000
#define BIT14 0x4000
#define BIT15 0x8000

#ifdef __cplusplus
}
#endif
#endif
