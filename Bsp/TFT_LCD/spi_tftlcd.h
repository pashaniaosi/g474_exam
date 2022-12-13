

#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_
#include "main.h"

/////////////////////////////////////用户配置区///////////////////////////////////	 
//支持横竖屏快速定义切换
#define USE_HORIZONTAL  		1
//定义是否使用横屏 		0,不使用.1,使用.

//-----------------------------SPI 总线配置--------------------------------------//
#define USE_HARDWARE_SPI     1  //1:Enable Hardware SPI;0:USE Soft SPI

//-------------------------屏幕物理像素设置--------------------------------------//
#define LCD_X_SIZE	        320
#define LCD_Y_SIZE	        240

#if USE_HORIZONTAL//如果定义了横屏 
#define X_MAX_PIXEL	        LCD_Y_SIZE
#define Y_MAX_PIXEL	        LCD_X_SIZE
#else
#define X_MAX_PIXEL	        LCD_X_SIZE
#define Y_MAX_PIXEL	        LCD_Y_SIZE
#endif
//////////////////////////////////////////////////////////////////////////////////
#define RED  	0xf800
#define GREEN	0x07e0
#define BLUE 	0x001f
#define WHITE	0xffff
#define BLACK	0x0000
#define YELLOW  0xFFE0
#define GRAY0   0xEF7D   			//灰色0 3165 00110 001011 00101
#define GRAY1   0x8410      	//灰色1      00000 000000 00000
#define GRAY2   0x4208      	//灰色2  1111111111011111

/*这部分用于软件模拟SPI*/
//#define LCD_RS         	GPIO_PIN_6	//PB14连接至TFT --DC
//#define LCD_SCL        	GPIO_PIN_5	//PC10连接至TFT --CLK
//#define LCD_SDA        	GPIO_PIN_7	//PC12连接至TFT - SDI
//#define LCD_RST         GPIO_PIN_4

////液晶控制口置1操作语句宏定义
//#define	LCD_RS_SET  	 GPIOA->BSRR=LCD_RS
//#define	LCD_SDA_SET  	 GPIOA->BSRR=LCD_SDA
//#define	LCD_SCL_SET    GPIOA->BSRR=LCD_SCL
//#define LCD_RST_SET    GPIOA->BSRR=LCD_RST

////液晶控制口置0操作语句宏定义
//#define	LCD_RS_CLR  	 GPIOA->BRR=LCD_RS
//#define	LCD_SDA_CLR  	 GPIOA->BRR=LCD_SDA
//#define	LCD_SCL_CLR  	 GPIOA->BRR=LCD_SCL
//#define LCD_RST_CLR    GPIOA->BRR=LCD_RST

/*这部分用于硬件SPI中普通管脚部分*/
#define	LCD_RS_SET  	 LCD_RS_GPIO_Port->BSRR=LCD_RS_Pin
#define	LCD_RS_CLR  	 LCD_RS_GPIO_Port->BRR=LCD_RS_Pin
#define LCD_RST_SET    LCD_RST_GPIO_Port->BSRR=LCD_RST_Pin
#define LCD_RST_CLR    LCD_RST_GPIO_Port->BRR=LCD_RST_Pin



void LCD_GPIO_Init(void);
void LCD_WR_REG(uint8_t Index);
void LCD_WR_DATA(uint8_t Data);
void Lcd_WriteReg(uint8_t Index,uint8_t Data);
uint16_t Lcd_ReadReg(uint8_t LCD_Reg);
void Lcd_Reset(void);
void Lcd_Init(void);
void Lcd_Clear(uint16_t Color);
void Lcd_SetXY(uint16_t x,uint16_t y);
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data);
unsigned int Lcd_ReadPoint(uint16_t x,uint16_t y);
void Lcd_SetRegion(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void Lcd_WriteData_16Bit(uint16_t Data);

#endif
