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
#include "adc.h"
#include "dma.h"
#include "eth.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RXBUFFERSIZE 1
// ADC读取内部温度传感器值，以及内部参考电压值
#define AD_DMA_SIZE 2
#define USART_REC_LEN 100
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF1E820))
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF1E840))
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FF1E860))
#if defined( __ICCARM__ )
#define DMA_BUFFER \
      _Pragma("location=\".dma_buffer\"")
#else
#define DMA_BUFFER \
      __attribute__((section(".dma_buffer")))
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t aRxBuffer[RXBUFFERSIZE] DMA_BUFFER;
uint16_t USART_RX_STA = 0;
uint8_t USART_RX_BUF[USART_REC_LEN];
uint8_t USART_TX_BUF[USART_REC_LEN] DMA_BUFFER;
uint16_t AD_Value = 0;
uint32_t AD_DMA_Value[AD_DMA_SIZE] DMA_BUFFER;
float Temperature_Vlaue = 0;
float VREFINT_Vlaue = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
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
	MX_DMA_Init();
	MX_ADC3_Init();
	MX_ETH_Init();
	MX_USART3_UART_Init();
	MX_USB_OTG_FS_PCD_Init();
	MX_TIM6_Init();
	/* USER CODE BEGIN 2 */
	HAL_UART_Receive_DMA(&huart3, (uint8_t *) aRxBuffer, RXBUFFERSIZE);
	ADC_ConfigureBoostMode(&hadc3);
	HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET_LINEARITY,
	ADC_SINGLE_ENDED);
	HAL_TIM_Base_Start_IT(&htim6);
	HAL_ADC_Start_DMA(&hadc3, (uint32_t*) &AD_DMA_Value, AD_DMA_SIZE);

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Supply configuration update enable
	 */
	HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
	/** Configure the main internal regulator output voltage
	 */
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {
	}
	/** Macro to configure the PLL clock source
	 */
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 24;
	RCC_OscInitStruct.PLL.PLLP = 2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	RCC_OscInitStruct.PLL.PLLR = 2;
	RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
	RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
	RCC_OscInitStruct.PLL.PLLFRACN = 0;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1
			| RCC_CLOCKTYPE_D1PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
	RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART3
			| RCC_PERIPHCLK_ADC | RCC_PERIPHCLK_USB;
	PeriphClkInitStruct.PLL2.PLL2M = 1;
	PeriphClkInitStruct.PLL2.PLL2N = 21;
	PeriphClkInitStruct.PLL2.PLL2P = 7;
	PeriphClkInitStruct.PLL2.PLL2Q = 2;
	PeriphClkInitStruct.PLL2.PLL2R = 2;
	PeriphClkInitStruct.PLL2.PLL2RGE = RCC_PLL2VCIRANGE_3;
	PeriphClkInitStruct.PLL2.PLL2VCOSEL = RCC_PLL2VCOMEDIUM;
	PeriphClkInitStruct.PLL2.PLL2FRACN = 0;
	PeriphClkInitStruct.Usart234578ClockSelection =
	RCC_USART234578CLKSOURCE_D2PCLK1;
	PeriphClkInitStruct.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
	PeriphClkInitStruct.AdcClockSelection = RCC_ADCCLKSOURCE_PLL2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Enable USB Voltage detector
	 */
	HAL_PWREx_EnableUSBVoltageDetector();
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
	if (htim->Instance == htim6.Instance) {
		/* USER CODE HERE */
		static int count;
		static int count_sum = 1000 - 1;
		if (count < count_sum) {
			count++;
		} else {
			count = 0;
			Temperature_Vlaue = (float) (110 - 30)
					/ (float) (*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR)
					* (float) (AD_DMA_Value[0] - *TEMP30_CAL_ADDR) + 30;
			VREFINT_Vlaue = (float) 3.3 * (float) *VREFINT_CAL_ADDR
					/ (float) AD_DMA_Value[1];
			memset(USART_TX_BUF, 0, sizeof(USART_TX_BUF));
			sprintf(USART_TX_BUF,
					"MCU Temperature %09.5f℃ VREFINT_Vlaue %09.5f\r\n",
					Temperature_Vlaue, VREFINT_Vlaue);
			HAL_UART_Transmit_DMA(&huart3, (uint8_t *) USART_TX_BUF,
					strlen(USART_TX_BUF));
		}
		/* USER CODE HERE */
	}
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == huart3.Instance) {
		if ((USART_RX_STA & 0x8000) == 0) {
			if (USART_RX_STA & 0x4000) {
				if (aRxBuffer[0] != 0x0a)
					USART_RX_STA = 0;
				else
					USART_RX_STA |= 0x8000;
			} else {
				if (aRxBuffer[0] == 0x0d) {
					USART_RX_STA |= 0x4000;
				} else {
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = aRxBuffer[0];
					USART_RX_STA++;
					if (USART_RX_STA > (USART_REC_LEN - 1)) {
						USART_RX_STA = 0;
					}
				}
			}
		}
		if ((USART_RX_STA & 0x8000) != 0) {
			memcpy(USART_TX_BUF, USART_RX_BUF, (USART_RX_STA & 0x3FFF));
			USART_TX_BUF[(USART_RX_STA & 0x3FFF)] = 0x0d;
			USART_TX_BUF[(USART_RX_STA & 0x3FFF) + 1] = 0x0a;
			HAL_UART_Transmit_DMA(&huart3, (uint8_t *) USART_TX_BUF,
					((USART_RX_STA & 0x3FFF) + 2));
			USART_RX_STA = 0;
		}
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == huart3.Instance) {

	}
}
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc) {
	if (hadc->Instance == hadc3.Instance) {

	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
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
