/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;

/* USER CODE BEGIN Private defines */

//Length of buffer storing incoming uart bytes
#define UART_BUFFER_LENGTH 255

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART3_UART_Init(void);
void MX_USART6_UART_Init(void);

/* USER CODE BEGIN Prototypes */

typedef struct{
	uint8_t data[UART_BUFFER_LENGTH];
	uint8_t readHead;
	uint8_t writeHead;
	uint32_t bytesRecieved;
	uint32_t bytesRead;
} uartBuffer;	

enum bufferReturnMessage{
	success,
	error
};

uartBuffer newBuffer(void);
void flushBuffer(uartBuffer *buff);
uint8_t addByteToBuffer(uartBuffer *buff, uint8_t byteToAdd);
uint8_t readSingleByteFromBuffer(uartBuffer *buff, uint8_t *dataOut);
uint8_t readBytesFromBuffer(uartBuffer *buff, uint8_t *dataOut, uint8_t numOfBytes);

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
