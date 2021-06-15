#include "jetson.h"
#include "usart.h"

jetsonPacket_t jetsonTempPacket;
jetsonData_t jetsonData;

/* USART1 is used for the jetson nano and corresponds to UART1 on the board, the 4-pin port */
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


//gets called on initialization, clears buffer and packets
void jetsonInitialization(){
	JETSON_Clear_Packet(&jetsonTempPacket);
	jetsonData.status = DEFAULT_STATUS;
	jetsonData.xPos = DEFAULT_X_POS;
	jetsonData.yPos = DEFAULT_Y_POS;
	jetsonData.distance = DEFAULT_DISTANCE;
}
//Processing functions
void JETSON_Parse_Packet_Bytewise(jetsonPacket_t *tempPacket, uint8_t incomingByte){
	if(jetsonTempPacket.bytesProcessed == SOP_OFFSET){
		if(incomingByte == SOP){
			jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		}
	}
	else if(jetsonTempPacket.bytesProcessed == STATUS_OFFSET){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.status = incomingByte;
	}
	else if(jetsonTempPacket.bytesProcessed == X_POS_OFFSET){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.xPos = (uint16_t)incomingByte << 8;
	}
	else if(jetsonTempPacket.bytesProcessed == X_POS_OFFSET + 1){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.xPos += incomingByte;
	}
	else if(jetsonTempPacket.bytesProcessed == Y_POS_OFFSET){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.yPos = (uint16_t)incomingByte << 8;
	}
	else if(jetsonTempPacket.bytesProcessed == Y_POS_OFFSET + 1){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.yPos += incomingByte;
	}
	else if(jetsonTempPacket.bytesProcessed == DISTANCE_OFFSET){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.distance = (uint16_t)incomingByte << 8;
	}
	else if(jetsonTempPacket.bytesProcessed == DISTANCE_OFFSET + 1){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
		jetsonData.distance += incomingByte;
	}
	else if(jetsonTempPacket.bytesProcessed == CRC8_OFFSET){
		jetsonTempPacket.packetBytes[jetsonTempPacket.bytesProcessed++] = incomingByte;
	}
	else{
		jetsonTempPacket.bytesProcessed = 0;
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