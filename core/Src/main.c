/* USER CODE BEGIN Header */
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "gpio.h"
#include "dma.h"
#include "usart.h"
#include "math.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "remote_control.h"
#include "bsp_usart.h"
#include "bsp_can.h"
#include "CAN_receive.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

const RC_ctrl_t *local_rc_ctrl;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
	MX_DMA_Init();
	MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	can_filter_init();
	/* USER CODE END 2 */


	/* USER CODE BEGIN 2 */
	remote_control_init();
	usart1_tx_dma_init();
	local_rc_ctrl = get_remote_control_point();

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
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

		// Step 1: Split the desired velocity vector (magnitude and direction) into it's x and y components
		float xThrottle = (local_rc_ctrl->rc.ch[0] / 16384.0);
		float yThrottle = (local_rc_ctrl->rc.ch[1] / 16384.0);
		float rotation = (local_rc_ctrl->rc.ch[2] / 16384.0);

		// Step 2: Calculate the power for each wheel (on a scale of -1.0 to 1.0)
		float frontRight = xThrottle - yThrottle - rotation;
		float frontLeft = xThrottle + yThrottle + rotation;
		float backRight = xThrottle + yThrottle - rotation;
		float backLeft = xThrottle - yThrottle + rotation;

		// Step 3: Normalize motor power (due to the previous step, some of the motor power variables may have a value higher than 1.0 or lower than -1.0)

		// Find the highest magnitude (max) value in the four power variables.
		float max = abs(frontRight);
		if (abs(frontLeft) > max) max = abs(frontLeft);
		if (abs(backRight) > max) max = abs(backRight);
		if (abs(backLeft) > max) max = abs(backLeft);

		// If the max value is greater than 1.0, divide all the motor power variables by the max value, forcing all magnitudes to be less than or equal to 1.0.
		if (max > 1.0) {
				frontRight /= max;
				frontLeft /= max;
				backRight /= max;
				backLeft /= max;
		}

		CAN_cmd_chassis((int16_t) (frontRight * 16384), (int16_t) (backRight * 16384), (int16_t) (backLeft * 16384), (int16_t) (frontLeft * 16384));
	}
  /* USER CODE END 3 */
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
