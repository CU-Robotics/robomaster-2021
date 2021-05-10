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
#include "usart.h"
#include "math.h"

#include "remote_control.h"
#include "bsp_usart.h"
#include "bsp_can.h"
#include "CAN_receive.h"

#include "chassis.h"
#include "turret.h"
#include "pid.h"

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

const RC_ctrl_t *local_rc_ctrl;

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void) {
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
  MX_USART3_UART_Init();

	can_filter_init();

	remote_control_init();
	usart1_tx_dma_init();
	local_rc_ctrl = get_remote_control_point();

	float yawHoldPosition = 0;
	float pitchHoldPosition = 0;

  /* Infinite loop */
  while (1) {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		//CAN_cmd_chassis(1000, 1000, 1000, 1000);
		/*HAL_Delay(1000);
		//CAN_cmd_gimbal(10000, 10000, 10000, 10000);
		//HAL_Delay(1);
		//CAN_cmd_chassis(-400,-400,-1000,-400);
		// ecd is encoder position with 16 bit accuracy
		int16_t var = get_chassis_motor_measure_point(2)->ecd;
		printf("andle /d",var);
		printf("print");*/
		//HAL_Delay(1000);

    //if (local_rc_ctrl->rc.s[0]) {
    float xThrottle = (local_rc_ctrl->rc.ch[2] * 10 / 16384.0);
    float yThrottle = (local_rc_ctrl->rc.ch[3] * 10 / 16384.0);
    float rotation = (local_rc_ctrl->rc.ch[1] * 10 / 16384.0);

		//
    yawHoldPosition += ((local_rc_ctrl->rc.ch[2])/10000.0f);
		//create true yaw around circle
		/*
		if(yawHoldPosition > 8194)
			yawHoldPosition = 0 ;
		if(yawHoldPosition < 0)
			yawHoldPosition = 8194;
		*/
    pitchHoldPosition += ((local_rc_ctrl->rc.ch[3])/10000.0f);
		if(pitchHoldPosition > 8191)
			pitchHoldPosition -= 8191;
		if(pitchHoldPosition < 0)
			pitchHoldPosition += 8191;
		
    Chassis chassis = calculateMecanum(xThrottle, yThrottle, rotation);
    Turret turret = calculateTurret((int16_t) yawHoldPosition, (int16_t) pitchHoldPosition);

    //CAN_cmd_chassis((int16_t) (chassis.frontRight * 16384), (int16_t) (chassis.backRight * 16384), (int16_t) (chassis.backLeft * 16384), (int16_t) (chassis.frontLeft * 16384));
    CAN_cmd_gimbal_working((int16_t) (turret.yaw), (int16_t) (turret.pitch), 0, 0);//(int16_t) (turret.pitch));
	}
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/