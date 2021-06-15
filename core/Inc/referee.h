#ifndef __referee_H
#define __referee_H

#include "struct_typedef.h"
#include "stm32f4xx_hal.h"

/* DEFINITIONS */

//Check Data

//Packet information and offsets
#define ConfirmCRC		//define to confirm CRCs

#define REF_PACKET_BUFFER_LENGTH 256

#define HEADER_length 	0x05
#define SOF_data 				0xA5
#define SOF_Offset 			0x00
#define DL_offset 			0x0001
#define SEQ_offset 			0x0003
#define CRC8_offset 		0x0004

#define CMD_length			0x0002
#define CMD_ID_offset 	0x0005

#define DATA_offset 		0x0006

#define CRC16_length  	0x0002

#define PACKET_MAX_LENGTH 0x0030
#define PACKET_MIN_LENGTH 0x0009

//Command IDs
#define CMD_ID_game_status 				0x0001
#define CMD_ID_game_result 				0x0002
#define CMD_ID_robot_HP 					0x0003
#define CMD_ID_dart_status 				0x0004
#define CMD_ID_debuff_status 			0x0005
#define CMD_ID_event_data 				0x0101
#define CMD_ID_projectile_action 	0x0102
#define CMD_ID_warnings 					0x0104
#define CMD_ID_remaining_time 		0x0105
#define CMD_ID_robot_status 			0x0201
#define CMD_ID_heat_data 					0x0202
#define CMD_ID_robot_pos 					0x0203
#define CMD_ID_buff 							0x0204
#define CMD_ID_aerial_energy 			0x0205
#define CMD_ID_robot_hurt 				0x0206
#define CMD_ID_shoot_data 				0x0207
#define CMD_ID_bullet_remaining 	0x0208
#define CMD_ID_rfid_status 				0x0209
#define CMD_ID_dart_client_cmd 		0x020A


/* STRUCTS */
//packet data and parsing info
typedef struct{
	uint8_t packetBytes[PACKET_MAX_LENGTH];
	uint16_t dataLength;
	uint8_t bytesProcessed;
	uint8_t isComplete;
} refPacket;

//ref struct definitions outlined in:
//https://rm-static.djicdn.com/tem/17348/RoboMaster%202021%20Referee%20System%20Serial%20Port%20Protocol%20Appendix%20V1.1%EF%BC%8820210419%EF%BC%89.pdf

typedef __packed struct
{
  uint8_t game_type : 4;
  uint8_t game_progress : 4;
  uint16_t stage_remain_time;
	uint64_t SyncTimeStamp;
} ext_game_status_t;

typedef __packed struct
{
 uint8_t winner;
} ext_game_result_t;

typedef __packed struct
{
 uint16_t red_1_robot_HP;
 uint16_t red_2_robot_HP;
 uint16_t red_3_robot_HP;
 uint16_t red_4_robot_HP;
 uint16_t red_5_robot_HP;
 uint16_t red_7_robot_HP;
 uint16_t red_outpost_HP;
 uint16_t red_base_HP;
 uint16_t blue_1_robot_HP;
 uint16_t blue_2_robot_HP;
 uint16_t blue_3_robot_HP;
 uint16_t blue_4_robot_HP;
 uint16_t blue_5_robot_HP;
 uint16_t blue_7_robot_HP;
 uint16_t blue_outpost_HP;
 uint16_t blue_base_HP;
} ext_game_robot_HP_t;

typedef __packed struct
{
 uint8_t dart_belong;
 uint16_t stage_remaining_time;
} ext_dart_status_t;

typedef __packed struct
{
 uint8_t F1_zone_status:1;
 uint8_t F1_zone_buff_debuff_status:3; 
 uint8_t F2_zone_status:1;
 uint8_t F2_zone_buff_debuff_status:3;
 uint8_t F3_zone_status:1;
 uint8_t F3_zone_buff_debuff_status:3;
 uint8_t F4_zone_status:1;
 uint8_t F4_zone_buff_debuff_status:3;
 uint8_t F5_zone_status:1;
 uint8_t F5_zone_buff_debuff_status:3;
 uint8_t F6_zone_status:1;
 uint8_t F6_zone_buff_debuff_status:3;
 uint16_t red1_bullet_left;
 uint16_t red2_bullet_left;
 uint16_t blue1_bullet_left;
 uint16_t blue2_bullet_left;
} ext_ICRA_buff_debuff_zone_status_t;

typedef __packed struct
{
 uint32_t event_type;
} ext_event_data_t;

typedef __packed struct
{
 uint8_t supply_projectile_id; 
 uint8_t supply_robot_id;
 uint8_t supply_projectile_step;
 uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;

typedef __packed struct
{
 uint8_t level;
 uint8_t foul_robot_id;
} ext_referee_warning_t;

typedef __packed struct
{
 uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

typedef __packed struct
{
 uint8_t robot_id;
 uint8_t robot_level;
 uint16_t remain_HP;
 uint16_t max_HP;
 uint16_t shooter_id1_17mm_cooling_rate;
 uint16_t shooter_id1_17mm_cooling_limit;
 uint16_t shooter_id1_17mm_speed_limit;
 uint16_t shooter_id2_17mm_cooling_rate;
 uint16_t shooter_id2_17mm_cooling_limit;
 uint16_t shooter_id2_17mm_speed_limit;
 uint16_t shooter_id1_42mm_cooling_rate;
 uint16_t shooter_id1_42mm_cooling_limit;
 uint16_t shooter_id1_42mm_speed_limit;
 uint16_t chassis_power_limit;
 uint8_t mains_power_gimbal_output : 1;
 uint8_t mains_power_chassis_output : 1;
 uint8_t mains_power_shooter_output : 1;
} ext_game_robot_status_t;

typedef __packed struct
{
 uint16_t chassis_volt;
 uint16_t chassis_current;
 float chassis_power;
 uint16_t chassis_power_buffer;
 uint16_t shooter_id1_17mm_cooling_heat;
uint16_t shooter_id2_17mm_cooling_heat;
uint16_t shooter_id1_42mm_cooling_heat;
} ext_power_heat_data_t;

typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
} ext_game_robot_pos_t;

typedef __packed struct
{
 uint8_t power_rune_buff;
}ext_buff_t;

typedef __packed struct
{
 uint8_t attack_time;
} aerial_robot_energy_t;

typedef __packed struct
{
 uint8_t armor_id : 4;
 uint8_t hurt_type : 4;
} ext_robot_hurt_t;

typedef __packed struct
{
 uint8_t bullet_type;
uint8_t shooter_id;
 uint8_t bullet_freq;
 float bullet_speed;
} ext_shoot_data_t;

typedef __packed struct
{
 uint16_t bullet_remaining_num_17mm;
uint16_t bullet_remaining_num_42mm;
uint16_t coin_remaining_num;
} ext_bullet_remaining_t;

typedef __packed struct
{
 uint32_t rfid_status;
} ext_rfid_status_t;

typedef __packed struct
{
 uint8_t dart_launch_opening_status;
 uint8_t dart_attack_target;
 uint16_t target_change_time;
 uint8_t first_dart_speed;
 uint8_t second_dart_speed;
 uint8_t third_dart_speed;
 uint8_t fourth_dart_speed;
 uint16_t last_dart_launch_time;
 uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;



//Full struct for all ref sysetem information
typedef struct{
	ext_game_status_t 		REF_game_status;
	ext_game_result_t 		REF_game_result;
	ext_game_robot_HP_t 	REF_robot_HP;
	ext_dart_status_t 		REF_dart_status;
	ext_ICRA_buff_debuff_zone_status_t REF_debuff_status;
	ext_event_data_t			REF_event_data;
	ext_supply_projectile_action_t REF_projectile_action;
	ext_referee_warning_t	REF_warnings;
	ext_dart_remaining_time_t	REF_remaining_time;
	ext_game_robot_status_t REF_robot_status;
	ext_power_heat_data_t REF_heat_data;
	ext_game_robot_pos_t  REF_robot_pos;
	ext_buff_t						REF_buff;
	aerial_robot_energy_t REF_aerial_energy;
	ext_robot_hurt_t			REF_robot_hurt;
	ext_shoot_data_t			REF_shoot_data;
	ext_bullet_remaining_t REF_bullet_remaining;
	ext_rfid_status_t			REF_rfid_status;
	ext_dart_client_cmd_t REF_dart_client_cmd;
} referee_data_t;




/* FUNCTIONS */

//processing functions
void REF_Process_Packet_Buffer(refPacket *packet_Buffer, referee_data_t *ref_Data);
void REF_Parse_Packet_Bytewise(refPacket *tempPacket, refPacket *packet_Buffer, uint8_t incomingByte);
void REF_Clear_Packet(refPacket *packet);
void REF_Write_Packet_To_Buffer(refPacket *tempPacket, refPacket *packet_Buffer);

//contains all of the tasks neccesary to run the ref system
void refereeInitialization(void);
void refereeLoop(void);

/* CRC */
//These functions serve to verify the packets
unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8);
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength);
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC);
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength);
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength);

#endif
