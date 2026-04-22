/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    noteI2c.c
  * @brief   This file provides code for the I2C interface to Blues Notecard
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
#include "noteI2c.h"
#include "note.h"
#include "i2c.h"
#include <stdlib.h>
#include <string.h>

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* USER CODE BEGIN 1 */

// Note-c I2C interface functions

// I2C reset procedure, called before any I/O and called again upon I/O error
bool noteI2CReset(uint16_t DevAddress) {
    HAL_I2C_DeInit(&hi2c1);
    MX_I2C1_Init();
    return true;
}

// Transmits in master mode an amount of data, in blocking mode.     The address
// is the actual address; the caller should have shifted it right so that the
// low bit is NOT the read/write bit. An error message is returned, else NULL if success.
const char *noteI2CTransmit(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size) {
    char *errstr = NULL;
    int writelen = sizeof(uint8_t) + Size;
    uint8_t *writebuf = malloc(writelen);
    if (writebuf == NULL) {
        errstr = "i2c: insufficient memory (write)";
    } else {
        writebuf[0] = Size;
        memcpy(&writebuf[1], pBuffer, Size);
        HAL_StatusTypeDef err_code = HAL_I2C_Master_Transmit(&hi2c1, DevAddress<<1, writebuf, writelen, 250);
        free(writebuf);
        if (err_code != HAL_OK) {
            errstr = "i2c: HAL_I2C_Master_Transmit error";
        }
    }
    return errstr;
}

// Receives in master mode an amount of data in blocking mode. An error mesage returned, else NULL if success.
const char *noteI2CReceive(uint16_t DevAddress, uint8_t* pBuffer, uint16_t Size, uint32_t *available) {
    const char *errstr = NULL;
    HAL_StatusTypeDef err_code;

    // Retry transmit errors several times, because it's harmless to do so
    for (int i=0; i<3; i++) {
        uint8_t hdr[2];
        hdr[0] = (uint8_t) 0;
        hdr[1] = (uint8_t) Size;
        HAL_StatusTypeDef err_code = HAL_I2C_Master_Transmit(&hi2c1, DevAddress<<1, hdr, sizeof(hdr), 250);
        if (err_code == HAL_OK) {
            errstr = NULL;
            break;
        }
        errstr = "i2c: HAL_I2C_Master_Transmit error";
    }

    // Only receive if we successfully began transmission
    if (errstr == NULL) {

        int readlen = Size + (sizeof(uint8_t)*2);
        uint8_t *readbuf = malloc(readlen);
        if (readbuf == NULL) {
            errstr = "i2c: insufficient memory (read)";
        } else {
            err_code = HAL_I2C_Master_Receive(&hi2c1, DevAddress<<1, readbuf, readlen, 10);
            if (err_code != HAL_OK) {
                errstr = "i2c: HAL_I2C_Master_Receive error";
            } else {
                uint8_t availbyte = readbuf[0];
                uint8_t goodbyte = readbuf[1];
                if (goodbyte != Size) {
                    errstr = "i2c: incorrect amount of data";
                } else {
                    *available = availbyte;
                    memcpy(pBuffer, &readbuf[2], Size);
                }
            }
            free(readbuf);
        }
    }

    // Done
    return errstr;
}

/* USER CODE END 1 */