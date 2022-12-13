/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart1;

/* USER CODE BEGIN Private defines */
#define TX_BUFFER_SIZE BUFFER_SIZE
#define RX_BUFFER_SIZE BUFFER_SIZE
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint8_t tx_buffer[TX_BUFFER_SIZE];
extern uint8_t rx_buffer[RX_BUFFER_SIZE];
extern uint8_t rx_cot;
extern uint8_t rx_flag;

void USER_UART_IRQHandler(UART_HandleTypeDef *huart);   // UART IDLE IRQ handler
void USER_UART_IdleCallback(UART_HandleTypeDef *huart); // IDLE IRQ callback function
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

