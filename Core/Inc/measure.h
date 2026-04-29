/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    measure.h
  * @brief   This file contains the function prototypes for
  *          the measure.c file
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
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MEASURE_H__
#define __MEASURE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdint.h>

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

// Initialize the random number generator
#define SENSOR_180 0
#define SENSOR_90 1
void initMeasure();

// Get a random measurement value (0-255) and output to UART2
long getMeasure(int sensor);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __MEASURE_H__ */