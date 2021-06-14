#include "referee.h"
#include "usart.h"
#include "stm32f4xx_hal.h"

//globally available ref variables
referee_data_t refData;
uint8_t refPacket[PACKET_MAX_LENGTH];
uint8_t refPacketParserState;
uartBuffer refBuffer;

/* USART6 is used for the referee system and corresponds to UART2 on the board, the 3-pin port */
//This interrupt is called everytime a byte is recieved
//The byte is then fed into the buffer
void USART6_IRQHandler(void)  
{

    volatile uint8_t receive;
    //receive interrupt
    if(huart6.Instance->SR & UART_FLAG_RXNE)
    {
        receive = huart6.Instance->DR;
        HAL_GPIO_TogglePin(LED_G_GPIO_Port, LED_G_Pin);
				addByteToBuffer(&refBuffer, receive);

    }
    //idle interrupt
    else if(huart6.Instance->SR & UART_FLAG_IDLE)
    {
        receive = huart6.Instance->DR;
        HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET);
    }


}

void refereeInitialization(void){
	refPacketParserState = SEARCHING_FOR_SOF;
	refBuffer = newBuffer();
}

void refereeLoop(void){
	switch(refPacketParserState){
		case SEARCHING_FOR_SOF:{
			//iterate through buffer searching for start of packet
			for(int i = 0; i < UART_BUFFER_LENGTH; i++){
				uint8_t byteToCheck;
				
				//read byte, and flush buffer if there's a problem
				if(readSingleByteFromBuffer(&refBuffer, &byteToCheck) == error){
					flushBuffer(&refBuffer);
					break;
				}
					
				if(byteToCheck == SOF_data){
					refPacketParserState = PROCESSING_HEADER;
					refPacket[0] = SOF_data;
					break;
				}
			}
			break;
		}
		case PROCESSING_HEADER:{
			//read header
			for(int i = 1; i < 5; i++){
				uint8_t readByte;
				
				//read byte, and flush buffer if there's a problem
				if(readSingleByteFromBuffer(&refBuffer, &readByte) == error){
					flushBuffer(&refBuffer);
					break;
				}
				
				refPacket[i] = readByte;
			}
			
			//confirm CRC or move to data extraction
			#ifdef ConfirmCRC
				if(Verify_CRC8_Check_Sum(refPacket,HEADER_length)){
					refPacketParserState = EXTRACTING_DATA;
				}
				else{
					refPacketParserState = SEARCHING_FOR_SOF;
				}
			#else
				refPacketParserState = EXTRACTING_DATA;
			#endif
			break;
		}
		case EXTRACTING_DATA:{
			//read length of data
			uint16_t dataLength = ((uint16_t)refPacket[DL_offset]) >> 8 | (uint16_t)refPacket[DL_offset + 1];
			
			//extract data from buffer
			uint8_t data[dataLength];
			//read byte, and flush buffer if there's a problem
			if(readBytesFromBuffer(&refBuffer, data, dataLength) == error){
				flushBuffer(&refBuffer);
				break;
			}
			
			//put data into ref packet
			for(int i = 0; i < dataLength; i++){
				refPacket[DATA_offset + i] = data[i];
			}
			
			//parse packet, includeing CRC16 check
			REF_Parse_Packet(refPacket, &refData);
			
			//re-enter search for next packet
			refPacketParserState = SEARCHING_FOR_SOF;
			break;
		}
		default:{
			refPacketParserState = SEARCHING_FOR_SOF;
			break;
		}
	}
}

uint8_t REF_Parse_Packet(uint8_t *packet_Data, referee_data_t *ref_data){
	/* SEARCH FOR HEADER */
	uint8_t SOF_Offset = 0;
	for(SOF_Offset = 0; SOF_Offset < PACKET_MAX_LENGTH - PACKET_MIN_LENGTH; SOF_Offset++){
		//check if byte matches Start Of Frame (SOF)
		if(packet_Data[SOF_Offset] == SOF_data)
			break;
		
		//if loop reaches end without find start of frame, return error
		if(SOF_Offset == PACKET_MAX_LENGTH - PACKET_MIN_LENGTH - 1)
			return 0U;
	}	
	/* PROCESS HEADER (the first five bytes) */
	uint8_t Header_Frame[5];
	
	
	Header_Frame[0] = packet_Data[SOF_Offset];
	
	
	//reads length of data from the next two bytes
	Header_Frame[DL_offset] = packet_Data[SOF_Offset + DL_offset];
	uint16_t data_length = ((uint16_t)packet_Data[SOF_Offset + DL_offset]) >> 8 | (uint16_t)packet_Data[SOF_Offset + DL_offset + 1];
	
	//reads sequence number
	//wtf is sequence?
	Header_Frame[SEQ_offset] = packet_Data[SOF_Offset + SEQ_offset];
	uint8_t sequence_number = packet_Data[SOF_Offset + SEQ_offset];
	
	//Verify with CRC8
	Header_Frame[CRC8_offset] = packet_Data[SOF_Offset + CRC8_offset];
	if(!Verify_CRC8_Check_Sum(Header_Frame, HEADER_length))
		return 0U;
	
	/* PROCESS CMD ID */
	uint16_t CMD_ID = ((uint16_t)packet_Data[SOF_Offset + CMD_ID_offset]) >> 8 | (uint16_t)packet_Data[SOF_Offset + CMD_ID_offset + 1];
	
	/* CONFIRM CRC16 */
	//Needs to happen before loading data into the ref system struct
	
	if(!Verify_CRC16_Check_Sum(packet_Data, (uint32_t)(HEADER_length + CMD_length + data_length + CRC16_length)))
		return 0U;
	
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
		Target_Struct[i] = packet_Data[SOF_Offset + DATA_offset + i];
	
	//return true on successful processing!
	return 1U;
}

//Cyclical Redundancy Checks
//crc8 generator polynomial:G(x)=x8+x5+x4+1
const unsigned char CRC8_INIT = 0xff;
const unsigned char CRC8_TAB[256] =
{
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc, 0x23, 0x7d, 0x9f,
	0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62, 0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81,
	0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff, 0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84,
	0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07, 0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5,
	0xfb, 0x78, 0x26, 0xc4, 0x9a, 0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6,
	0x98, 0x7a, 0x24, 0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7,
	0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd, 0x11, 0x4f, 0xad,
	0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50, 0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90,
	0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee, 0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0,
	0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73, 0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea,
	0x69, 0x37, 0xd5, 0x8b, 0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa,
	0x48, 0x16, 0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35,
};

unsigned char Get_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength, unsigned char ucCRC8)
{
	unsigned char ucIndex;
	while (dwLength--)
	{	
		ucIndex = ucCRC8^(*pchMessage++);
		ucCRC8 = CRC8_TAB[ucIndex];
	}
	return(ucCRC8);
}

/*
** Descriptions: CRC8 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
unsigned int Verify_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
unsigned char ucExpected = 0;
if ((pchMessage == 0) || (dwLength <= 2)) return 0;
ucExpected = Get_CRC8_Check_Sum (pchMessage, dwLength-1, CRC8_INIT);
return ( ucExpected == pchMessage[dwLength-1] );
} 

/*
** Descriptions: append CRC8 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC8_Check_Sum(unsigned char *pchMessage, unsigned int dwLength)
{
unsigned char ucCRC = 0;
if ((pchMessage == 0) || (dwLength <= 2)) return;
ucCRC = Get_CRC8_Check_Sum ( (unsigned char *)pchMessage, dwLength-1, CRC8_INIT);
pchMessage[dwLength-1] = ucCRC;
}

uint16_t CRC_INIT = 0xffff;
const uint16_t wCRC_Table[256] =
{
0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/*
** Descriptions: CRC16 checksum function
** Input: Data to check,Stream length, initialized checksum
** Output: CRC checksum
*/
uint16_t Get_CRC16_Check_Sum(uint8_t *pchMessage,uint32_t dwLength,uint16_t wCRC)
{
	uint8_t chData;
	if (pchMessage == NULL)
	{
		return 0xFFFF;
	}
	while(dwLength--)
	{
		chData = *pchMessage++;
		(wCRC) = ((uint16_t)(wCRC) >> 8) ^ wCRC_Table[((uint16_t)(wCRC) ^ (uint16_t)(chData)) & 0x00ff];
	}
	return wCRC;
}

/*
** Descriptions: CRC16 Verify function
** Input: Data to Verify,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
uint32_t Verify_CRC16_Check_Sum(uint8_t *pchMessage, uint32_t dwLength)
{
	uint16_t wExpected = 0;
	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return 0;
	}
	wExpected = Get_CRC16_Check_Sum ( pchMessage, dwLength - 2, CRC_INIT);
	return ((wExpected & 0xff) == pchMessage[dwLength - 2] && ((wExpected >> 8) & 0xff) == pchMessage[dwLength - 1]);
}

/*
** Descriptions: append CRC16 to the end of data
** Input: Data to CRC and append,Stream length = Data + checksum
** Output: True or False (CRC Verify Result)
*/
void Append_CRC16_Check_Sum(uint8_t * pchMessage,uint32_t dwLength)
{
	uint16_t wCRC = 0;
	if ((pchMessage == NULL) || (dwLength <= 2))
	{
		return;
	}
	wCRC = Get_CRC16_Check_Sum ( (uint8_t *)pchMessage, dwLength-2, CRC_INIT );
	pchMessage[dwLength-2] = (uint8_t)(wCRC & 0x00ff);
	pchMessage[dwLength-1] = (uint8_t)((wCRC >> 8)& 0x00ff);
}
