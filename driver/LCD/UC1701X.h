#ifndef _UC1701x_CONFIG_H_
#define _UC1701x_CONFIG_H_

#include"stm32f10x.h"


#define LCD_CLK_CLOCK            RCC_APB2Periph_GPIOA
#define LCD_CLK_BASE             GPIOA
#define LCD_CLK_PIN              GPIO_Pin_9

#define LCD_SDA_CLOCK            RCC_APB2Periph_GPIOA
#define LCD_SDA_BASE             GPIOA
#define LCD_SDA_PIN              GPIO_Pin_3

#define LCD_RS_CLOCK            RCC_APB2Periph_GPIOA
#define LCD_RS_BASE             GPIOA
#define LCD_RS_PIN              GPIO_Pin_2

#define LCD_RST_CLOCK            RCC_APB2Periph_GPIOB
#define LCD_RST_BASE             GPIOB
//#define LCD_RST_PIN              GPIO_Pin_6   //¹Ü½ÅËð»µ
#define LCD_RST_PIN              GPIO_Pin_9

#define LCD_CS_CLOCK            RCC_APB2Periph_GPIOB
#define LCD_CS_BASE             GPIOB
#define LCD_CS_PIN              GPIO_Pin_10

#define LCD_BLACKLED_CLOCK            RCC_APB2Periph_GPIOA
#define LCD_BLACKLED_BASE             GPIOA
#define LCD_BLACKLED_PIN              GPIO_Pin_10
   
#define LCD_CLK_HI  GPIO_SetBits(LCD_CLK_BASE, LCD_CLK_PIN);	
#define LCD_CLK_LO  GPIO_ResetBits(LCD_CLK_BASE, LCD_CLK_PIN);	

#define LCD_SDA_HI  GPIO_SetBits(LCD_SDA_BASE, LCD_SDA_PIN);	
#define LCD_SDA_LO  GPIO_ResetBits(LCD_SDA_BASE, LCD_SDA_PIN);	

#define LCD_RS_HI   GPIO_SetBits(LCD_RS_BASE, LCD_RS_PIN);	
#define LCD_RS_LO   GPIO_ResetBits(LCD_RS_BASE, LCD_RS_PIN);	

#define LCD_RST_HI  GPIO_SetBits(LCD_RST_BASE, LCD_RST_PIN);	
#define LCD_RST_LO  GPIO_ResetBits(LCD_RST_BASE, LCD_RST_PIN);	

#define LCD_CS_HI   GPIO_SetBits(LCD_CS_BASE, LCD_CS_PIN);	
#define LCD_CS_LO   GPIO_ResetBits(LCD_CS_BASE, LCD_CS_PIN);	

#define LCD_BLACKLED_ON   GPIO_SetBits(LCD_BLACKLED_BASE, LCD_BLACKLED_PIN);	
#define LCD_BLACKLED_OFF  GPIO_ResetBits(LCD_BLACKLED_BASE, LCD_BLACKLED_PIN);	


#define BLACK 0xff
#define WHILE 0x00


extern void Initial_Lcd(void);
extern void clear_screen(unsigned char x_start,unsigned char y_start,unsigned char x_size,unsigned char y_size,unsigned char Color);
extern void clear_All(void);
extern void DisplayGraphic1(void);
extern void DisplayCode(unsigned char x_start, unsigned char y_start,unsigned char x_size, unsigned char y_size, const unsigned char *Data);
extern void DisplayNum(unsigned char x_start, unsigned char y_start,unsigned char num);
extern void displayChar(unsigned char x_start, unsigned char y_start,unsigned char iChar);
extern void displayString(unsigned char x_start, unsigned char y_start,unsigned char *iString);
extern void DisplayRate(u8 x_start, u8 y_start, u16 rate);

extern void Display_WiFi_Status_Code();
extern void Display_WiFi_Status_NoConnect();
extern void Display_WiFi_Status_Connecting();
extern void Display_WiFi_Status_Connected();
extern void Display_WiFi_Status_ReConnecting();
extern void Display_WiFi_Router_Name();
extern void Display_Connect_Type();
extern void Display_WiFi_Status_Scanning();
extern void Display_WiFi_Status_Scanfinish();
extern void Display_Line_Clear(unsigned char Line);
extern void Display_WiFi_TCP_Status();

#endif
