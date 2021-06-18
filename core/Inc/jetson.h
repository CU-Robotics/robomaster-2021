#ifndef __jetson_H
#define __jetson_H

#include "struct_typedef.h"
#include "stm32f4xx_hal.h"

#define JETSON_BUFFER_LENGTH 256

//jetson packet structure
#define PACKET_LENGTH 		10

#define SOP_OFFSET 				0x00
#define STATUS_OFFSET 		0x01
#define X_POS_OFFSET 			0x02
#define Y_POS_OFFSET 			0x04
#define DISTANCE_OFFSET 	0x06
#define CRC8_OFFSET				0x08
#define EOP_OFFSET				0x09

#define SOP								0xA5
#define EOP								0x5A

//default states
#define DEFAULT_STATUS 		0x00
#define DEFAULT_X_POS			0x00
#define	DEFAULT_Y_POS			0x00
#define DEFAULT_DISTANCE	0x00

typedef __packed struct{
	uint8_t status;
	uint16_t xPos;
	uint16_t yPos;
	uint16_t distance;
} jetsonData_t;

typedef struct{
	uint8_t packetBytes[PACKET_LENGTH];
	uint8_t bytesProcessed;
	uint8_t isComplete;
} jetsonPacket_t;

//globally available jetson data
static jetsonData_t jetsonData;

/* FUNCTIONS */
void jetsonInitialization(void);
void jetsonLoop(void);

void JETSON_Parse_Packet_Bytewise(jetsonPacket_t *tempPacket, uint8_t incomingByte);
void JETSON_Clear_Packet(jetsonPacket_t *packet);
void JETSON_Write_Packet_To_Buffer(jetsonPacket_t *tempPacket, jetsonPacket_t *packet_Buffer);
void JETSON_Process_Packet_Buffer(jetsonPacket_t *packet_Buffer, jetsonData_t *jetson_Data);

#endif
