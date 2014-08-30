#ifndef __SPI_FLASH_H
#define __SPI_FLASH_H
#define SPI_FLASH_ANALOG  /*SPI_FLASH_HAL*/
//#define _BOARD  //DEMO Board   测试
#include "stm32f10x.h"

#ifdef SPI_FLASH_HAL  /* 硬件SPI配置 */
#define SPI_FLASH_SPI                           SPI2
#define SPI_FLASH_SPI_CLK                       RCC_APB2Periph_SPI2
#define SPI_FLASH_SPI_SCK_PIN                   GPIO_Pin_14                  /* PD.14 */
#define SPI_FLASH_SPI_SCK_GPIO_PORT             GPIOD                       /* GPIOD */
#define SPI_FLASH_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOD
#define SPI_FLASH_SPI_MISO_PIN                  GPIO_Pin_7                  /* PC.07 */
#define SPI_FLASH_SPI_MISO_GPIO_PORT            GPIOC                       /* GPIOC */
#define SPI_FLASH_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOC
#define SPI_FLASH_SPI_MOSI_PIN                  GPIO_Pin_13                  /* PD.13 */
#define SPI_FLASH_SPI_MOSI_GPIO_PORT            GPIOD                       /* GPIOD */
#define SPI_FLASH_SPI_MOSI_GPIO_CLK             RCC_APB2Periph_GPIOD
#define SPI_FLASH_CS_PIN                        GPIO_Pin_8                  /* PC.08 */
#define SPI_FLASH_CS_GPIO_PORT                  GPIOC                       /* GPIOC */
#define SPI_FLASH_CS_GPIO_CLK                   RCC_APB2Periph_GPIOC

#else   /* 模拟SPI配置 */
// SPI_SCK
#define SPI_FLASH_SCK_CLOCK      		RCC_APB2Periph_GPIOB
#define SPI_FLASH_SCK_BASE  				GPIOB
#define SPI_FLASH_SCK_PIN    				GPIO_Pin_3

// SPI_MISO
#define SPI_FLASH_MISO_CLOCK       	RCC_APB2Periph_GPIOA
#define SPI_FLASH_MISO_BASE  				GPIOA
#define SPI_FLASH_MISO_PIN   				GPIO_Pin_12

// SPI_MOSI
#define SPI_FLASH_MOSI_CLOCK       	RCC_APB2Periph_GPIOB
#define SPI_FLASH_MOSI_BASE  				GPIOB
#define SPI_FLASH_MOSI_PIN   				GPIO_Pin_4

// NSS
#define SPI_FLASH_NSS_CLOCK      		RCC_APB2Periph_GPIOA
#define SPI_FLASH_NSS_BASE       		GPIOA
#define SPI_FLASH_NSS_PIN        		GPIO_Pin_11

// io ctrl
#define SPI_FLASH_SCK_S_1      GPIO_SetBits(GPIOB, GPIO_Pin_3)
#define SPI_FLASH_SCK_S_0      GPIO_ResetBits(GPIOB, GPIO_Pin_3)

#define SPI_FLASH_MISO_S_G     (GPIOA->IDR & GPIO_Pin_12)

#define SPI_FLASH_MOSI_S_1     GPIO_SetBits(GPIOB, GPIO_Pin_4)
#define SPI_FLASH_MOSI_S_0     GPIO_ResetBits(GPIOB, GPIO_Pin_4)

#define SPI_FLASH_NSS_S_1     GPIO_SetBits(GPIOA, GPIO_Pin_11)
#define SPI_FLASH_NSS_S_0     GPIO_ResetBits(GPIOA, GPIO_Pin_11)

#endif   /*SPI配置 */

// NSS
#define SPI_FLASH_CS_LOW()       GPIO_ResetBits(GPIOA, GPIO_Pin_11)
#define SPI_FLASH_CS_HIGH()      GPIO_SetBits(GPIOA, GPIO_Pin_11)

//Write_Protect
#define SPI_FLASH_Write_Protect_Assert()      GPIO_ResetBits(GPIOA, GPIO_Pin_15)
#define SPI_FLASH_Write_Protect_Deassert()      GPIO_SetBits(GPIOA, GPIO_Pin_15)

////Hold
//#define SPI_FLASH_Hold_Assert()      GPIO_ResetBits(GPIOD, GPIO_Pin_15)
//#define SPI_FLASH_Hold_Deassert()      GPIO_SetBits(GPIOD, GPIO_Pin_15)

void SPI_FLASH_Write_Protect_ENABLE(void);//flash 关闭写保护
void SPI_FLASH_Write_Protect_DISABLE(void);//flash 打开写保护
//void SPI_FLASH_Hold_ENABLE(void);//flash HOLD ENABLE
//void SPI_FLASH_Hold_DISABLE(void);//flash HOLD DISABLE

void SPI_FLASH_Init(void);
void SPI_FLASH_SectorErase(u32 SectorAddr);
void SPI_FLASH_BulkErase(void);
void SPI_FLASH_PageWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferWrite(u8* pBuffer, u32 WriteAddr, u16 NumByteToWrite);
void SPI_FLASH_BufferRead(u8* pBuffer, u32 ReadAddr, u16 NumByteToRead);
u32 SPI_FLASH_ReadID(void);
u32 SPI_FLASH_ReadDeviceID(void);
void SPI_FLASH_StartReadSequence(u32 ReadAddr);
void SPI_Flash_PowerDown(void);
void SPI_Flash_WAKEUP(void);

#ifdef SPI_FLASH_ANALOG
void SPI_FLASH_SendByte(u8 byte);
#else
u8 SPI_FLASH_SendByte(u8 byte);
#endif

u8 SPI_FLASH_ReadByte(void);
u16 SPI_FLASH_SendHalfWord(u16 HalfWord);
void SPI_FLASH_WriteEnable(void);
void SPI_FLASH_WaitForWriteEnd(void);

#endif /* __SPI_FLASH_H */

