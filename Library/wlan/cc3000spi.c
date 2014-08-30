

//*****************************************************************************
//
//! \addtogroup link_buff_api
//! @{
//
//*****************************************************************************
#include "hci.h"
#include "cc3000spi.h"
#include "evnt_handler.h"


#define READ                    3
#define WRITE                   1

#define HI(value)               (((value) & 0xFF00) >> 8)
#define LO(value)               ((value) & 0x00FF)

#define ASSERT_CS()          (GPIO_ResetBits(SPI_CS_BASE, SPI_CS_PIN))
#define DEASSERT_CS()        (GPIO_SetBits(SPI_CS_BASE, SPI_CS_PIN))

#define HEADERS_SIZE_EVNT       (SPI_HEADER_SIZE + 5)

#define SPI_HEADER_SIZE			                         (5)

#define 	eSPI_STATE_POWERUP 				 (0)
#define 	eSPI_STATE_INITIALIZED  		         (1)
#define 	eSPI_STATE_IDLE					 (2)
#define 	eSPI_STATE_WRITE_IRQ	   		         (3)
#define 	eSPI_STATE_WRITE_FIRST_PORTION                   (4)
#define 	eSPI_STATE_WRITE_EOT			         (5)
#define 	eSPI_STATE_READ_IRQ				 (6)
#define 	eSPI_STATE_READ_FIRST_PORTION	                 (7)
#define 	eSPI_STATE_READ_EOT				 (8)

typedef struct
{
    gcSpiHandleRx SPIRxHandler;
    unsigned short usTxPacketLength;
    unsigned short usRxPacketLength;
    unsigned long ulSpiState;
    unsigned char *pTxPacket;
    unsigned char *pRxPacket;

} tSpiInformation;

tSpiInformation sSpiInformation;   //spi 的结构体

// buffer for 5 bytes of SPI HEADER
unsigned char tSpiReadHeader[] = { READ, 0, 0, 0, 0 };

void SpiWriteDataSynchronous(unsigned char *data, unsigned short size);
void SpiPauseSpi(void);
void SpiResumeSpi(void);
void SSIContReadOperation(void);

// The magic number that resides at the end of the TX/RX buffer (1 byte after
// the allocated size) for the purpose of detection of the overrun. The location
// of the memory where the magic number resides shall never be written. In case
// it is written - the overrun occurred and either receive function or send
// function will stuck forever.
#define CC3000_BUFFER_MAGIC_NUMBER (0xDE)

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//__no_init is used to prevent the buffer initialization in order to prevent hardware WDT expiration    ///
// before entering to 'main()'.                                                                         ///
//for every IDE, different syntax exists :          1.   __CCS__ for CCS v5                             ///
//                                                  2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench  ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __IAR_SYSTEMS_ICC__
__no_init char wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];
#else
unsigned char wlan_rx_buffer[CC3000_RX_BUFFER_SIZE];    //最大的缓存
#endif

#ifdef __IAR_SYSTEMS_ICC__
__no_init unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
#else
unsigned char wlan_tx_buffer[CC3000_TX_BUFFER_SIZE];
#endif


static  void delay(unsigned int n)
{
unsigned int i,j;
	for(i=0;i<n;i++)
	for(j=0;j<1000;j++);
	
}

//*****************************************************************************
//
//!  SpiCleanGPIOISR
//!
//!  \param  none
//!
//!  \return none
//!
//!  \brief  This function get the reason for the GPIO interrupt and clear
//!          corresponding interrupt flag
//
//*****************************************************************************
void SpiCleanGPIOISR(void)
{
    EXTI_ClearITPendingBit(IRQ_INT_LINE);
}

//*****************************************************************************
//
//!  SpiClose
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Close Spi interface
//
//*****************************************************************************
void SpiClose(void)
{
    if (sSpiInformation.pRxPacket)
    {
        sSpiInformation.pRxPacket = 0;
    }

    //	Disable Interrupt in GPIOA module...
    tSLInformation.WlanInterruptDisable();
}

//*****************************************************************************
//
//!  SpiOpen
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Open Spi interface
//
//*****************************************************************************
void SpiOpen(gcSpiHandleRx pfRxHandler)
{
    sSpiInformation.ulSpiState = eSPI_STATE_POWERUP;
    sSpiInformation.SPIRxHandler = pfRxHandler;
    sSpiInformation.usTxPacketLength = 0;
    sSpiInformation.pTxPacket = NULL;
    sSpiInformation.pRxPacket = (unsigned char *) wlan_rx_buffer;
    sSpiInformation.usRxPacketLength = 0;
    wlan_rx_buffer[CC3000_RX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;
    wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] = CC3000_BUFFER_MAGIC_NUMBER;

    // Enable interrupt on the GPIOA pin of WLAN IRQ
    tSLInformation.WlanInterruptEnable();
}


static void CC3000_DMA_Config(SPI_DMADirection_TypeDef Direction, uint8_t* buffer, uint16_t NumData)
{
	
  DMA_InitTypeDef DMA_InitStructure;
  /* Initialize the DMA_PeripheralBaseAddr member */
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t )&SPI1->DR; //SPI_DR_BASE; //
  /* Initialize the DMA_MemoryBaseAddr member */
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)buffer;
   /* Initialize the DMA_PeripheralInc member */
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //寄存器地址不加一
  /* Initialize the DMA_MemoryInc member */
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;   //存储器地址自动加一
  /* Initialize the DMA_PeripheralDataSize member */
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
  /* Initialize the DMA_MemoryDataSize member */
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
  /* Initialize the DMA_Mode member */
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  /* Initialize the DMA_Priority member */
  DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
  /* Initialize the DMA_M2M member */
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
  
  /* If using DMA for Reception */
  if (Direction == SPI_DMA_RX)
  {
    /* Initialize the DMA_DIR member */   //dma目标地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
//    DMA_DeInit(SPI_DMA_RX_CHANNEL);
    
    DMA_Init(SPI_DMA_RX_CHANNEL, &DMA_InitStructure);
  }
   /* If using DMA for Transmission */
  else if (Direction == SPI_DMA_TX)
  { 
    /* Initialize the DMA_DIR member */
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    
    /* Initialize the DMA_BufferSize member */
    DMA_InitStructure.DMA_BufferSize = NumData;
    
//    DMA_DeInit(SPI_DMA_TX_CHANNEL);
 
    DMA_Init(SPI_DMA_TX_CHANNEL, &DMA_InitStructure);
  }
}

void SpiCC3000DMAInit(void)
{
  NVIC_InitTypeDef NVIC_RxInt_InitStructure; 

  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  DMA_DeInit(SPI_DMA_RX_CHANNEL);
  DMA_DeInit(SPI_DMA_TX_CHANNEL);

  /* Configure and enable SPI DMA TX Channel interrupt */
  NVIC_RxInt_InitStructure.NVIC_IRQChannel = DMA1_Channel3_IRQn;
  NVIC_RxInt_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_RxInt_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_RxInt_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_RxInt_InitStructure);

  /* Enable the DMA Channels Interrupts */
  DMA_ITConfig(SPI_DMA_TX_CHANNEL, DMA_IT_TC, ENABLE);    //传输完成中断打开
  DMA_ITConfig(SPI_DMA_RX_CHANNEL, DMA_IT_TC, ENABLE);   
  /* Configure DMA Peripheral but don't send data*/
  CC3000_DMA_Config(SPI_DMA_RX, (uint8_t*)wlan_rx_buffer,0);  
  CC3000_DMA_Config(SPI_DMA_TX, (uint8_t*)wlan_tx_buffer,0);

  /* Enable SPI DMA request */
  SPI_I2S_DMACmd(SPI_BASE,SPI_I2S_DMAReq_Rx, ENABLE);
  SPI_I2S_DMACmd(SPI_BASE,SPI_I2S_DMAReq_Tx, ENABLE);
  /* Enable DMA RX Channel */
  DMA_Cmd(SPI_DMA_RX_CHANNEL, ENABLE);  
  /* Enable DMA TX Channel */  
  DMA_Cmd(SPI_DMA_TX_CHANNEL, ENABLE); 

}



void SpiDMARXReconfig(unsigned char *rx_buffer,unsigned short NumData)
{
	DMA_DeInit(SPI_DMA_RX_CHANNEL);
	DMA_ITConfig(SPI_DMA_RX_CHANNEL, DMA_IT_TC, ENABLE);    //传输完成中断打开 
	CC3000_DMA_Config(SPI_DMA_RX, (uint8_t*)rx_buffer,NumData); 
	DMA_Cmd(SPI_DMA_RX_CHANNEL, ENABLE); 
	
}

void SpiDMATXReconfig(unsigned char *tx_buffer,unsigned short NumData)
{
  DMA_DeInit(SPI_DMA_TX_CHANNEL);
  DMA_ITConfig(SPI_DMA_TX_CHANNEL, DMA_IT_TC, ENABLE);    //传输完成中断打开 
  CC3000_DMA_Config(SPI_DMA_TX, (uint8_t*)tx_buffer,NumData);
  DMA_Cmd(SPI_DMA_TX_CHANNEL, ENABLE); 
	
}


//TIM3_Mode_Config PWM_32K_output
static void TIM3_Mode_Config(void)
{
  //定时器初始化结构
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //定时器通道初始化结构
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 2196;     //F103
  //设置预设分频：
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分频系数：
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  //PWM1 Mode configuration: Channel3
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //配置为PWM模式1
  
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  //设置跳变值，当计数器计数到这个值时，电平发生跳变

  TIM_OCInitStructure.TIM_Pulse = 1098;   //F103
  //当定时器计数值小于CCR1——Val 时为高电平
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);//使能通道3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  //使能定时器3
  TIM_Cmd(TIM3, DISABLE);
}

//*****************************************************************************
//
//!  init_spi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  initializes an SPI interface
//
//*****************************************************************************
int init_spi(void)
{
    GPIO_InitTypeDef gpio;
    EXTI_InitTypeDef exti;
    NVIC_InitTypeDef nvic;
    SPI_InitTypeDef spi;
    
    RCC_APB2PeriphClockCmd(IRQ_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_CS_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(SPI_CLOCK, ENABLE);
    
     //Config GPIO - IRQ pin
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    gpio.GPIO_Pin = IRQ_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(IRQ_BASE, &gpio);
    
    //Connect EXTIx Line to GPIO pin
    GPIO_EXTILineConfig(IRQ_INT_PORT, IRQ_INT_PIN);

    //Config External Interrupt
    exti.EXTI_Line = IRQ_INT_LINE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);

    // Enable interrupt for WLAN_IRQ pin
    nvic.NVIC_IRQChannel = IRQ_INT_CHANNEL;
    nvic.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvic.NVIC_IRQChannelSubPriority = 0x0F;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_ClearPendingIRQ(IRQ_INT_CHANNEL);
    NVIC_Init(&nvic);
    
    //Config GPIO - CS pin
//    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;   //for test 硬件上有电平转换芯片
    gpio.GPIO_Pin = SPI_CS_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_CS_BASE, &gpio);
    
    // Disable WLAN CS with pull up Resistor
    GPIO_SetBits(SPI_CS_BASE, SPI_CS_PIN);


    //Init SCK pin
    //gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;    //for test  硬件上有电平转换芯片
    gpio.GPIO_Pin = SPI_SCK_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PIN_BASE, &gpio);
    
    //Init MOSI pin
  //  gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;    //for test  硬件上有电平转换芯片
    gpio.GPIO_Pin = SPI_MOSI_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PIN_BASE, &gpio);
    
    //Init MISO pin
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = SPI_MISO_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SPI_PIN_BASE, &gpio);
    //Init SPI
    spi.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    spi.SPI_Mode = SPI_Mode_Master;
    spi.SPI_DataSize = SPI_DataSize_8b;
    //SPI MODE: 1
    //Clock polarity = Negetive
    //Clock phase = 2nd edge (launch on rising edge, capture on falling edge)
    spi.SPI_CPOL = SPI_CPOL_Low;
    spi.SPI_CPHA = SPI_CPHA_2Edge;
    spi.SPI_NSS = SPI_NSS_Soft;
    //Prescaler = 8, clock freq = 36MHz / 4 = 9MHz
    spi.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    //MSB first
    spi.SPI_FirstBit = SPI_FirstBit_MSB;
    spi.SPI_CRCPolynomial = 7;
    //Apply this config
    SPI_Cmd(SPI_BASE, DISABLE);
    SPI_Init(SPI_BASE, &spi);
    SPI_Cmd(SPI_BASE, ENABLE); 


		SpiCC3000DMAInit(); 
    
    delay(20);
		RCC_APB2PeriphClockCmd(CC3000_EN_CLOCK, ENABLE);
		RCC_APB2PeriphClockCmd(WL_EN_CLOCK, ENABLE);
		//Config GPIO - EN pin
		gpio.GPIO_Mode = GPIO_Mode_Out_PP;
		gpio.GPIO_Pin = CC3000_EN_PIN;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(CC3000_EN_BASE, &gpio);
  
  //WL_EN
		gpio.GPIO_Mode = GPIO_Mode_Out_OD;
		gpio.GPIO_Pin = WL_EN_PIN;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(WL_EN_BASE, &gpio);
  
  //SET WL_EN low
		GPIO_ResetBits(WL_EN_BASE, WL_EN_PIN);
  
  // Disable WLAN chip
		GPIO_SetBits(CC3000_EN_BASE, CC3000_EN_PIN);
		
		RCC_APB2PeriphClockCmd(PWM_32K_CLOCK, ENABLE);
		  //PWM_32K_OUTPUT
		gpio.GPIO_Mode = GPIO_Mode_AF_OD;
		gpio.GPIO_Pin = PWM_32K_PIN;
		gpio.GPIO_Speed = GPIO_Speed_50MHz;
		GPIO_Init(PWM_32K_BASE, &gpio);
		
		TIM3_Mode_Config();	 
		
    return (ESUCCESS);
}

//*****************************************************************************
//
//! ReadWlanInterruptPin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  return wlan interrup pin
//
//*****************************************************************************
long ReadWlanInterruptPin(void)
{
    return GPIO_ReadInputDataBit(IRQ_BASE, IRQ_PIN);
}

//*****************************************************************************
//
//! Enable waln IRQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************
void WlanInterruptEnable()
{
    EXTI_InitTypeDef exti;
    EXTI_ClearITPendingBit(IRQ_INT_LINE);
    exti.EXTI_Line = IRQ_INT_LINE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti);
}

//*****************************************************************************
//
//! Disable waln IrQ pin
//!
//! \param  none
//!
//! \return none
//!
//! \brief  Nonr
//
//*****************************************************************************
void WlanInterruptDisable()
{
    EXTI_InitTypeDef exti;
    EXTI_ClearITPendingBit(IRQ_INT_LINE);
    exti.EXTI_Line = IRQ_INT_LINE;
    exti.EXTI_Mode = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = EXTI_Trigger_Falling;
    exti.EXTI_LineCmd = DISABLE;
    EXTI_Init(&exti);
}

//*****************************************************************************
//
//! WriteWlanPin
//!
//! \param  new val
//!
//! \return none
//!
//! \brief  This functions enables and disables the CC3000 Radio
//
//*****************************************************************************
void WriteWlanPin(unsigned char val)
{
    if (val)
    {
        GPIO_ResetBits(CC3000_EN_BASE, CC3000_EN_PIN);
        delay(100);
        GPIO_SetBits(WL_EN_BASE, WL_EN_PIN);
    }
    else
    {
        GPIO_ResetBits(WL_EN_BASE, WL_EN_PIN);
        GPIO_SetBits(CC3000_EN_BASE, CC3000_EN_PIN);
    }
}

//*****************************************************************************
//
//! SpiFirstWrite
//!
//!  @param  ucBuf     buffer to write
//!  @param  usLength  buffer's length
//!
//!  @return none
//!
//!  @brief  enter point for first write flow
//
//*****************************************************************************
long SpiFirstWrite(unsigned char *ucBuf, unsigned short usLength)
{
    volatile int i;
    // workaround for first transaction
    ASSERT_CS();

    // Assuming we are running on 24 MHz ~50 micro delay is 1200 cycles;
    for (i = 0; i < 4000; i++)
        ;

    // SPI writes first 4 bytes of data
    SpiWriteDataSynchronous(ucBuf, 4);

    for (i = 0; i < 4000; i++);

    SpiWriteDataSynchronous(ucBuf + 4, usLength - 4);

    // From this point on - operate in a regular way
    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

    DEASSERT_CS();

    return (0);
}

//*****************************************************************************
//
//!  SpiWrite
//!
//!  @param  pUserBuffer  buffer to write
//!  @param  usLength     buffer's length
//!
//!  @return none
//!
//!  @brief  Spi write operation
//
//*****************************************************************************
long SpiWrite(unsigned char *pUserBuffer, unsigned short usLength)
{
    unsigned char ucPad = 0;

    // Figure out the total length of the packet in order to figure out if there
    // is padding or not
    if (!(usLength & 0x0001))
    {
        ucPad++;
    }

    pUserBuffer[0] = WRITE;
    pUserBuffer[1] = HI(usLength + ucPad);  //数据包发送长度
    pUserBuffer[2] = LO(usLength + ucPad);
    pUserBuffer[3] = 0;
    pUserBuffer[4] = 0;

    usLength += (SPI_HEADER_SIZE + ucPad);   //总共的发送字节

    // The magic number that resides at the end of the TX/RX buffer (1 byte after
    // the allocated size) for the purpose of detection of the overrun. If the
    // magic number is overwritten - buffer overrun occurred - and we will stuck
    // here forever!
    if (wlan_tx_buffer[CC3000_TX_BUFFER_SIZE - 1] != CC3000_BUFFER_MAGIC_NUMBER)  //超过最大发送字节
    {
        while (1);
				
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
    {
        while (sSpiInformation.ulSpiState != eSPI_STATE_INITIALIZED)
            ;
    }

    if (sSpiInformation.ulSpiState == eSPI_STATE_INITIALIZED)
    {
        // This is time for first TX/RX transactions over SPI: the IRQ is down -
        // so need to send read buffer size command
        SpiFirstWrite(pUserBuffer, usLength);
    }
    else
    {
        // We need to prevent here race that can occur in case 2 back to back
        // packets are sent to the  device, so the state will move to IDLE and once
        //again to not IDLE due to IRQ
        tSLInformation.WlanInterruptDisable();

        while (sSpiInformation.ulSpiState != eSPI_STATE_IDLE)
        {
            ;
        }

        sSpiInformation.ulSpiState = eSPI_STATE_WRITE_IRQ;
        sSpiInformation.pTxPacket = pUserBuffer;
        sSpiInformation.usTxPacketLength = usLength;

        // Re-enable IRQ - if it was not disabled - this is not a problem...
        tSLInformation.WlanInterruptEnable();
        
        // Assert the CS line and wait till SSI IRQ line is active and then
        // initialize write operation
        ASSERT_CS();

        // check for a missing interrupt between the CS assertion and enabling back the interrupts
        if (tSLInformation.ReadWlanInterruptPin() == 0)
        {
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket,
                    sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            DEASSERT_CS();
        }
    }

    // Due to the fact that we are currently implementing a blocking situation
    // here we will wait till end of transaction
    while (eSPI_STATE_IDLE != sSpiInformation.ulSpiState)
        ;

    return (0);
}

//*****************************************************************************
//
//!  SpiWriteDataSynchronous
//!
//!  @param  data  buffer to write
//!  @param  size  buffer's size
//!
//!  @return none
//!
//!  @brief  Spi write operation
//
//*****************************************************************************
void SpiWriteDataSynchronous(unsigned char *data, unsigned short size)
{
#if 0
	unsigned char flag =1;
	while (size--)
	{
		SpiDMATXReconfig(data,1);
		if(flag--)
		SpiDMARXReconfig(wlan_rx_buffer,size);
        data++;
	}
#endif
#if 1
    while (size)
    {
        while (!(TXBufferIsEmpty()))
            ;
        SPI_I2S_SendData(SPI_BASE, *data);
        while (!(RXBufferIsNotEmpty()))
            ;
        SPI_I2S_ReceiveData(SPI_BASE);
        size--;
        data++;
    }
#endif	

}

//*****************************************************************************
//
//! SpiReadDataSynchronous
//!
//!  @param  data  buffer to read
//!  @param  size  buffer's size
//!
//!  @return none
//!
//!  @brief  Spi read operation
//
//*****************************************************************************
void SpiReadDataSynchronous(unsigned char *data, unsigned short size)
{
#if 0
	unsigned char flag = 1;
    unsigned char *data_to_send = tSpiReadHeader;

	while (size--)
    {
		SpiDMATXReconfig(data_to_send,1);
		if(flag--)
		SpiDMARXReconfig(data,size+1);
		data++;
    }
#endif
#if 1
    long i = 0;
    unsigned char *data_to_send = tSpiReadHeader;

    for (i = 0; i < size; i++)
    {
        while (!(TXBufferIsEmpty()))
            ;
        //Dummy write to trigger the clock
        SPI_I2S_SendData(SPI_BASE, data_to_send[0]);
        while (!(RXBufferIsNotEmpty()))
            ;
        data[i] = SPI_I2S_ReceiveData(SPI_BASE);
    }
#endif


}

//*****************************************************************************
//
//!  SpiReadHeader
//!
//!  \param  buffer
//!
//!  \return none
//!
//!  \brief   This function enter point for read flow: first we read minimal 5
//!	          SPI header bytes and 5 Event Data bytes
//
//*****************************************************************************
void SpiReadHeader(void)
{
    SpiReadDataSynchronous(sSpiInformation.pRxPacket, 10);
}

//*****************************************************************************
//
//!  SpiReadDataCont
//!
//!  @param  None
//!
//!  @return None
//!
//!  @brief  This function processes received SPI Header and in accordance with
//!	         it - continues reading the packet
//
//*****************************************************************************
long SpiReadDataCont(void)
{
    long data_to_recv;
    unsigned char *evnt_buff, type;

    //determine what type of packet we have
    evnt_buff = sSpiInformation.pRxPacket;
    data_to_recv = 0;
    STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE),
            HCI_PACKET_TYPE_OFFSET, type);

    switch (type)
    {
    case HCI_TYPE_DATA:
    {
        // We need to read the rest of data..
        STREAM_TO_UINT16((char *)(evnt_buff + SPI_HEADER_SIZE),
                HCI_DATA_LENGTH_OFFSET, data_to_recv);
        if (!((HEADERS_SIZE_EVNT + data_to_recv) & 1))
        {
            data_to_recv++;
        }

        if (data_to_recv)
        {
            SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
        }
        break;
    }
    case HCI_TYPE_EVNT:
    {
        // Calculate the rest length of the data
        STREAM_TO_UINT8((char *)(evnt_buff + SPI_HEADER_SIZE),
                HCI_EVENT_LENGTH_OFFSET, data_to_recv);
        data_to_recv -= 1;

        // Add padding byte if needed
        if ((HEADERS_SIZE_EVNT + data_to_recv) & 1)
        {

            data_to_recv++;
        }

        if (data_to_recv)
        {
            SpiReadDataSynchronous(evnt_buff + 10, data_to_recv);
        }

        sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;
        break;
    }
    }

    return (0);
}

//*****************************************************************************
//
//! SpiPauseSpi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi pause operation
//
//*****************************************************************************
void SpiPauseSpi(void)
{
    NVIC->ICER[0] = (1 << 7);    
//		NVIC->ICER[1] = (1 << (IRQ_INT_CHANNEL - 32));
	
}

//*****************************************************************************
//
//! SpiResumeSpi
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi resume operation
//
//*****************************************************************************
void SpiResumeSpi(void)
{
   NVIC->ISER[0] = (1 << 7);
//		NVIC->ISER[1] = (1 << (IRQ_INT_CHANNEL - 32));
}

//*****************************************************************************
//
//! SpiTriggerRxProcessing
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Spi RX processing
//
//*****************************************************************************
void SpiTriggerRxProcessing(void)
{

    // Trigger Rx processing
    SpiPauseSpi();
    DEASSERT_CS();

    // The magic number that resides at the end of the TX/RX buffer (1 byte after
    // the allocated size) for the purpose of detection of the overrun. If the
    // magic number is overwritten - buffer overrun occurred - and we will stuck
    // here forever!
    if (sSpiInformation.pRxPacket[CC3000_RX_BUFFER_SIZE - 1]
            != CC3000_BUFFER_MAGIC_NUMBER)
    {
        while (1)
            ;
    }

    sSpiInformation.ulSpiState = eSPI_STATE_IDLE;
    sSpiInformation.SPIRxHandler(sSpiInformation.pRxPacket + SPI_HEADER_SIZE);
}

//*****************************************************************************
//
//!  EXTI11_IRQHandler
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  Interrupt handler. When the external SSI WLAN device is
//!          ready to interact with Host CPU it generates an interrupt signal.
//!          After that Host CPU has registered this interrupt request
//!          it set the corresponding /CS in active state.
//
//*****************************************************************************
void EXTI1_IRQHandler(void)
{
    if (EXTI_GetITStatus(IRQ_INT_LINE) != RESET)
    {
        if (sSpiInformation.ulSpiState == eSPI_STATE_POWERUP)
        {
            //This means IRQ line was low call a callback of HCI Layer to inform
            //on event
            sSpiInformation.ulSpiState = eSPI_STATE_INITIALIZED;
        }
        else if (sSpiInformation.ulSpiState == eSPI_STATE_IDLE)
        {
            sSpiInformation.ulSpiState = eSPI_STATE_READ_IRQ;

            /* IRQ line goes down - we are start reception */
            ASSERT_CS();

            // Wait for TX/RX Compete which will come as DMA interrupt
            SpiReadHeader();

            sSpiInformation.ulSpiState = eSPI_STATE_READ_EOT;

            SSIContReadOperation();
        }
        else if (sSpiInformation.ulSpiState == eSPI_STATE_WRITE_IRQ)
        {
            SpiWriteDataSynchronous(sSpiInformation.pTxPacket,
                    sSpiInformation.usTxPacketLength);

            sSpiInformation.ulSpiState = eSPI_STATE_IDLE;

            DEASSERT_CS();
        }
        EXTI_ClearITPendingBit(IRQ_INT_LINE);
    }
}


void DMA1_Channel3_IRQHandler(void)     //spi 发送dma 中断 
{
	if(DMA_GetITStatus(DMA1_IT_TC3)==SET)
	{
	  DMA_ClearITPendingBit(DMA1_IT_TC3);
	}

}

void DMA1_Channel2_IRQHandler(void)     //spi 发送dma 中断 
{
	if(DMA_GetITStatus(DMA1_IT_TC2)==SET)
	{
	  DMA_ClearITPendingBit(DMA1_IT_TC2);
	}

}

//*****************************************************************************
//
//! SSIContReadOperation
//!
//!  @param  none
//!
//!  @return none
//!
//!  @brief  SPI read operation
//
//*****************************************************************************
void SSIContReadOperation(void)
{
    // The header was read - continue with  the payload read
    if (!SpiReadDataCont())
    {
        // All the data was read - finalize handling by switching to the task
        //	and calling from task Event Handler
        SpiTriggerRxProcessing();
    }
}

//*****************************************************************************
//
//! TXBufferIsEmpty
//!
//!  @param
//!
//!  @return returns 1 if buffer is empty, 0 otherwise
//!
//!  @brief  Indication if TX SPI buffer is empty
//
//*****************************************************************************
long TXBufferIsEmpty(void)
{
    return SPI_I2S_GetFlagStatus(SPI_BASE, SPI_I2S_FLAG_TXE) == SET ? 1 : 0;
}

//*****************************************************************************
//
//! RXBufferIsNotEmpty
//!
//!  @param  none
//!
//!  @return returns 1 if buffer is not empty, 0 otherwise
//!
//!  @brief  Indication if RX SPI buffer is not empty
//
//*****************************************************************************
long RXBufferIsNotEmpty(void)
{
    return SPI_I2S_GetFlagStatus(SPI_BASE, SPI_I2S_FLAG_RXNE) == SET ? 1 : 0;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
