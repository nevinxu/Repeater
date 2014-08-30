
//#include "stm32f10x.h"
#include "spi_flash.h"
#include "debug.h"
#include "spi_flash_ctrl.h"
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
/* 发送缓冲区初始化 */
u8 Tx_Buffer[] = "OK!";
u8 Rx_Buffer[BufferSize];

u8 DeviceID = 0;
volatile u32 FlashID = 0;
volatile TestStatus TransferStatus1 = FAILED;

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

/*
 * 函数名：Buffercmp
 * 描述  ：比较两个缓冲区中的数据是否相等
 * 输入  ：-pBuffer1     src缓冲区指针
 *         -pBuffer2     dst缓冲区指针
 *         -BufferLength 缓冲区长度
 * 输出  ：无
 * 返回  ：-PASSED pBuffer1 等于   pBuffer2
 *         -FAILED pBuffer1 不同于 pBuffer2
 */
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }

    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

void Delay(__IO uint32_t nCount)
{
  for(; nCount != 0; nCount--);
}


void FLSASH_Test_Ctrl(void)
{

  /* Get SPI Flash Device ID */
  DeviceID = SPI_FLASH_ReadDeviceID();
  
	Delay( 200 );

  /* Get SPI Flash ID */
  FlashID = SPI_FLASH_ReadID();

  printf("\r\n FlashID is 0x%X,  Manufacturer Device ID is 0x%X\r\n", FlashID, DeviceID);

  /* Check the SPI Flash ID */
  if (FlashID == sFLASH_ID)  /* #define  sFLASH_ID  0xEF3015 */
  {
  
    printf("\r\n 检测到串行flash W25X40 !\r\n");
    
    /* Erase SPI FLASH Sector to write on */
    SPI_FLASH_SectorErase(FLASH_SectorToErase);	 	 
   
    /* 将发送缓冲区的数据写到flash中 */
    SPI_FLASH_BufferWrite(Tx_Buffer, FLASH_WriteAddress, BufferSize);
		printf("\r\n 写入的数据为：%s \r\t", Tx_Buffer);

    /* 将刚刚写入的数据读出来放到接收缓冲区中 */
    SPI_FLASH_BufferRead(Rx_Buffer, FLASH_ReadAddress, BufferSize);
		printf("\r\n 读出的数据为：%s \r\n", Tx_Buffer);

    /* 检查写入的数据与读出的数据是否相等 */
    TransferStatus1 = Buffercmp(Tx_Buffer, Rx_Buffer, BufferSize);

    if( PASSED == TransferStatus1 )
    {    
        printf("\r\n 4M串行flash(W25X40)测试成功!\n\r");
    }
    else
    {        
        printf("\r\n 4M串行flash(W25X40)测试失败!\n\r");
    }
  }
  else
  {    
    printf("\r\n 获取不到 W25X40 ID!\n\r");
  }

  SPI_Flash_PowerDown();  
}


