/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "spi.h"
#include "math.h"

#include "remote_control.h"
#include "bsp_usart.h"
#include "bsp_can.h"
#include "CAN_receive.h"
#include "bsp_fric.h"
#include "BMI088driver.h"

#include "chassis.h"
#include "turret.h"
#include "pid.h"
#include "constants.h"
#include "referee.h"

#include "time_manip.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

const RC_ctrl_t *local_rc_ctrl;

void masterLoop(void);



/**
  * @brief  The application entry point.
  * @retval int
  */
	
uint32_t prevTick;

int main(void) {
  // Essential Setup
  HAL_Init();
  SystemClock_Config();

  // Lib inits
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
	MX_DMA_Init();
  MX_SPI1_Init();
	MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM8_Init();
  MX_TIM1_Init();
  MX_TIM10_Init();
  
  // Configure PWM and CAN communication
	fric_off();
	can_filter_init();

  // Start remote control process
	remote_control_init();
	usart1_tx_dma_init();
	local_rc_ctrl = get_remote_control_point();
  
  // Start lib processes
	HAL_TIM_Base_Start(&htim1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
	HAL_TIM_Base_Start(&htim8);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
		
  /* Init functions */
  chassisInit();
  turretInit();

  /* Run master loop function on a timer */
  while (1) { function_with_interval(&masterLoop, NULL, M_MASTER_LOOP_INTERVAL); };
}

/* Loop functions */
void masterLoop(void) {
  uint32_t currentTick = HAL_GetTick();
  int deltaTime = currentTick - prevTick;

  chassisLoop(local_rc_ctrl, deltaTime);
  turretLoop(local_rc_ctrl, deltaTime);
	refereeLoop();
	//read IMU
	BMI088_read(gyro, accel, &temp);
	
  prevTick = currentTick;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 6;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {

}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) { 

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
