/*****************************************************************************
 *
 *  spi.h  - CC3000 Host Driver Implementation.
 *  Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/

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

	
#ifdef TEST_DEMO	
//IRQ Pin: PB1
#define IRQ_CLOCK              RCC_APB2Periph_GPIOB
#define IRQ_BASE               GPIOB
#define IRQ_PIN                GPIO_Pin_1
#define IRQ_INT_PORT           GPIO_PortSourceGPIOB
#define IRQ_INT_PIN            GPIO_PinSource1
#define IRQ_INT_LINE           EXTI_Line1
#define IRQ_INT_CHANNEL        EXTI1_IRQn
#else
//IRQ Pin: PA1
#define IRQ_CLOCK              RCC_APB2Periph_GPIOA
#define IRQ_BASE               GPIOA
#define IRQ_PIN                GPIO_Pin_1
#define IRQ_INT_PORT           GPIO_PortSourceGPIOA
#define IRQ_INT_PIN            GPIO_PinSource1
#define IRQ_INT_LINE           EXTI_Line1
#define IRQ_INT_CHANNEL        EXTI1_IRQn	
#endif

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

