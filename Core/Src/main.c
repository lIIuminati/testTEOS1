/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "button.h"
#include "measure.h"
#include "note.h"
#include "noteI2C.h"
#include "notecard.h"
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

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

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//HardwareSerial stlinkSerial(PIN_VCP_RX, PIN_VCP_TX);

//void setup() {
  //stlinkSerial.begin(115200);
  //stlinkSerial.println("ST-LINK VCP test");


void UX_signal(int length) {
    HAL_GPIO_WritePin(TEST_SGN_GPIO_Port, TEST_SGN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    HAL_Delay(100*length);
    HAL_GPIO_WritePin(TEST_SGN_GPIO_Port, TEST_SGN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
}

void flipLED(int delay) {
    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
    if (delay > 0) {
        HAL_Delay(delay*100);
    }
}

void LEDon(void) {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
}

void LEDoff(void) {
    HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
}

void logInfo(const char* message, int severity) {
    char buffer[128];
    sprintf(buffer, "LOG: %s\r\n", message);
    LEDon();
    HAL_UART_Transmit(&hlpuart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
    LEDoff();
    //UX_signal(10);
    for(int i=0; i<severity*2; i++) {
        flipLED(1);
    }
    LEDoff();
    //UX_signal(10);
}

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
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */

  // Initialize button interrupt handling
  buttonInit();

  // Initialize measure module
  initMeasure();

  // Initialize note-c library
  NoteSetFn(malloc, free, HAL_Delay, HAL_GetTick);
  NoteSetFnI2C(NOTE_I2C_ADDR_DEFAULT, NOTE_I2C_MAX_DEFAULT, noteI2CReset, noteI2CTransmit, noteI2CReceive);

  const char *errorInfo;
  errorInfo = noteCardCheckInit();
  if (errorInfo != NULL) {
    logInfo(errorInfo, 10);
  }
  int timePressed = 0;
  char buf[100];
  int len;
  int i = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    while (1) {
    //long value = getMeasure(SENSOR_180);
    //logInfo("Waiting for button press...", 1);
    //HAL_Delay(100);

      uint8_t data;
    // Check for button press event
    LEDoff();
    timePressed = 0;
    while (buttonIsDown()) {
      flipLED(1);
      timePressed++;
    }
    if (timePressed > 2) {
      LEDon();
          // Button press detected - perform action
      long value = getMeasure(SENSOR_180);
      float_t turbidityValue = (float_t)value / 255.0f * 100.0f; // Example conversion to percentage
      //errorInfo = sendMeasure(value, value, turbidityValue);
      sprintf(buf, "Sensor 180, Probe value = %d - AD = %d \r\n", -1, value);
      logInfo(buf, 1);
      
      value = getMeasure(SENSOR_90);
      sprintf(buf, "Sensor 90, Probe value = %d - AD = %d \r\n", -1, value);
      logInfo(buf, 1);

      LEDoff();
      if (errorInfo != NULL) {
        logInfo(errorInfo, 3);
      }
      buttonClearFlag();
    }
  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == USER_BTN_Pin) {
    uint32_t currentTime = HAL_GetTick();
    flipLED(0);

    // Debounce: ignore if interrupt occurred within BUTTON_DEBOUNCE_TIME_MS
    // if ((currentTime - lastButtonPressTime) >= BUTTON_DEBOUNCE_TIME_MS) {
    //   // Check if button is actually pressed (low on PC13)
    //   if (HAL_GPIO_ReadPin(USER_BTN_GPIO_Port, USER_BTN_Pin) ==
    //       GPIO_PIN_RESET) {
    //     buttonPressed = true;
    //     lastButtonPressTime = currentTime;
    //   }
    // }
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
  HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
  HAL_GPIO_WritePin(TEST_SGN_GPIO_Port, TEST_SGN_Pin, GPIO_PIN_SET);
  __disable_irq();
  while (1) {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
