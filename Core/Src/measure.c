/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    measure.c
  * @brief   This file provides code for measurement handling
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
#include "measure.h"
#include "usart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

// Initialize the random number generator
void initMeasure(void) {
    // Seed the random generator with current system tick
    srand(HAL_GetTick());
}

uint8_t response[100]; // Serial port data reception
uint8_t dataRequest[5] = {
  0x18,0x05, 0x00, 0x01 ,0x0D
};
uint8_t adRequest[5] = {
  0x18,0x05, 0x00, 0x02 ,0x0D
};

long requestProbe(int sensor, int what) {
	// if what == 0 => request data
	// if what == 1 => request AD
	uint8_t *request;
	uint8_t respLength;
	UART_HandleTypeDef *uart;

	if (sensor == SENSOR_180) {
		uart = &huart1;
	} else {
		uart = &huart2;
	}

	if (what == 0) {
		request = dataRequest;
		respLength = 5;
	} else {
		request = adRequest;
		respLength = 6;
	}

	HAL_StatusTypeDef statusTransmit, statusRecept;
	// first send the request for getting the last measure
	statusTransmit = HAL_UART_Transmit(uart, (uint8_t*)request, 5, 1000);
	if (statusTransmit != HAL_OK) {
		Error_Handler();
	}
	// clear the response buffer
	memset(response, 0, sizeof(response));
	// now wait for the response
	statusRecept = HAL_UART_Receive(uart, (uint8_t*)response, respLength, 1000);
//	if (statusRecept != HAL_OK) {
//		Error_Handler();
//	}
	long r = response[2];
	r = r *256;
	r += response[3];
	if (respLength>5) {
		r = r *256;
		r += response[4];
	}
	return r;
}



long getMeasure(int sensor) {

	long probe = requestProbe(sensor, 0);
	  // HAL_Delay(300);
	long probe_ad = requestProbe(sensor, 1);

    return probe_ad;
}

/* USER CODE END 1 */