/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>

#include "DBG.h"

#include "PCF2123.h"
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
pcf2123_error_t my_spi_xfer(uint8_t *write, uint8_t *read, size_t xfer_len, uint32_t timeout_ms);
void my_control_ce(pcf2123_ce_t ce_state);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
pcf2123_t my_pcf;

volatile bool pcf_alarm = false;
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
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  DBG_init(&huart2);
  DBG_println("PCF2123 demo project");

  /* Wait for Oscilator to become stable */
  HAL_Delay(1000 * 2);

  /* Register HAL callbacks */
  PCF2123_init(&my_pcf, my_spi_xfer, my_control_ce);

  PCF2123_sw_reset(&my_pcf);

  /* Wait for Oscilator to become stable */
  HAL_Delay(1000 * 2);

  uint8_t control_1 = 0xff;
  uint8_t control_2 = 0xff;
  uint8_t osc_status = 0xff;

  PCF2123_read_register(&my_pcf, PCF2123_REG_CONTROL_1,
	&control_1, 1);
  PCF2123_read_register(&my_pcf, PCF2123_REG_CONTROL_2,
	&control_2, 1);
  PCF2123_read_register(&my_pcf, PCF2123_REG_SECONDS,
  	&osc_status, 1);

  DBG_println("C1: %x, C2: %x", control_1, control_2);
  DBG_println("Osc sts: %x", osc_status);

  if (PCF2123_AF_INT_GENERATED & control_2) {
	  control_2 &= ~(PCF2123_AF_INT_GENERATED);
	  DBG_println("C2: %x", control_2);
	  PCF2123_write_register(&my_pcf, PCF2123_REG_CONTROL_2,
		&control_2, 1);

	  PCF2123_read_register(&my_pcf, PCF2123_REG_CONTROL_2,
		&control_2, 1);

	  DBG_println("C2: %x", control_2);
  }

  if (PCF2123_OS_INTEGRITY_NOT_GUARANTEED & osc_status) {
	  osc_status &= ~(PCF2123_OS_INTEGRITY_NOT_GUARANTEED);
	  DBG_println("Osc sts: %x", osc_status);
	  PCF2123_write_register(&my_pcf, PCF2123_REG_SECONDS,
		&osc_status, 1);
  }

  osc_status = 0xff;
  PCF2123_read_register(&my_pcf, PCF2123_REG_SECONDS,
  	&osc_status, 1);

  DBG_println("Osc sts: %x", osc_status);

  pcf2123_time_t time = {
		  .sec = 56,
		  .min = 10,
		  .hour = 0
  };

  pcf2123_date_t date = {
		  .day = 17,
		  .weekday = PCF2123_WEEKDAY_SUNDAY,
		  .month = PCF2123_MONTH_OCTOBER,
		  .year = 20
  };

  PCF2123_set_rtcc_data(&my_pcf, &time, &date);

  pcf2123_alarm_conf_t alarm = {
		  .alarm_enable = PCF2123_ALARM_MIN_ENABLE,
		  .min = 11,
  };

  PCF2123_set_alarm_interrupt(&my_pcf, &alarm);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  HAL_GPIO_TogglePin(HEART_GPIO_Port, HEART_Pin);

	  uint8_t flags = PCF2123_get_interrupt_flags(&my_pcf);

	  /* AF */
	  if (PCF2123_AF_INT_GENERATED & flags) {
		  PCF2123_clear_af(&my_pcf);
	  }

	  /* TF */
	  if (PCF2123_TF_INTERRUPT_GENERTED & flags) {
		  PCF2123_clear_tf(&my_pcf);
	  }

	  pcf2123_time_t current_time;
	  pcf2123_date_t current_date;

	  PCF2123_get_rtcc_data(&my_pcf, &current_time, &current_date);

	  DBG_println("Hour: %d, Min: %d, Sec: %d",
			  current_time.hour, current_time.min, current_time.sec);
	  DBG_println("Day: %d, Weekday: %d, month: %d, year: %d",
			  current_date.day, current_date.weekday, current_date.month, current_date.year);

	  HAL_Delay(1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
#if 0
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	DBG_println("a");

	if (INT_Pin == GPIO_Pin) {
		DBG_println("here!");
		pcf_alarm = true;
	}
}
#endif

void PCF2123_on_assertion(void)
{

}

pcf2123_error_t my_spi_xfer(uint8_t *write, uint8_t *read, size_t xfer_len, uint32_t timeout_ms)
{
	PCF2123_ASSERT(write);
	PCF2123_ASSERT(read);

	pcf2123_error_t retval;
	HAL_StatusTypeDef xfer_sts;

	HAL_GPIO_WritePin(HEART_GPIO_Port, HEART_Pin, GPIO_PIN_RESET);
	xfer_sts = HAL_SPI_TransmitReceive(&hspi1, write, read, xfer_len, timeout_ms);
	HAL_GPIO_WritePin(HEART_GPIO_Port, HEART_Pin, GPIO_PIN_SET);

	if (HAL_TIMEOUT == xfer_sts) {
		retval = PCF2123_ETIMEOUT;
	} else if (HAL_OK == xfer_sts) {
		retval = PCF2123_ENONE;
	}

	return retval;
}

void my_control_ce(pcf2123_ce_t ce_state)
{
	if (PCF2123_CE_ENABLE == ce_state) {
		HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_SET);
	} else if (PCF2123_CE_DISABLE == ce_state) {
		HAL_GPIO_WritePin(CE_GPIO_Port, CE_Pin, GPIO_PIN_RESET);
	} else {
		/* NOTE: You should never reach this point */
		PCF2123_ASSERT(0);
	}
}
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
