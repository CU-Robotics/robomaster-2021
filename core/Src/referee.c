#include "referee.h"
#include "usart.h"
#include "CRC.h"
#include "stm32f4xx_hal.h"

//globally available ref variables
referee_data_t refData;
refPacket refTempPacket;
refPacket refPacketBuffer[REF_PACKET_BUFFER_LENGTH];

/* USART6 is used for the referee system and corresponds to UART2 on the board, the 3-pin port */
//This interrupt is called everytime a byte is recieved
//The byte is then processed bytewise in a temporary packet which is transfered to a buffer when complete
void USART6_IRQHandler(void)  
{
    volatile uint8_t receive;
    //receive interrupt
    if(huart6.Instance->SR & UART_FLAG_RXNE)
    {
        receive = huart6.Instance->DR;
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
				REF_Parse_Packet_Bytewise(&refTempPacket, refPacketBuffer, receive);
    }
    //idle interrupt
    else if(huart6.Instance->SR & UART_FLAG_IDLE)
    {
        receive = huart6.Instance->DR;
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
    }
}
//gets called on initialization, clears buffer and packets
void refereeInitialization(void){
	REF_Clear_Packet(&refTempPacket);
	for(int i = 0; i < REF_PACKET_BUFFER_LENGTH; i++){
		REF_Clear_Packet(&refPacketBuffer[i]);
	}
}
//gets called by main loop every tick
void refereeLoop(void){
	REF_Process_Packet_Buffer(refPacketBuffer, &refData);
}
//processes an incoming packet one byte at a time
void REF_Parse_Packet_Bytewise(refPacket *tempPacket, refPacket *packet_Buffer, uint8_t incomingByte){
	if(tempPacket->bytesProcessed == SOF_Offset){
		if(incomingByte == SOF_data){
			tempPacket->packetBytes[tempPacket->bytesProcessed++] = incomingByte;
		}
	}
	else if(tempPacket->bytesProcessed == DL_offset){
		tempPacket->dataLength = (uint16_t)incomingByte << 8;
		tempPacket->packetBytes[tempPacket->bytesProcessed++] = incomingByte;
	}
	else if(tempPacket->bytesProcessed == DL_offset + 1){
		tempPacket->dataLength += (uint16_t)incomingByte;
		tempPacket->packetBytes[tempPacket->bytesProcessed++] = incomingByte;
	}
	else if(tempPacket->bytesProcessed < (tempPacket->dataLength + HEADER_length + CMD_length + CRC16_length - 1)){
		tempPacket->packetBytes[tempPacket->bytesProcessed++] = incomingByte;
	}
	else{
		tempPacket->packetBytes[tempPacket->bytesProcessed++] = incomingByte;
		tempPacket->isComplete = 1;
		REF_Write_Packet_To_Buffer(tempPacket, packet_Buffer);
		REF_Clear_Packet(tempPacket);
	}
}
//writes the packet to the lowest available spot in the buffer
void REF_Write_Packet_To_Buffer(refPacket *tempPacket, refPacket *packet_Buffer){
	for(int i = 0; i < REF_PACKET_BUFFER_LENGTH; i++){
		if(!packet_Buffer[i].isComplete){
			packet_Buffer[i] = *tempPacket;
			REF_Clear_Packet(tempPacket);
			break;
		}
	}
}

//processes all of the packets in the buffer
void REF_Process_Packet_Buffer(refPacket *packet_Buffer, referee_data_t *ref_Data){
	//iterate through buffer in reverse
	for(int i = (REF_PACKET_BUFFER_LENGTH - 1); i >= 0; i--){
		//check to make sure data packet is complete before operating
		if(packet_Buffer[i].isComplete){
			/* PROCESS HEADER (the first five bytes) */
			//collect header data into single array
			uint8_t Header_Frame[5];
			for(int j = 0; j < HEADER_length; j++){
				Header_Frame[j] = packet_Buffer[i].packetBytes[j];
			}
			
			//Verify with CRC8
			#ifdef ConfirmCRC
			if(!Verify_CRC8_Check_Sum(Header_Frame, HEADER_length)){
				REF_Clear_Packet(&packet_Buffer[i]);
				break;
			}
			#endif
			
			//read sequence number ** currently unused
			//uint8_t sequence_number = packet_Buffer[i].packetBytes[SEQ_offset];
			
			//read data length
			uint16_t data_length = ((uint16_t)packet_Buffer[i].packetBytes[DL_offset]) << 8 | (uint16_t)packet_Buffer[i].packetBytes[DL_offset + 1];
			
			/* PROCESS CMD ID */
			uint16_t CMD_ID = ((uint16_t)packet_Buffer[i].packetBytes[CMD_ID_offset]) << 8 | (uint16_t)packet_Buffer[i].packetBytes[CMD_ID_offset + 1];
			
			/* CONFIRM CRC16 */
			//Needs to happen before loading data into the ref system struct
			#ifdef ConfirmCRC
			if(!Verify_CRC16_Check_Sum(packet_Buffer[i].packetBytes, (uint32_t)(packet_Buffer[i].dataLength + HEADER_length + CMD_length + CRC16_length))){
				REF_Clear_Packet(&packet_Buffer[i]);
				break;
			}
			#endif
			
			/* PROCESS DATA */
			
			uint8_t *Target_Struct;
			
			//selects the proper sub-struct to put the data into based on the CMD ID
			switch(CMD_ID){
				case CMD_ID_game_status:
					Target_Struct = (uint8_t*)&(refData.REF_game_status); 
				case CMD_ID_game_result:
					Target_Struct = (uint8_t*)&(refData.REF_game_result); 
				case CMD_ID_robot_HP:
					Target_Struct = (uint8_t*)&(refData.REF_robot_HP); 
				case CMD_ID_dart_status:
					Target_Struct = (uint8_t*)&(refData.REF_dart_status); 
				case CMD_ID_debuff_status:
					Target_Struct = (uint8_t*)&(refData.REF_debuff_status); 
				case CMD_ID_event_data:
					Target_Struct = (uint8_t*)&(refData.REF_event_data); 
				case CMD_ID_projectile_action:
					Target_Struct = (uint8_t*)&(refData.REF_projectile_action); 
				case CMD_ID_warnings:
					Target_Struct = (uint8_t*)&(refData.REF_warnings); 
				case CMD_ID_remaining_time:
					Target_Struct = (uint8_t*)&(refData.REF_remaining_time); 
				case CMD_ID_robot_status:
					Target_Struct = (uint8_t*)&(refData.REF_robot_status); 
				case CMD_ID_heat_data:
					Target_Struct = (uint8_t*)&(refData.REF_heat_data); 
				case CMD_ID_robot_pos:
					Target_Struct = (uint8_t*)&(refData.REF_robot_pos); 
				case CMD_ID_buff:
					Target_Struct = (uint8_t*)&(refData.REF_buff); 
				case CMD_ID_aerial_energy:
					Target_Struct = (uint8_t*)&(refData.REF_aerial_energy); 
				case CMD_ID_robot_hurt:
					Target_Struct = (uint8_t*)&(refData.REF_robot_hurt); 
				case CMD_ID_shoot_data:
					Target_Struct = (uint8_t*)&(refData.REF_shoot_data); 
				case CMD_ID_bullet_remaining:
					Target_Struct = (uint8_t*)&(refData.REF_bullet_remaining); 
				case CMD_ID_rfid_status:
					Target_Struct = (uint8_t*)&(refData.REF_rfid_status); 
				case CMD_ID_dart_client_cmd:
					Target_Struct = (uint8_t*)&(refData.REF_dart_client_cmd); 
			}
			
			
			//fills the packed struct with data
			for(int i = 0; i < data_length; i++)
				Target_Struct[i] = packet_Buffer[i].packetBytes[DATA_offset + i];
			
			//clear packet after reading
			REF_Clear_Packet(&packet_Buffer[i]);
		}
	}
}

//clears the specified packet
void REF_Clear_Packet(refPacket *packet){
	for(int i = 0; i < PACKET_MAX_LENGTH; i++){
		packet->packetBytes[i] = 0;
	}
	packet->dataLength = PACKET_MAX_LENGTH;
	packet->bytesProcessed = 0;
	packet->isComplete = 0;
	
}

