#ifndef __CC3000SPI_H__
#define __CC3000SPI_H__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stm32f10x.h"

//*****************************************************************************
//
// If building with a C++ compiler, make all of the definitions in this header
// have a C binding.
//
//*****************************************************************************
#ifdef  __cplusplus
extern "C"
{
#endif

typedef void (*gcSpiHandleRx)(void *p);
typedef void (*gcSpiHandleTx)(void);

//IRQ Pin: PA1
#define IRQ_CLOCK              RCC_APB2Periph_GPIOA
#define IRQ_BASE               GPIOA
#define IRQ_PIN                GPIO_Pin_1
#define IRQ_INT_PORT           GPIO_PortSourceGPIOA
#define IRQ_INT_PIN            GPIO_PinSource1
#define IRQ_INT_LINE           EXTI_Line1
#define IRQ_INT_CHANNEL        EXTI1_IRQn	

//CS Pin: PA4
#define SPI_CS_CLOCK           RCC_APB2Periph_GPIOA
#define SPI_CS_BASE            GPIOA
#define SPI_CS_PIN             GPIO_Pin_4

//SPI
#define SPI_CLOCK              RCC_APB2Periph_SPI1
#define SPI_BASE               SPI1
#define SPI_PIN_BASE           GPIOA
#define SPI_SCK_PIN            GPIO_Pin_5
#define SPI_MISO_PIN           GPIO_Pin_6
#define SPI_MOSI_PIN           GPIO_Pin_7

#define SPI_DMA_RX_CHANNEL              DMA1_Channel2 
#define SPI_DMA_TX_CHANNEL              DMA1_Channel3


//EN Pin: PB1
#define CC3000_EN_CLOCK        RCC_APB2Periph_GPIOB
#define CC3000_EN_BASE         GPIOA   //新板子
#define CC3000_EN_PIN          GPIO_Pin_0
//#define CC3000_EN_BASE         GPIOB    //旧板子
//#define CC3000_EN_PIN          GPIO_Pin_1

//WL_EN Pin: PA0
#define WL_EN_CLOCK            RCC_APB2Periph_GPIOA
#define WL_EN_BASE             GPIOB		//新版子
#define WL_EN_PIN              GPIO_Pin_6
//#define WL_EN_BASE             GPIOA   //旧板子
//#define WL_EN_PIN              GPIO_Pin_0


//PWM_32K PIN PB0
#define PWM_32K_CLOCK                  RCC_APB2Periph_GPIOB
#define PWM_32K_BASE                   GPIOB
#define PWM_32K_PIN                    GPIO_Pin_0


typedef enum
{
  SPI_DMA_TX = 0,
  SPI_DMA_RX = 1
}SPI_DMADirection_TypeDef;


extern unsigned char wlan_tx_buffer[];

//*****************************************************************************
//
// Prototypes for the APIs.
//
//*****************************************************************************
extern void SpiCleanGPIOISR(void);
extern void SpiClose(void);
extern void SpiOpen(gcSpiHandleRx pfRxHandler);

extern int init_spi(void);
extern long ReadWlanInterruptPin(void);
extern void WlanInterruptEnable(void);
extern void WlanInterruptDisable(void);
extern void WriteWlanPin(unsigned char val);
extern long SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength);
extern long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength);
extern void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
extern void SpiReadDataSynchronous(unsigned char *data, unsigned short size);
extern void SpiReadHeader(void);
extern long SpiReadDataCont(void);
extern void SpiPauseSpi(void);
extern void SpiResumeSpi(void);
extern void SpiTriggerRxProcessing(void);
extern void SSIContReadOperation(void);
extern long TXBufferIsEmpty(void);
extern long RXBufferIsNotEmpty(void);

//*****************************************************************************
//
// Mark the end of the C bindings section for C++ compilers.
//
//*****************************************************************************
#ifdef  __cplusplus
}
#endif // __cplusplus
#endif

