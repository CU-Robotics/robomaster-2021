#include "jetson.h"
#include "usart.h"
#include "CRC.h"

//locally available jetson data
jetsonPacket_t jetsonTempPacket;
jetsonPacket_t jetsonPacketBuffer[JETSON_BUFFER_LENGTH];

/* USART1 is used for the jetson nano and corresponds to UART1 on the board, the 4-pin port */
/*
void USART1_IRQHandler(void){
    volatile uint8_t receive;
    //receive interrupt
    if(huart1.Instance->SR & UART_FLAG_RXNE)
    {
        receive = huart1.Instance->DR;
        HAL_GPIO_TogglePin(LED_R_GPIO_Port, LED_R_Pin);
				JETSON_Parse_Packet_Bytewise(&jetsonTempPacket, receive);
    }
    //idle interrupt
    else if(huart1.Instance->SR & UART_FLAG_IDLE)
    {
        receive = huart1.Instance->DR;
        HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET);
    }

}
*/

//gets called on initialization, clears buffer and packets
void jetsonInitialization(){
	JETSON_Clear_Packet(&jetsonTempPacket);
	jetsonData.status = DEFAULT_STATUS;
	jetsonData.xPos = DEFAULT_X_POS;
	jetsonData.yPos = DEFAULT_Y_POS;
	jetsonData.distance = DEFAULT_DISTANCE;
}
//gets called by main every tick
void jetsonLoop(){
	JETSON_Process_Packet_Buffer(jetsonPacketBuffer, &jetsonData);
}

//Processing functions
void JETSON_Parse_Packet_Bytewise(jetsonPacket_t *tempPacket, uint8_t incomingByte){
	if(jetsonTempPacket.bytesProcessed == SOP_OFFSET){
		if(incomingByte == SOP){
			jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		}
	}
	else if(jetsonTempPacket.bytesProcessed < PACKET_LENGTH - 1){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
	}
	else{
		jetsonTempPacket.bytesProcessed = 0;
		JETSON_Write_Packet_To_Buffer(tempPacket, jetsonPacketBuffer);
		JETSON_Clear_Packet(tempPacket);
	}
}
void JETSON_Clear_Packet(jetsonPacket_t *packet){
	for(int i = 0; i < PACKET_LENGTH; i++){
		packet->packetBytes[i] = 0;
	}
	packet->bytesProcessed = 0;
	packet->isComplete = 0;
}
//writes the packet to the lowest available spot in the buffer
void JETSON_Write_Packet_To_Buffer(jetsonPacket_t *tempPacket, jetsonPacket_t *packet_Buffer){
	for(int i = 0; i < JETSON_BUFFER_LENGTH; i++){
		if(!packet_Buffer[i].isComplete){
			packet_Buffer[i] = *tempPacket;
			JETSON_Clear_Packet(tempPacket);
			break;
		}
	}
}
//reads jetson packets into data
void JETSON_Process_Packet_Buffer(jetsonPacket_t *packet_Buffer, jetsonData_t *jetson_Data){
	for(int i = 0; i < JETSON_BUFFER_LENGTH; i++){
		uint8_t pchMessage[PACKET_LENGTH - 1];
		for(int j = 0; j < PACKET_LENGTH - 1; j++){
			pchMessage[j] = packet_Buffer[i].packetBytes[j];
		}
		if(Verify_CRC8_Check_Sum(pchMessage, PACKET_LENGTH - 1)){
			for(int j = 0; j < PACKET_LENGTH - 1; j++){
				((uint8_t*)jetson_Data)[j] = packet_Buffer[i].packetBytes[j];
			}
		}
	}
}
