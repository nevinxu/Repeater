#ifndef __INCLUDES_H__
#define __INCLUDES_H__

#include <stdio.h> 
#include <string.h> 
#include <ctype.h> 
#include <stdlib.h> 
#include <stdarg.h> 

#include "ucos_ii.h" 
#include "os_cpu.h" 
#include "os_cfg.h" 

#include "app_cfg.h" 
#include "bsp.h" 
#include "stm32f10x.h"
#include "stm32f10x_tim.h"
#include "stm32f10x_spi.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "wifi_application.h"
#include "nvmem.h"
#include "Dataprocess.h"
#include "Lcddisplay.h"

#include "appcc3000.h"
#include "appuart.h"
#include "appcc1101.h"


#include  "cc1101.h"
#include  "driver/w25x40/spi_flash.h"
#include 	"driver/w25x40/spi_flash_ctrl.h"
#include 	"driver/LCD/UC1701X.h"

   
#define CONVERT_TO_INT(lsb,msb)      ((lsb) + 0x0100*(msb))
  
#define CONVERT_TO_LONG(b1,b2,b3,b4) ((b1) + 0x0100*(b2) + 0x010000*(b3) + 0x01000000*(b4))
  
#define CONVERT_TO_LONG_LH(long)     (((long&0xff000000)>>24) + ((long&0x00ff0000)>>8) + ((long&0x0000ff00)<<8) + ((long&0x000000ff)<<24))

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)
  
/*
********************************************************************************************************
Date types(Compiler specific) 
********************************************************************************************************
*/

  typedef unsigned char   uint8;                                          /* Unsigned  8 bit quantity     */
  typedef signed   char   int8;                                           /* Signed    8 bit quantity     */
  typedef unsigned short  uint16;                                         /* Unsigned  16 bit quantity    */
  typedef signed   short  int16;                                          /* Signed    16 bit quantity    */
  typedef unsigned int    uint32;                                         /* Unsigned  32 bit quantity    */
  typedef signed   int    int32;                                          /* Signed    32 bit quantity    */
  typedef float           fp32;                                           /* Single    precision          */
/* floating  point              */
  typedef double          fp64;                                           /* Double    precision          */
/* floating  point              */


#define bool _Bool
#define FALSE 0
#define TRUE !FALSE

#define GPIO_LOW(a,b) 		GPIO_ResetBits(a,b)
#define GPIO_HIGH(a,b)		GPIO_SetBits(a,b)

typedef enum
    {
        hw_false=0x00,
        hw_true=0x01,
        hw_null=0xFF,
    }hw_bool;
		
			
#define  EVENT_TIMEOUT_S            1000             
#define  EVENT_TIMEOUT_M            60000    

#define  EVENT_MAX_COUNT              8
  
#define  RECV_EVENT_HANDLER           0X01 
#define  SEND_EVENT_HANDLER           0X02
#define  LED_EVENT_HANDLER            0X04 
#define  LOCAL_CONTROL_EVENT_HANDLER  0X08 

#define  NON_EVENT_HANDLER            0X00   
#define  ALL_EVENT_HANDLER            0Xff 
  
#define  RECV_DATA_TIMEOUT            50
		
		
#define 	NOCONNECT 				0x0		
#define 	SCANNING 					0x05	
#define 	SCANOVER 					0x06	
#define  	CONNECTING       	0x01		
#define		CONNECTED					0x02
#define		SENDDATA					0x03
#define   WAITDATARECE			0x04
#define		RECONNECTED					0x07

#define 	TCPSETTING 				0x10		
#define 	TCPOVER 						0x20	
//#define 	SCANOVER 					0x06	
//#define  	CONNECTING       	0x01		
//#define		CONNECTED					0x02
//#define		SENDDATA					0x03
//#define   WAITDATARECE			0x04


  
extern volatile unsigned long EventTimeOut[EVENT_MAX_COUNT];
extern volatile unsigned char SysEvent;  
extern void SetEventTimeOut(unsigned char event,unsigned long  TimeOut); 
extern void SetEvent(unsigned char event);
extern void ClearEvent(unsigned char event);
extern void GetEvent(void);
		
		
		
#endif
