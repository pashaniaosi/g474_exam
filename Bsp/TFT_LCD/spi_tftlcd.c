#include "main.h"
#include "spi_tftlcd.h"
#include "spi.h"
#include "gpio.h"

/***************************************************************************************

****************************************************************************************/

//---------------------------------function----------------------------------------------------//

/****************************************************************************
* 名    称：void LCD_GPIO_Init(void)
* 功    能：STM32_模拟SPI所用到的GPIO初始化
* 入口参数：无
* 出口参数：无
* 说    明：初始化模拟SPI所用的GPIO
****************************************************************************/
void LCD_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_Initure;
//	__HAL_RCC_GPIOB_CLK_ENABLE();					//开启GPIOB时钟
	__HAL_RCC_GPIOA_CLK_ENABLE();					//开启GPIOA时钟

	GPIO_Initure.Pin=GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_4;		
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  		//推挽输出
//	GPIO_Initure.Pull=GPIO_PULLUP;         			//上拉
	GPIO_Initure.Speed=GPIO_SPEED_FREQ_VERY_HIGH;  	//高速
	HAL_GPIO_Init(GPIOA,&GPIO_Initure);
      
}
/****************************************************************************
* 名    称：void  SPIv_WriteData(uint8_t Data)
* 功    能：STM32_模拟SPI写一个字节数据底层函数
* 入口参数：Data
* 出口参数：无
* 说    明：STM32_模拟SPI读写一个字节数据底层函数
****************************************************************************/
void  SPIv_WriteData(uint8_t Data)
{

}

/****************************************************************************
* 名    称：uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte)
* 功    能：STM32_硬件SPI读写一个字节数据底层函数
* 入口参数：SPIx,Byte
* 出口参数：返回总线收到的数据
* 说    明：STM32_硬件SPI读写一个字节数据底层函数
****************************************************************************/
uint8_t SPI_WriteByte(SPI_TypeDef* SPIx,uint8_t Byte)
{
	HAL_SPI_Transmit(&hspi1,&Byte,1,0XFF);
	return 0;
} 

/****************************************************************************
* 名    称：void SPI_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet)
* 功    能：设置SPI的速度
* 入口参数：SPIx,SpeedSet
* 出口参数：无
* 说    明：SpeedSet:1,高速;0,低速;
****************************************************************************/
void SPI_SetSpeed(SPI_TypeDef* SPIx,uint8_t SpeedSet)
{

} 

/****************************************************************************
* 名    称：SPI1_Init(void)
* 功    能：STM32_SPI1硬件配置初始化
* 入口参数：无
* 出口参数：无
* 说    明：STM32_SPI1硬件配置初始化
****************************************************************************/
void SPI1_Init(void)	
{

}

/****************************************************************************
* 名    称：Lcd_WriteIndex(uint8_t Index)
* 功    能：向液晶屏写一个8位指令
* 入口参数：Index   寄存器地址
* 出口参数：无
* 说    明：调用前需先选中控制器，内部函数
****************************************************************************/
void LCD_WR_REG(uint8_t Index)
{
//   LCD_CS_CLR;
   LCD_RS_CLR;	
//	 SW_delay_us(1);
#if USE_HARDWARE_SPI   
   SPI_WriteByte(SPI1,Index);
#else
   SPIv_WriteData(Index);
#endif 
//   LCD_CS_SET;
}

/****************************************************************************
* 名    称：Lcd_WriteData(uint8_t Data)
* 功    能：向液晶屏写一个8位数据
* 入口参数：dat     寄存器数据
* 出口参数：无
* 说    明：向控制器指定地址写入数据，内部函数
****************************************************************************/
void LCD_WR_DATA(uint8_t Data)
{
//   LCD_CS_CLR;
   LCD_RS_SET;
//	SW_delay_us(1);
#if USE_HARDWARE_SPI   
   SPI_WriteByte(SPI1,Data);
#else
   SPIv_WriteData(Data);
#endif 
//   LCD_CS_SET;
}

/****************************************************************************
* 名    称：void LCD_WriteReg(uint8_t Index,uint16_t Data)
* 功    能：写寄存器数据
* 入口参数：Index,Data
* 出口参数：无
* 说    明：本函数为组合函数，向Index地址的寄存器写入Data值
****************************************************************************/
void LCD_WriteReg(uint8_t Index,uint16_t Data)
{
	LCD_WR_REG(Index);
  Lcd_WriteData_16Bit(Data);
}

/****************************************************************************
* 名    称：void Lcd_WriteData_16Bit(uint16_t Data)
* 功    能：向液晶屏写一个16位数据
* 入口参数：Data
* 出口参数：无
* 说    明：向控制器指定地址写入一个16位数据
****************************************************************************/
void Lcd_WriteData_16Bit(uint16_t Data)
{	
	LCD_WR_DATA(Data>>8);
	LCD_WR_DATA(Data&0xff);	
}

/****************************************************************************
* 名    称：void Lcd_Reset(void)
* 功    能：液晶硬复位函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化前需执行一次复位操作
****************************************************************************/
void Lcd_Reset(void)
{
//	 LCD_RST_SET;
//   HAL_Delay(20);
//	 LCD_RST_CLR;
//   HAL_Delay(100);
//   LCD_RST_SET;
//   HAL_Delay(50);
	
	LCD_RST_SET;	
	HAL_Delay(10);
	LCD_RST_CLR;
	HAL_Delay(100);
	LCD_RST_SET;	
	HAL_Delay(100);	
}
/****************************************************************************
* 名    称：void Lcd_Init(void)
* 功    能：液晶初始化函数
* 入口参数：无
* 出口参数：无
* 说    明：液晶初始化_ILI9225_176X220
****************************************************************************/
void Lcd_Init(void)
{	
#if USE_HARDWARE_SPI //使用硬件SPI
	SPI1_Init();
#else	
	LCD_GPIO_Init();//使用模拟SPI
	//LCD_LED_SET;
#endif
	  Lcd_Reset(); //Reset before LCD Init.
		LCD_WR_REG(0xCF);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0xc3); 
		LCD_WR_DATA(0X30); 

		LCD_WR_REG(0xED);  
		LCD_WR_DATA(0x64); 
		LCD_WR_DATA(0x03); 
		LCD_WR_DATA(0X12); 
		LCD_WR_DATA(0X81); 

		LCD_WR_REG(0xE8);  
		LCD_WR_DATA(0x85); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x78); //7a

		LCD_WR_REG(0xCB);  
		LCD_WR_DATA(0x39); 
		LCD_WR_DATA(0x2C); 
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x34); 
		LCD_WR_DATA(0x02); 

		LCD_WR_REG(0xF7);  
		LCD_WR_DATA(0x20); 

		LCD_WR_REG(0xEA);  
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x00); 

		LCD_WR_REG(0xC0);    //Power control 
		LCD_WR_DATA(0x1b);   //VRH[5:0]  21

		LCD_WR_REG(0xC1);    //Power control 
		LCD_WR_DATA(0x12);   //SAP[2:0];BT[3:0] 

		LCD_WR_REG(0xC5);   
		LCD_WR_DATA(0x32);   //32
		LCD_WR_DATA(0x3C); 

		LCD_WR_REG(0xC7);    //VCM control2   
		LCD_WR_DATA(0xAF);  //9e

		LCD_WR_REG(0x36);    // Memory Access Control 
	#if USE_HORIZONTAL
		 LCD_WR_DATA(0x68);//横屏参数0x68
	#else
		 LCD_WR_DATA(0x08);//竖屏参数 
	#endif   // FPC D:0X08   68
		//();     
		
		LCD_WR_REG(0x21);

		LCD_WR_REG(0x3A);   
		LCD_WR_DATA(0x55); 

		LCD_WR_REG(0xB1);   
		LCD_WR_DATA(0x00);   
		LCD_WR_DATA(0x1b); 
		LCD_WR_REG(0xB4);   
		LCD_WR_DATA(0x04);

		LCD_WR_REG(0xB6);    // Display Function Control 
		LCD_WR_DATA(0x0A); 
		LCD_WR_DATA(0xA2); 

		LCD_WR_REG(0xF6);    
		LCD_WR_DATA(0x01); 
		LCD_WR_DATA(0x30); 

		LCD_WR_REG(0xF2);    // 3Gamma Function Disable 
		LCD_WR_DATA(0x00); 

		LCD_WR_REG(0x26);    //Gamma curve selected 
		LCD_WR_DATA(0x01); 

		LCD_WR_REG(0xE0);    //Set Gamma 
		LCD_WR_DATA(0x0F); 
		LCD_WR_DATA(0x1d); 
		LCD_WR_DATA(0x1a); 
		LCD_WR_DATA(0x0a); 
		LCD_WR_DATA(0x0d); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x49); 
		LCD_WR_DATA(0X66); 
		LCD_WR_DATA(0x3b); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x11); 
		LCD_WR_DATA(0x01); 
		LCD_WR_DATA(0x09); 
		LCD_WR_DATA(0x05); 
		LCD_WR_DATA(0x04); //04

		LCD_WR_REG(0XE1);    //Set Gamma '
		LCD_WR_DATA(0x00); 
		LCD_WR_DATA(0x18); 
		LCD_WR_DATA(0x1d); 
		LCD_WR_DATA(0x02); 
		LCD_WR_DATA(0x0f); 
		LCD_WR_DATA(0x04); 
		LCD_WR_DATA(0x36); 
		LCD_WR_DATA(0x13); 
		LCD_WR_DATA(0x4c); 
		LCD_WR_DATA(0x07); 
		LCD_WR_DATA(0x13); 
		LCD_WR_DATA(0x0f); 
		LCD_WR_DATA(0x2E); 
		LCD_WR_DATA(0x2f); 
		LCD_WR_DATA(0x05);//0F

		LCD_WR_REG(0x11);    //Exit Sleep 
		HAL_Delay(120);
		LCD_WR_REG(0x29); //display on
}



/*************************************************
函数名：LCD_Set_XY
功能：设置lcd显示起始点
入口参数：xy坐标
返回值：无
*************************************************/
void Lcd_SetXY(uint16_t Xpos, uint16_t Ypos)
{	
	LCD_WR_REG(0x2A);
	Lcd_WriteData_16Bit(Xpos);
	LCD_WR_REG(0x2B);
	Lcd_WriteData_16Bit(Ypos);
	LCD_WR_REG(0x2c);	
} 
/*************************************************
函数名：LCD_Set_Region
功能：设置lcd显示区域，在此区域写点数据自动换行
入口参数：xy起点和终点
返回值：无
*************************************************/
//设置显示窗口
void Lcd_SetRegion(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd)
{
	LCD_WR_REG(0x2A);
	Lcd_WriteData_16Bit(xStar);
	Lcd_WriteData_16Bit(xEnd);
	LCD_WR_REG(0x2B);
	Lcd_WriteData_16Bit(yStar);
	Lcd_WriteData_16Bit(yEnd);
	LCD_WR_REG(0x2c);
}

	
/*************************************************
函数名：LCD_DrawPoint
功能：画一个点
入口参数：xy坐标和颜色数据
返回值：无
*************************************************/
void Gui_DrawPoint(uint16_t x,uint16_t y,uint16_t Data)
{
	Lcd_SetXY(x,y);
	Lcd_WriteData_16Bit(Data);
}    

/*************************************************
函数名：Lcd_Clear
功能：全屏清屏函数
入口参数：填充颜色COLOR
返回值：无
*************************************************/
void Lcd_Clear(uint16_t Color)               
{	
   unsigned int i;
   Lcd_SetRegion(0,0,X_MAX_PIXEL-1,Y_MAX_PIXEL-1);
//	 LCD_CS_CLR;
   LCD_RS_SET;	
   for(i=0;i<X_MAX_PIXEL*Y_MAX_PIXEL;i++)
   {	
	  	//Lcd_WriteData_16Bit(Color);
#if USE_HARDWARE_SPI   
		SPI_WriteByte(SPI1,Color>>8);
		SPI_WriteByte(SPI1,Color);
#else
		SPIv_WriteData(Color>>8);
		SPIv_WriteData(Color);
#endif 
   }   
//		LCD_CS_SET;
}

