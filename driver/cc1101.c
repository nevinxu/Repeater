#include  <includes.h>
#include "cc1101.h"
 
u8 PaTabel[8] = {0xC0 ,0xc0 ,0xc0 ,0xc0 ,0xc0 ,0xc0 ,0xc0 ,0xC0};	 // 10dB
extern unsigned char  CC1101DataRecFlag;

extern unsigned char CC1101RxBuf[64];
// RF_SETTINGS is a data structure which contains all relevant CCxxx0 registers

typedef struct S_RF_SETTINGS
{
		u8 FSCTRL2;		//���Ѽӵ�
    u8 FSCTRL1;   // Frequency synthesizer control.
    u8 FSCTRL0;   // Frequency synthesizer control.
    u8 FREQ2;     // Frequency control word, high INT8U.
    u8 FREQ1;     // Frequency control word, middle INT8U.
    u8 FREQ0;     // Frequency control word, low INT8U.
    u8 MDMCFG4;   // Modem configuration.
    u8 MDMCFG3;   // Modem configuration.
    u8 MDMCFG2;   // Modem configuration.
    u8 MDMCFG1;   // Modem configuration.
    u8 MDMCFG0;   // Modem configuration.
    u8 CHANNR;    // Channel number.
    u8 DEVIATN;   // Modem deviation setting (when FSK modulation is enabled).
    u8 FREND1;    // Front end RX configuration.
    u8 FREND0;    // Front end RX configuration.
    u8 MCSM0;     // Main Radio Control State Machine configuration.
    u8 FOCCFG;    // Frequency Offset Compensation Configuration.
    u8 BSCFG;     // Bit synchronization Configuration.
    u8 AGCCTRL2;  // AGC control.
		u8 AGCCTRL1;  // AGC control.
    u8 AGCCTRL0;  // AGC control.
    u8 FSCAL3;    // Frequency synthesizer calibration.
    u8 FSCAL2;    // Frequency synthesizer calibration.
		u8 FSCAL1;    // Frequency synthesizer calibration.
    u8 FSCAL0;    // Frequency synthesizer calibration.
    u8 FSTEST;    // Frequency synthesizer calibration control
    u8 TEST2;     // Various test settings.
    u8 TEST1;     // Various test settings.
    u8 TEST0;     // Various test settings.
    u8 IOCFG2;    // GDO2 output pin configuration
    u8 IOCFG0;    // GDO0 output pin configuration
    u8 PKTCTRL1;  // Packet automation control.
    u8 PKTCTRL0;  // Packet automation control.
    u8 ADDR;      // Device address.
    u8 PKTLEN;    // Packet length.
} RF_SETTINGS;

const RF_SETTINGS rfSettings = 
{
		0x00,
    0x0C,   // FSCTRL1   Frequency synthesizer control.
    0x00,   // FSCTRL0   Frequency synthesizer control.
    0x10,   // FREQ2     Frequency control word, high byte.
    0xA7,   // FREQ1     Frequency control word, middle byte.
    0x62,   // FREQ0     Frequency control word, low byte.
    0x2d,   // MDMCFG4   Modem configuration.
    0x3B,   // MDMCFG3   Modem configuration.
    0x13,   // MDMCFG2   Modem configuration.
    0x22,   // MDMCFG1   Modem configuration.
    0xF8,   // MDMCFG0   Modem configuration.

    0x00,   // CHANNR    Channel number.
    0x62,   // DEVIATN   Modem deviation setting (when FSK modulation is enabled).
    0xB6,   // FREND1    Front end RX configuration.
    0x10,   // FREND0    Front end RX configuration.
    0x18,   // MCSM0     Main Radio Control State Machine configuration.
    0x1D,   // FOCCFG    Frequency Offset Compensation Configuration.
    0x1C,   // BSCFG     Bit synchronization Configuration.
    0xC7,   // AGCCTRL2  AGC control.
    0x00,   // AGCCTRL1  AGC control.
    0xB0,   // AGCCTRL0  AGC control.

    0xEA,   // FSCAL3    Frequency synthesizer calibration.
    0x2A,   // FSCAL2    Frequency synthesizer calibration.
    0x00,   // FSCAL1    Frequency synthesizer calibration.
    0x1f,   // FSCAL0    Frequency synthesizer calibration.
    0x59,   // FSTEST    Frequency synthesizer calibration.
    0x88,   // TEST2     Various test settings.
    0x31,   // TEST1     Various test settings.
    0x09,   // TEST0     Various test settings.
    0x01,   // IOCFG2    GDO2 output pin configuration.
    0x02,   // IOCFG0   GDO0 output pin configuration. Refer to SmartRF?Studio User Manual for detailed pseudo register explanation.

    0x04,   // PKTCTRL1  Packet automation control.
    0x05,   // PKTCTRL0  Packet automation control.
    0x00,   // ADDR      Device address.
    0xFF    // PKTLEN    Packet length.
};


void Delay(__IO uint32_t nTime)
{ 
  uint32_t TimingDelay = nTime;

  while(TimingDelay --);
}

/*******************************************************************************
* ��    �ƣ�unsigned char SPI_ReadByte(void)
* ��    �ܣ�SPI��һ�ֽڳ���
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
*******************************************************************************/
unsigned char SPI_ReadByte(void)
{
  return SPI_ReadWrite(Dummy_Byte);
}
/****************************************************************************
* ��    �ƣ�unsigned char	SPI_ReadWrite(unsigned char writedat)
* ��    �ܣ�SPI1��дһ�ֽڳ���
* ��ڲ���������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
unsigned char	SPI_ReadWrite(unsigned char writedat)
{
	/* Loop while DR register in not emplty */
	while(SPI_I2S_GetFlagStatus(CC1101SPI,SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI2 peripheral */
	SPI_I2S_SendData(CC1101SPI, writedat);
	
	/* Wait to receive a byte */
	while(SPI_I2S_GetFlagStatus(CC1101SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(CC1101SPI);
}
/****************************************************************************
* ��    �ƣ�void SPiCWriteReg(u8 Addr,u8 Value)
* ��    �ܣ�SPI1д�Ĵ���
* ��ڲ�������ַ��������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void SPiCWriteReg(u8 Addr,u8 Value)
{
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Addr);
	SPI_ReadWrite(Value);
	CC1101_CSN_1;	//CS disable
}
/****************************************************************************
* ��    �ƣ�void SpiCWriteBurstReg(u8 Addr,u8 *Buffer,u8 Count)
* ��    �ܣ�SPIд����ֽ�
* ��ڲ�������ַ����������ַ,д�뻺�����ĸ���
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void SpiCWriteBurstReg(u8 Addr,u8 *Buffer,u8 Count)
{
	u8 i=0;
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Addr|WRITE_BURST);
	for(i=0;i<Count;i++)
	{
	 SPI_ReadWrite(Buffer[i]);
	}
	CC1101_CSN_1;		//CS disable 	
}
/****************************************************************************
* ��    �ƣ�u8 SpiCReadReg(u8 Addr)
* ��    �ܣ�SPI��һ���ֽ�
* ��ڲ�������ַ
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
u8 SpiCReadReg(u8 Addr)
{
	unsigned char Data=0;
	CC1101_CSN_0;	//CS enable  
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Addr|READ_SINGLE);
	Data = 	SPI_ReadWrite(0);
	CC1101_CSN_1;	 	//CS disable
	return Data;
}
/****************************************************************************
* ��    �ƣ�void SpiCReadBurstReg(u8 Addr,u8 *Buffer,u8 Count)
* ��    �ܣ�SPI������ֽ�
* ��ڲ�������ַ,��������ַ���������ݸ���
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
void SpiCReadBurstReg(u8 Addr,u8 *Buffer,u8 Count)
{
	u8 i=0 ;
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Addr|READ_BURST);
	for(i=0;i<Count;i++)
	{
		Buffer[i]=SPI_ReadWrite(0);//read data		
	}
	CC1101_CSN_1;	//CS disable 
}
/****************************************************************************
* ��    �ƣ�u8 SpiCReadStatus(u8 Addr)
* ��    �ܣ�SPI��״̬�Ĵ���
* ��ڲ�������ַ
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/ 
u8 SpiCReadStatus(u8 Addr)
{
	unsigned char Data;
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Addr|READ_BURST); //send  addr
	Data = SPI_ReadWrite(0);
	CC1101_CSN_1;	    	//CS disable
	return Data;
}
/****************************************************************************
* ��    �ƣ�void SpiCStrobe(u8 Strobe)
* ��    �ܣ�SPIд����
* ��ڲ���������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void SpiCStrobe(u8 Strobe)
{
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(Strobe);
	CC1101_CSN_1;	 	//CS disable
}
void SPiCPowerUpReset(void)
{
  CC1101_CSN_1;	//CS disable
	GPIO_ResetBits(CC1101GPIO, CC1101SCK);// SCLK=0;
  GPIO_ResetBits(CC1101GPIO, CC1101MOSI);// MOSI=0;
	Delay(200);
	CC1101_CSN_1;	//CS disable
	Delay(200);
	CC1101_CSN_0;	//CS enable 
	Delay(200);
	CC1101_CSN_1;	//CS disable
	Delay(1000);
	CC1101_CSN_0;	//CS enable 
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	SPI_ReadWrite(CCxxx0_SRES);
	while(GPIO_ReadInputDataBit(CC1101GPIO,CC1101MISO))	; //wait for 0
	CC1101_CSN_1;	//CS disable		
}
void WriteRfSetting(void)
{
		SPiCWriteReg(CCxxx0_FSCTRL0,  rfSettings.FSCTRL2);//���Ѽӵ�
    // Write register settings
    SPiCWriteReg(CCxxx0_FSCTRL1,  rfSettings.FSCTRL1);
    SPiCWriteReg(CCxxx0_FSCTRL0,  rfSettings.FSCTRL0);
    SPiCWriteReg(CCxxx0_FREQ2,    rfSettings.FREQ2);
    SPiCWriteReg(CCxxx0_FREQ1,    rfSettings.FREQ1);
    SPiCWriteReg(CCxxx0_FREQ0,    rfSettings.FREQ0);
    SPiCWriteReg(CCxxx0_MDMCFG4,  rfSettings.MDMCFG4);
    SPiCWriteReg(CCxxx0_MDMCFG3,  rfSettings.MDMCFG3);
    SPiCWriteReg(CCxxx0_MDMCFG2,  rfSettings.MDMCFG2);
    SPiCWriteReg(CCxxx0_MDMCFG1,  rfSettings.MDMCFG1);
    SPiCWriteReg(CCxxx0_MDMCFG0,  rfSettings.MDMCFG0);
    SPiCWriteReg(CCxxx0_CHANNR,   rfSettings.CHANNR);
    SPiCWriteReg(CCxxx0_DEVIATN,  rfSettings.DEVIATN);
    SPiCWriteReg(CCxxx0_FREND1,   rfSettings.FREND1);
    SPiCWriteReg(CCxxx0_FREND0,   rfSettings.FREND0);
    SPiCWriteReg(CCxxx0_MCSM0 ,   rfSettings.MCSM0 );
    SPiCWriteReg(CCxxx0_FOCCFG,   rfSettings.FOCCFG);
    SPiCWriteReg(CCxxx0_BSCFG,    rfSettings.BSCFG);
    SPiCWriteReg(CCxxx0_AGCCTRL2, rfSettings.AGCCTRL2);
		SPiCWriteReg(CCxxx0_AGCCTRL1, rfSettings.AGCCTRL1);
    SPiCWriteReg(CCxxx0_AGCCTRL0, rfSettings.AGCCTRL0);
    SPiCWriteReg(CCxxx0_FSCAL3,   rfSettings.FSCAL3);
		SPiCWriteReg(CCxxx0_FSCAL2,   rfSettings.FSCAL2);
		SPiCWriteReg(CCxxx0_FSCAL1,   rfSettings.FSCAL1);
    SPiCWriteReg(CCxxx0_FSCAL0,   rfSettings.FSCAL0);
    SPiCWriteReg(CCxxx0_FSTEST,   rfSettings.FSTEST);
    SPiCWriteReg(CCxxx0_TEST2,    rfSettings.TEST2);
    SPiCWriteReg(CCxxx0_TEST1,    rfSettings.TEST1);
    SPiCWriteReg(CCxxx0_TEST0,    rfSettings.TEST0);
    SPiCWriteReg(CCxxx0_IOCFG2,   rfSettings.IOCFG2);
    SPiCWriteReg(CCxxx0_IOCFG0,   rfSettings.IOCFG0);    
    SPiCWriteReg(CCxxx0_PKTCTRL1, rfSettings.PKTCTRL1);
    SPiCWriteReg(CCxxx0_PKTCTRL0, rfSettings.PKTCTRL0);
    SPiCWriteReg(CCxxx0_ADDR,     rfSettings.ADDR);
    SPiCWriteReg(CCxxx0_PKTLEN,   rfSettings.PKTLEN);
}
//����һ������
void CC1101SendPacket(u8 *TxBuffer,u8 Size)
{
	u16 i = 0;
	SpiCStrobe(CCxxx0_SIDLE);//�������  ���һ��Ҫ��  
  Delay(1000);
	SpiCStrobe(CCxxx0_SFTX);// Flush the TX FIFO buffer.
	Delay(1000);
	SPiCWriteReg(CCxxx0_TXFIFO,Size);//д��С
	SpiCWriteBurstReg(CCxxx0_TXFIFO,TxBuffer,Size);//д��Ҫд������
	Delay(60000);
	SpiCStrobe(CCxxx0_STX);//In IDLE state: Enable TX.
	Delay(100);
	while(!GPIO_ReadInputDataBit(GDO0_GPIO,GDO0))	 //while(!GDO0)
	{
//		if(i>100)  	//��ʱ�ȴ�
//		break;
		Delay(30000);
		i++;		
	} 
	while(GPIO_ReadInputDataBit(GDO0_GPIO,GDO0))	 //while(GDO0)
	{
		if(i>200)  	//��ʱ�ȴ�
		break;
		Delay(100);
		i++;	
	}
	Delay(30000);
	SpiCStrobe(CCxxx0_SIDLE);//�������
//	SpiCStrobe(CCxxx0_SFTX);// ���������
//	SpiCStrobe(CCxxx0_SFRX);// ���������
	SpiCStrobe(CCxxx0_SRX);//�������״̬

}

void ClearRecBuf()
{
	unsigned char i;
	for(i=0;i<64;i++)
	{
		CC1101RxBuf[i] = 0;
	}
}

//�������� 
u8 CC1101ReceivePacket(u8 *RxBuffer)
{
//	u8 Status[2];
	u8 PacketLength;
	ClearRecBuf();
	SpiCStrobe(CCxxx0_SRX);//�������״̬
	if((SpiCReadStatus(CCxxx0_RXBYTES)&BYTES_IN_RXFIFO))//������ܵ��ֽ�����Ϊ0
	{
		SpiCReadBurstReg(CCxxx0_RXFIFO,RxBuffer,4);//��������
		if((RxBuffer[0] == 0xb4) && (RxBuffer[1] == 0xa5))
		{
			PacketLength = RxBuffer[2] - 4;   //Э��涨Ϊ16�ֽڳ���  ������
			SpiCReadBurstReg(CCxxx0_RXFIFO,RxBuffer+4,PacketLength);//��������
		}
		else
		{
			SpiCStrobe(CCxxx0_SFRX);//clear Buffer �������	
			SpiCStrobe(CCxxx0_SRX);//�������״̬
			return 0;
		}
		SpiCStrobe(CCxxx0_SFRX);//clear Buffer �������
		SpiCStrobe(CCxxx0_SRX);//�������״̬
		Delay(100);
		return 1;
	}
}

void GDO2EXTI_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GDO2, ENABLE);
  
  /* Configure PA.00 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GDO2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GDO2_GPIO, &GPIO_InitStructure);

  /* Enable AFIO clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

  /* Connect EXTI0 Line to PA.00 pin */
  GPIO_EXTILineConfig(GPIO_PortSourceGPO2, GPIO_PinSourceGPO2);

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line_GPO2;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI1 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI_IRQn_GPO2;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void CC1100_GPIO_Configuration()
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	SPI_InitTypeDef SPI_InitStructure;
	
	/*ʹ��SPI2ʱ�� */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI_CC1101, ENABLE);
	/* ʹ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIO_CC1101 | RCC_APB2Periph_GPIO_GDO0 | RCC_APB2Periph_AFIO,ENABLE);

	//����SPI2�ӿ� PB13��14��15
	GPIO_InitStructure.GPIO_Pin = CC1101MISO | CC1101MOSI | CC1101SCK;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(CC1101GPIO, &GPIO_InitStructure);

	GDO2EXTI_Config();
	
//   PB11-GDO0
	GPIO_InitStructure.GPIO_Pin = GDO0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GDO0_GPIO, &GPIO_InitStructure);
	//CS-PB12
	GPIO_InitStructure.GPIO_Pin = CC1101CSN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(CC1101GPIO, &GPIO_InitStructure);
	GPIO_SetBits(CC1101GPIO, CC1101CSN); 	            // disable cc1101 cs 
		
	SPI_Cmd(CC1101SPI,DISABLE);
	SPI_InitStructure.SPI_Direction=SPI_Direction_2Lines_FullDuplex;//SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode=SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize=SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL=SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA=SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS=SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler=SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit=SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial=7;
	SPI_Init(CC1101SPI,&SPI_InitStructure);
	SPI_Cmd(CC1101SPI,ENABLE);
}						    

//��ʼ��
void CC1101Init(void)
{	
	CC1100_GPIO_Configuration();
	SPiCPowerUpReset();
	WriteRfSetting();
	SpiCWriteBurstReg(CCxxx0_PATABLE,PaTabel,1);//��������
	SPiCWriteReg(CCxxx0_FIFOTHR,0x0e);
	Delay(1000);	
	
	SpiCStrobe(CCxxx0_SIDLE);//�������
	SpiCStrobe(CCxxx0_SFRX);// ���������
	SpiCStrobe(CCxxx0_SRX);//�������״̬
}

void EXTI9_5_IRQHandler(void)
{
	if (EXTI_GetITStatus(EXTI_Line_GPO2) != RESET)
	{	
		CC1101DataRecFlag = 0x11;
		EXTI_ClearITPendingBit(EXTI_Line_GPO2);
	}
}

