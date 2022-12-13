/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#include "rtc.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "spi_tftlcd.h"
#include "GUI.h"
#include "stm32g4xx_hal_flash.h"
#include <string.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef DEFAULT
char *strx, *extstrx, *Readystrx;
char RxBuffer[1024], Rxcouter;
char *strstr(const char *, const char *);
uint8_t Res;
#endif

// UART variables
static uint8_t time_buffer[TIME_BUFFER_SIZE];
static uint8_t date_buffer[DATE_BUFFER_SIZE];
static uint8_t temp_buffer[TEMP_BUFFER_SIZE];
// Temp variables
static uint8_t update_temp;         // flag of updating temperature
static uint8_t display_temp;        // flag of displaying temperature
// LED variables
static uint8_t rgb_led_flag;        // flag of LED, 0-close, 1-RED, 2-GREEN, 3-BLUE
// RTC variables
RTC_DateTypeDef rtc_date;
RTC_TimeTypeDef rtc_time;
static uint8_t set_rtc_buffer[6];   // 6 parameters, year, month, day, hour, mintue, second
// FLASH variables
static uint8_t write_flash_flag;    // flag of writing data to flash
static uint8_t flash_buffer[BUFFER_SIZE];
static uint8_t flash_cot;           // max = 20
uint64_t write_flash_data = 0x3031323334353637;
uint32_t flash_addr = 0x0803F800;   // flash address, 0x08000000 - 0x080007FF 2k bank1 Page0
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
#ifdef DEFAULT
// Çå¿Õ»º³åÇø
void Clear_Buffer(void)
{
  uint8_t i;
	
  for (i = 0; i < Rxcouter; i++)
    RxBuffer[i] = 0;
  
  Rxcouter=0;
}
#endif

// UART output function
void UART_SendString(UART_HandleTypeDef *huart, char *str)
{
  if (HAL_UART_Transmit(huart, (uint8_t *)str, (uint16_t) strlen(str), HAL_MAX_DELAY) != HAL_OK) {
    printf("uart send string error\r\n");
    // Error_Handler();
  }
}

// get RTC datetime
void Get_RTC_Datetime(void)
{
  HAL_RTC_GetTime(&hrtc, &rtc_time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&hrtc, &rtc_date, RTC_FORMAT_BIN);
}

// display default information (Student ID, Date, Time)
void Display_Default_Info(void)
{
  Gui_DrawFont_GBK16(16, 20, BLUE, WHITE, (uint8_t *)"Student ID: 20221211");

  sprintf((char *)time_buffer, "Time: %02d:%02d:%02d\r\n", rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds);
  sprintf((char *)date_buffer, "Date: %04d-%02d-%02d\r\n", 2000 + rtc_date.Year, rtc_date.Month, rtc_date.Date);
  Gui_DrawFont_GBK16(80, 40, RED, WHITE, time_buffer);
  Gui_DrawFont_GBK16(80, 60, RED, WHITE, date_buffer);
}

// get temperature
float Get_Temp(void)
{
  uint16_t TS_CAL1, TS_CAL2;
  float temp;
  
  HAL_ADC_Start(&hadc5);
  HAL_ADC_PollForConversion(&hadc5, HAL_MAX_DELAY);
  temp = HAL_ADC_GetValue(&hadc5);
  TS_CAL1 = *(__IO uint16_t *)(0x1FFF75A8);
  TS_CAL2 = *(__IO uint16_t *)(0x1FFF75CA);
  temp = (130.0f - 30.0f) / (TS_CAL2 - TS_CAL1) * (temp - TS_CAL1) + 30.0f;
  return temp;
}

// trigger RTC Alarm each 5s
void RTC_Set_AlarmA(uint8_t sec)
{
	rtc_alarm.AlarmTime.Seconds = sec + 5;
  if (rtc_alarm.AlarmTime.Seconds >= 60) rtc_alarm.AlarmTime.Seconds -= 60;
  HAL_RTC_SetAlarm_IT(&hrtc, &rtc_alarm, RTC_FORMAT_BIN);
}

uint8_t Get_Month_BCD(uint8_t month)
{
  uint8_t month_bcd;
  switch (month)
  {
  case 1:
    month_bcd = RTC_MONTH_JANUARY;
    break;
  case 2:
    month_bcd = RTC_MONTH_FEBRUARY;
    break;
  case 3:
    month_bcd = RTC_MONTH_MARCH;
    break;
  case 4:
    month_bcd = RTC_MONTH_APRIL;
    break;
  case 5:
    month_bcd = RTC_MONTH_MAY;
    break;
  case 6:
    month_bcd = RTC_MONTH_JUNE;
    break;
  case 7:
    month_bcd = RTC_MONTH_JULY;
    break;
  case 8:
    month_bcd = RTC_MONTH_AUGUST;
    break;
  case 9:
    month_bcd = RTC_MONTH_SEPTEMBER;
    break;
  case 10:
    month_bcd = RTC_MONTH_OCTOBER;
    break;
  case 11:
    month_bcd = RTC_MONTH_NOVEMBER;
    break;
  case 12:
    month_bcd = RTC_MONTH_DECEMBER;
    break;
  default:
    month_bcd = RTC_MONTH_DECEMBER;
    break;
  }
  return month_bcd;
}

// Set RTC Datetime, 2022-01-10 23:50:00
void RTC_Set_Datetime(uint8_t year, uint8_t month, uint8_t date, uint8_t hours, uint8_t mintues, uint8_t seconds)
{
  if (HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A) != HAL_OK) {
    // Error_Handler();
    printf("deactivete alarm error.\r\n");
  }

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  uint8_t month_bcd = Get_Month_BCD(month);

  sTime.Hours = hours;
  sTime.Minutes = mintues;
  sTime.Seconds = seconds;
  sTime.SubSeconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    // Error_Handler();
    printf("set rtc time error.\r\n");
  }
  sDate.WeekDay = RTC_WEEKDAY_FRIDAY;
  sDate.Month = month_bcd;
  sDate.Date = date;
  sDate.Year = year;
  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    // Error_Handler();
    printf("set rtc date error.\r\n");
  }
  printf("set rtc success.\r\n");
}

void Erase_Flash() {
  HAL_FLASH_Unlock();

  FLASH_EraseInitTypeDef f;
	f.TypeErase = FLASH_TYPEERASE_PAGES;
	f.Banks = FLASH_BANK_1;
  f.Page = 126;
	f.NbPages = 2;	                    
	uint32_t PageError = 0;
	if (HAL_FLASHEx_Erase(&f, &PageError) != HAL_OK) {
    printf("erase error\r\n");
    // Error_Handler();
  }

  HAL_FLASH_Lock();
}

void Write_Flash(uint32_t flash_addr, uint64_t write_flash_data)
{	
  HAL_FLASH_Unlock();
 
  if (HAL_FLASH_Program(TYPEPROGRAM_DOUBLEWORD, flash_addr, write_flash_data) != HAL_OK) {
    printf("write error\r\n");
    // Error_Handler();
  }

  HAL_FLASH_Lock();
}

#define BYTE_COT 16
void Read_Flash(uint32_t write_flash_addr)
{
  uint8_t dis[BYTE_COT];
  for (int i = 0; i < BYTE_COT; i++) {
    dis[i] = *(__IO uint8_t*)(write_flash_addr + i);
  }
  printf(
    "20%c%c-%c%c-%c%c %c%c:%c%c:%c%c %c%c%c%c C\r\n", 
    dis[0], dis[1], dis[2], dis[3], dis[4], dis[5], dis[6], dis[7],
    dis[8], dis[9], dis[10], dis[11], dis[12], dis[13], dis[14], dis[15]
  );
  // printf(
  //   "%c%c:%c%c:%c%c %c%c C\r\n", 
  //   (char)dis[0], (char)dis[1], (char)dis[2], (char)dis[3],
  //   (char)dis[4], (char)dis[5], (char)dis[6], (char)dis[7]
  // );
}
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
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_TIM2_Init();
  MX_ADC5_Init();
  MX_RTC_Init();
  /* USER CODE BEGIN 2 */
  /* Question 1 */
  HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);
  HAL_Delay(100);
  float temp = Get_Temp();
  
  Lcd_Init();
	Lcd_Clear(WHITE);
  Get_RTC_Datetime();
  Display_Default_Info();

  Erase_Flash();

  /* get time and date*/
#ifdef QUESTION_2
  Get_RTC_DateTime();
  mem_second = rtc_time.Seconds;
  if (mem_second >= 55) mem_second = mem_second - 60;
#endif
  
  /*
   * reference code
   */
  // MX_RTC_Init();
  // MX_DMA_Init();
  // MX_ADC5_Init();
  // HAL_ADCEx_Calibration_Start(&hadc5, ADC_SINGLE_ENDED);
  // HAL_ADC_Start_DMA(&hadc5, (uint32_t *) ADC_Value, 3);

  // key_value = KEY_Scan(0);
  // if (key_value == KEY1_PRES)
  // {
  //   flag_user = 1;
  // }
  // HAL_UART_Receive_IT(&huart1, &recive, 1);
  // if (recive)
  // {
  //   recive = 0;
  //   UART_sendstring(1, usrt_buf);
  // }
  // Gui_DrawFont_GBK16(180, 40, RED, GRAY0, "Time:");
  // Gui_DrawFont_GBK16(180, 60, RED, GRAY0, "Freq:");

  // HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
  // HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
  // if (sTime.Seconds != Seconds) time_s++;
  // if (time_s % 5 == 0 && temp_flag == 1)
  // {
  //   temp_flag = 0;
  //   temp = get_temp();
  //   sprintf(temp_buf, "Temp: %.1f C\r\n", temp);
  //   UART_sendstring(1, temp_buf);
  // }
  // if (time_s % 5 != 0) temp_flag = 1;

  // if (time_s == 10)
  // {
  //   time_s = 0;
  //   data = temp;
  //   flash_page_erase(num, 2);

  //   flash_write(0 + num * 32, 2, data);
  //   data = sTime.Hours;
  //   flash_write(8 + num * 32, 2, data);

  //   data = sTime.Minutes;
  //   flash_write(16 + num * 32, 2, data);

  //   data = sTime.Seconds
  //   flash_write(24 + num * 32, 2, data);

  //   num++;
  //   if (num == 20) num = 0;
  // }

  // sprintf(time_buf, "Time: %02d:%02d:%02d\r\n", sTime.Hours, sTime.Minutes, sTime.Seconds);
  // sprintf(date_buf, "Date: %02d.%02d.%02d\r\n", 2000 + sDate.year, sDate.Month, sDate.Date);
  // Gui_DrawFont_GBK16(80, 40, RED, WHITE, time_buf);
  // Gui_DrawFont_GBK16(80, 60, RED, WHITE, date_buf);
  // if (flag_usr == 1)
  // {
  //   Gui_DrawFont_GBK16(80, 80, RED, WHITE, temp_buf);
  // }
  // Seconds = sTime.Seconds;

#ifdef DEFAULT
  HAL_UART_Receive_IT(&huart1, &Res, 1);
	HAL_TIM_Base_Start_IT(&htim2);
	Lcd_Init();
	Lcd_Clear(WHITE);
	
	Gui_DrawFont_GBK16(60,120,RED,GRAY0,(uint8_t *)"L610 Module checking");      // L610 Check
	HAL_Delay(3000);
	
	//	test uart output
	printf("AT\r\n");
	HAL_Delay(1000);
	strx=strstr((const char*)RxBuffer,(const char*)"OK");
	while(strx==NULL)
	{
		Clear_Buffer();	
		printf("AT\r\n");
		HAL_Delay(1000);		
		strx=strstr((const char*)RxBuffer,(const char*)"OK");
	}
	Lcd_Clear(WHITE);
	Gui_DrawFont_GBK16(60,120,RED,GRAY0,(uint8_t *)"Check success");
	HAL_Delay(2000);
	Clear_Buffer();
#endif
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
#ifdef DEFAULT
	Lcd_Clear(WHITE);
#endif
  
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* Question 2 */
// #ifdef QUESTION_2
    Get_RTC_Datetime();
    Display_Default_Info();
// #endif
    if (display_temp == 1) {  // display temperature
      memset(temp_buffer, 0, TEMP_BUFFER_SIZE);    // clear temperature buffer
      sprintf((char *)temp_buffer, "Temp: %4.1f C\r\n", temp);
      Gui_DrawFont_GBK16(80, 80, RED, WHITE, temp_buffer);    // display temperature
    } else {
      Gui_DrawFont_GBK16(80, 80, WHITE, WHITE, temp_buffer);    // clear temperature
    }

    if (update_temp == 1) {   // update temperature
      temp = Get_Temp();
      memset(tx_buffer, 0, TX_BUFFER_SIZE);  // clear tx buffer
      sprintf(
        (char *)tx_buffer, "Date: %04d-%02d-%02d Time: %02d:%02d:%02d, Temp: %.1f C\r\n", 
        2000 + rtc_date.Year, rtc_date.Month, rtc_date.Date, 
        rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, temp
      );
      UART_SendString(&huart1, (char *) tx_buffer);
      update_temp = 0;    // clear the temperature flag
    }

    if (rx_flag == 1) {
      if (rx_cot == 2) {
        int idx = atoi((char *)rx_buffer);
        // TODO: read_flash
        // read_flash
        printf("#%02d ", idx);
        Read_Flash(flash_addr + idx * 16);
      } else if (rx_cot == 19) {
        // Test function: print the received data
        // UART_SendString(&huart1, (char *)rx_buffer);
        int i = 0; 		
        char *s = strtok((char *)rx_buffer,"- :");  // separate symbol "- :"
        while (s != NULL) {   
			    set_rtc_buffer[i] = atoi(s);    // 2022-01-10 23:50:00
          i++;
          s = strtok(NULL, "- :");    // get other sub_string
			  }
        set_rtc_buffer[0] -= 2000;
			  RTC_Set_Datetime(
          set_rtc_buffer[0], set_rtc_buffer[1], set_rtc_buffer[2], 
          set_rtc_buffer[3], set_rtc_buffer[4], set_rtc_buffer[5]
        );    // set RTC datetime
        // test function
			  // printf(
        //   "%02d-%02d-%02d %02d:%02d:%02d  set success!\r\n", 
        //   set_rtc_buffer[0], set_rtc_buffer[1], set_rtc_buffer[2], 
        //   set_rtc_buffer[3],set_rtc_buffer[4], set_rtc_buffer[5]
        // );
        Get_RTC_Datetime();
        RTC_Set_AlarmA(set_rtc_buffer[5]);
        // test function
        // printf(
        //   "Date: %04d.%02d.%02d, Time: %02d:%02d:%02d\r\n", 
        //   2000 + rtc_date.Year, rtc_date.Month, rtc_date.Date, 
        //   rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds
        // );
      }
      rx_flag = 0; // clear RX flag
      // clear buffer
      memset(rx_buffer, 0, RX_BUFFER_SIZE);
      rx_cot = 0;
      // restart the DMA transfer, and 255 words each time
      HAL_UART_Receive_DMA(&huart1, rx_buffer, RX_BUFFER_SIZE);
    }
    
    if (write_flash_flag == 2) {
      // TODO: write flash
      memset(flash_buffer, 0, BUFFER_SIZE);
      sprintf(
        (char *)flash_buffer, "%02d%02d%02d%02d%02d%02d%4.1f", 
        rtc_date.Year, rtc_date.Month, rtc_date.Date,
        rtc_time.Hours, rtc_time.Minutes, rtc_time.Seconds, temp
      );
      // printf("Second: %02d, #%02d write flash\r\n", rtc_time.Seconds, flash_cot);   // Test function
      // UART_SendString(&huart1, (char *)flash_buffer);
      // printf("%x\r\n", write_flash_data);
      Write_Flash(flash_addr + flash_cot * 16, *(uint64_t *)flash_buffer);
      Write_Flash(flash_addr + flash_cot * 16 + 8, *((uint64_t *)(flash_buffer + 8)));
      // printf("0x%x\r\n", flash_addr + flash_cot * 8);
      // Read_Flash(flash_addr + flash_cot * 8);

      flash_cot++;    // update
      if (flash_cot == 20) flash_cot = 0;
      write_flash_flag = 0;
    }

#ifdef DEFAULT
		LED_RED_TOGGLE();
		HAL_Delay(500);
		
		Gui_DrawFont_GBK16(60,120,RED,GRAY0,(uint8_t *)"Start your work!"); 
#endif
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
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 12;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
// UART IRQ callback function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) 
{
  UNUSED(huart); 
#ifdef DEFAULT 
  if(huart->Instance == USART1)
  { 
    RxBuffer[Rxcouter++] = Res; 
    HAL_UART_Receive_IT(&huart1, (uint8_t *)&Res, 1); 
  }
#endif
}

// TIMER IRQ callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
#ifdef DEFAULT
   if(htim->Instance == TIM2)
   {
   }
#endif
}

// GPIO EXTI IRQ callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  UNUSED(GPIO_Pin);

  switch(GPIO_Pin) {
  case KEY1_Pin:
    display_temp = (display_temp) ? 0 : 1;
    // display_temp = 1;
    break;
  case KEY2_Pin:
    rgb_led_flag = (++rgb_led_flag) % 4;
    // HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    switch (rgb_led_flag) {
    case 1:
      LED_RED_ON();
      LED_GREEN_OFF();
      LED_BLUE_OFF();
      break;
    case 2:
      LED_RED_ON();
      LED_GREEN_ON();
      LED_BLUE_OFF();
      break;
    case 3:
      LED_RED_OFF();
      LED_GREEN_OFF();
      LED_BLUE_ON();
      break;
    default:
      LED_RED_OFF();
      LED_GREEN_OFF();
      LED_BLUE_OFF();
      break;
    }
    break;
  case KEY3_Pin:
    break;
  default:
    break;
  }
}

// rtc alarmA callback function
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	Get_RTC_Datetime();   // update datetime
  // printf("Second: %02d, display temp\r\n", rtc_time.Seconds);    // test function
  update_temp = 1;
  write_flash_flag++;
	RTC_Set_AlarmA(rtc_time.Seconds);
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
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
