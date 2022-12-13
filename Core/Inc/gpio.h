/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void LED_RED_TOGGLE(void);
void LED_ON(uint16_t GPIO_Pin);
void LED_OFF(uint16_t GPIO_Pin);

#define LED_RED_ON()    LED_ON(LED_RED_Pin)
#define LED_GREEN_ON()  LED_ON(LED_GREEN_Pin)
#define LED_BLUE_ON()   LED_ON(LED_BLUE_Pin)
#define LED_ORANGE_ON() LED_ON(LED_ORANGE_Pin)

#define LED_RED_OFF()   LED_OFF(LED_RED_Pin)
#define LED_GREEN_OFF() LED_OFF(LED_GREEN_Pin)
#define LED_BLUE_OFF()   LED_OFF(LED_BLUE_Pin)
#define LED_ORANGE_OFF() LED_OFF(LED_ORANGE_Pin)
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

