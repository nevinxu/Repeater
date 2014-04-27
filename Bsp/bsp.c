#include "includes.h" 
#include "evnt_handler.h"
#include "wifi_application.h"

#define BSP_GLOBALS

volatile unsigned int delayCount = 0;

#ifdef  DEBUG_WU 
volatile unsigned char g_ucUARTBuffer[UART_IF_BUFFER];
volatile unsigned char g_ucUARTFlag = 0;
volatile unsigned long g_ulRxBuffCount = 0;
#endif

//��̬��������
//static void SysTick_Init(void);
#ifdef TEST_DEMO
static void TIM4_Mode_Config(void);
#else
static void TIM3_Mode_Config(void);
#endif
static void SysTick_Configuration(void);

volatile unsigned char Key_event_Flag = 0;
volatile unsigned short Key_event_timeout = 0;

volatile unsigned short Relay_Flag = 0;


void BSP_Init(void)
{
#ifdef DEBUG_IN_RAM
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);
#endif 
    SysTick_Configuration();
}

static void SysTick_Configuration(void)
{
    if (SysTick_Config(SystemCoreClock / OS_TICKS_PER_SEC))
    { 
        /* Capture error */ 
        while (1);
    }
}
 
uint32_t  BSP_CPU_ClkFreq (void)
{
  RCC_ClocksTypeDef  rcc_clocks;

  RCC_GetClocksFreq(&rcc_clocks);
  return ((uint32_t)rcc_clocks.HCLK_Frequency);
}


uint32_t  OS_CPU_SysTickClkFreq (void)
{
  uint32_t  freq;
	freq = BSP_CPU_ClkFreq();
  return (freq);
}

//��ʱ����
void delay_ms(unsigned int ms)
{
  delayCount = (int) ms/10;
  while (delayCount > 0);
}

////ʱ�ӽ��ĳ�ʼ��
//static void SysTick_Init(void)
//{
//  SysTick_Config(SystemCoreClock / 1000);
//}

//IO��ʼ��
void HW_IO_init(void)
{
  GPIO_InitTypeDef gpio;
  
  RCC_APB2PeriphClockCmd(LED_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
  
  RCC_APB2PeriphClockCmd(PWM_32K_CLOCK, ENABLE);
  RCC_APB2PeriphClockCmd(CC3000_EN_CLOCK, ENABLE);
  RCC_APB2PeriphClockCmd(WL_EN_CLOCK, ENABLE);
//  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  
//  RCC_APB2PeriphClockCmd(KEY_CLOCK, ENABLE);            //������ʼ��
//  //Remap JTAG (SWD ONLY)
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  //״ָ̬ʾ��
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED_CONFIG_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_BASE, &gpio);

  GPIO_ResetBits(LED_BASE, LED_CONFIG_PIN);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
	GPIO_SetBits(LED_BASE, LED_CONFIG_PIN);  //�ϵ��Ĭ��Ϊ�ر�״ָ̬ʾ��
  
  // ���Ƶ� 
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED_RELAY_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED_BASE, &gpio);
  GPIO_ResetBits(LED_BASE, LED_RELAY_PIN);
  
  //PWM_32K_OUTPUT
  gpio.GPIO_Mode = GPIO_Mode_AF_OD;
  gpio.GPIO_Pin = PWM_32K_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(PWM_32K_BASE, &gpio);
  
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
  
//  //KEY
//  gpio.GPIO_Mode = GPIO_Mode_IPU;
//  gpio.GPIO_Pin = KEY_PIN;
//  gpio.GPIO_Speed = GPIO_Speed_50MHz;
//  GPIO_Init(KEY_BASE, &gpio);  
//  
//  //KEY �ж�
//  GPIO_EXTILineConfig(KEY_INT_PORT, KEY_INT_PIN);
//  
//  exti.EXTI_Line = KEY_INT_LINE;
//  exti.EXTI_Mode = EXTI_Mode_Interrupt;
//  exti.EXTI_Trigger = EXTI_Trigger_Falling;    //�½��ش���
//  exti.EXTI_LineCmd = ENABLE;
//  EXTI_Init(&exti);
//  
//  nvic.NVIC_IRQChannel = KEY_INT_CHANNEL;
//  nvic.NVIC_IRQChannelPreemptionPriority = 0x0f;
//  nvic.NVIC_IRQChannelSubPriority = 0x0f;
//  nvic.NVIC_IRQChannelCmd = ENABLE;
//  NVIC_ClearPendingIRQ(KEY_INT_CHANNEL);
//  NVIC_Init(&nvic);
  
}


#ifdef TEST_DEMO
//TIM4_Mode_Config PWM_32K_output
static void TIM4_Mode_Config(void)
{
  //��ʱ����ʼ���ṹ
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //��ʱ��ͨ����ʼ���ṹ
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 2196;     //F103
  //����Ԥ���Ƶ��
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷ�Ƶϵ����
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
  
  //PWM1 Mode configuration: Channel3
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  //��������ֵ�������������������ֵʱ����ƽ��������

  TIM_OCInitStructure.TIM_Pulse = 1098;   //F103
  //����ʱ������ֵС��CCR1����Val ʱΪ�ߵ�ƽ
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
  TIM_OC4Init(TIM4, &TIM_OCInitStructure);//ʹ��ͨ��3
  TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM4, ENABLE);
  //ʹ�ܶ�ʱ��4
  TIM_Cmd(TIM4, DISABLE);
}
#else

//TIM3_Mode_Config PWM_32K_output
static void TIM3_Mode_Config(void)
{
  //��ʱ����ʼ���ṹ
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  //��ʱ��ͨ����ʼ���ṹ
  TIM_OCInitTypeDef TIM_OCInitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
  
  TIM_TimeBaseStructure.TIM_Period = 2196;     //F103
  //����Ԥ���Ƶ��
  TIM_TimeBaseStructure.TIM_Prescaler = 0;
  
  TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷ�Ƶϵ����
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
  
  //PWM1 Mode configuration: Channel3
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //����ΪPWMģʽ1
  
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  //��������ֵ�������������������ֵʱ����ƽ��������

  TIM_OCInitStructure.TIM_Pulse = 1098;   //F103
  //����ʱ������ֵС��CCR1����Val ʱΪ�ߵ�ƽ
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);//ʹ��ͨ��3
  TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_ARRPreloadConfig(TIM3, ENABLE);
  //ʹ�ܶ�ʱ��3
  TIM_Cmd(TIM3, DISABLE);
}
#endif

void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  TIM_DeInit(TIM2);                                            //��λTIM2��ʱ��
  
  /* TIM2 configuration */
  TIM_TimeBaseStructure.TIM_Period = 200;                     // �Զ���װ�Ĵ�����ֵ(��ʱ����100ms)  
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;                 // ʱ��Ԥ��Ƶ��     
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // ʱ�ӷ�Ƶ 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ���
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);                        // Clear TIM2 update pending flag[���TIM2����жϱ�־] 
   
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                   // Enable TIM2 Update interrupt [TIM2����ж�����]
    
  TIM_Cmd(TIM2, ENABLE);                                       // TIM2 enable counter [����tim2����]
  
  //�������ȼ���
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //����TIM2��
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  //�����������ȼ�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //������Ӧ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //ʹ�����ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //���ô���Ĵ���
  NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
}

#ifdef  DEBUG_WU 
void UARTConfigure(void)
{

    USART_InitTypeDef usart;
    GPIO_InitTypeDef gpio;
    NVIC_InitTypeDef nvic;

    //Clock
    RCC_APB1PeriphClockCmd(USART_CLOCK, ENABLE);
    RCC_APB2PeriphClockCmd(UART_TX_CLOCK, ENABLE);
     RCC_APB2PeriphClockCmd(UART_RX_CLOCK, ENABLE);
    //GPIO - TX pin
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    gpio.GPIO_Pin = UART_TX_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_TX_BASE, &gpio);
    //GPIO - RX pin
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Pin = UART_RX_PIN;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(UART_RX_BASE, &gpio);
    //USART
    usart.USART_BaudRate = 115200;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART_BASE, &usart);
    USART_Cmd(USART_BASE, ENABLE);
    //Interrupt
    USART_ITConfig(USART_BASE, USART_IT_RXNE, ENABLE);
    //NVIC
    nvic.NVIC_IRQChannel = USART_INT_CHANNEL;
    nvic.NVIC_IRQChannelPreemptionPriority = 0x0F;
    nvic.NVIC_IRQChannelSubPriority = 0x0F;
    nvic.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&nvic);
}
#endif


#ifdef  DEBUG_WU 
void UartSendPacket(char *inBuff, unsigned short usLength)
{
    unsigned long ulIndex = 0;
    for (ulIndex = 0; ulIndex < usLength; ulIndex++)
    {
        while (USART_GetFlagStatus(USART_BASE, USART_FLAG_TXE) != SET)
            ;
        USART_SendData(USART_BASE, *inBuff++);
    }
}
#endif


void Board_Init(void)
{ 
//  SysTick_Init();
  
  HW_IO_init();
	
#ifdef TEST_DEMO	
  TIM4_Mode_Config();
#else
  TIM3_Mode_Config();	
#endif	
  
  TIM2_Configuration();
  
  Init_CC3000Driver();
  
  delay_ms(100);  
}


#ifdef  DEBUG_WU 
void USART2_IRQHandler(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {	

        g_ucUARTBuffer[g_ulRxBuffCount] = USART_ReceiveData(USART_BASE);
        
        if(g_ucUARTBuffer[g_ulRxBuffCount-1] == 0x0D&& g_ucUARTBuffer[g_ulRxBuffCount] == 0x0A)
        {
          g_ucUARTFlag = 1;
        }
        g_ulRxBuffCount++;

        /*Check for buffer overrun*/

        if (g_ulRxBuffCount >= UART_IF_BUFFER) g_ulRxBuffCount = 0;

    }
}
#endif

//void Key_Event_Handler(void)
//{
//  if(!GPIO_ReadInputDataBit(KEY_BASE, KEY_PIN))
//  {
//    if(++Key_event_timeout>=3000)
//    {
//      ulWifiEvent = WIFI_SMARTCONFIG;
//      Key_event_Flag = 0;
//      Key_event_timeout = 0;
//    }
//  }
//  else
//  {
//    if(Key_event_timeout > 80)
//    {
//      SetEvent(LOCAL_CONTROL_EVENT_HANDLER);
//    }
//    Key_event_Flag = 0;
//    Key_event_timeout = 0;
//  }
//}

void SetLEDStatus(unsigned char status)
{
  if(status)
  {
    GPIO_SetBits(LED_BASE, LED_CONFIG_PIN);
  }
  else
  {
    GPIO_ResetBits(LED_BASE, LED_CONFIG_PIN);
  }
}

void SetLEDToggle(void)
{
  if(GPIO_ReadInputDataBit(LED_BASE, LED_CONFIG_PIN))
  {
    GPIO_ResetBits(LED_BASE, LED_CONFIG_PIN);
  }
  else
  {
    GPIO_SetBits(LED_BASE, LED_CONFIG_PIN); 
  }
}

void SetRELAYToggle(void)
{
  if(GPIO_ReadInputDataBit(LED_BASE, LED_RELAY_PIN ))
  {
    GPIO_ResetBits(LED_BASE, LED_RELAY_PIN );
    Relay_Flag = 0;
  }
  else
  {
    GPIO_SetBits(LED_BASE, LED_RELAY_PIN );
    Relay_Flag = 1; 
  }
}

//��ʱ��2�жϺ���
void TIM2_IRQHandler(void)
{ 
  static unsigned char hci_unsolicited_timeout = 0;
  static unsigned char wifi_status_led_timeout = 0;
  static unsigned char sendratadata_timeout = 0;

  if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)
  {
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    
    if (ulWifiEvent == WIFI_SEND_RECV)
    {
      if(++hci_unsolicited_timeout>=5)
      {
        hci_unsolicited_timeout = 0;
        hci_unsolicited_event_handler();
      }
      if(++sendratadata_timeout >= 10)
      {
        sendratadata_timeout =0;
      }
       
      SetLEDStatus(TRUE); 
    }  
    else if(ulWifiEvent == WIFI_CONNECTING)
    {
      if(++wifi_status_led_timeout>=5)
      {
        wifi_status_led_timeout = 0;
        SetLEDToggle();
      }
    }
    else if(ulWifiEvent == WIFI_SMARTCONFIG)
    {
      SetLEDToggle();
    }
    else
    {
      {
        SetLEDStatus(FALSE); 
      }
    }   
  }
}

//void EXTI15_10_IRQHandler(void)
//{
//    if (EXTI_GetITStatus(KEY_INT_LINE) != RESET)
//   {
//      EXTI_ClearITPendingBit(KEY_INT_LINE);
//      if(Key_event_Flag == 0)
//      {
//        Key_event_timeout = 0;
//        Key_event_Flag = 1;
//      }
//   }
//}  




/*******************************************************************************
 * Function Name  : SysTick_Handler
 * Description    : This function handles SysTick Handler.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SysTick_Handler(void)
{ 
	
    OS_CPU_SR  cpu_sr;


    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
	  GetEvent();
  if(delayCount)delayCount--;
//  if(Key_event_Flag)Key_Event_Handler(); 
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
}
   





#ifdef USE_FULL_ASSERT
/*
 Function    : assert_failed
 Description : Reports the name of the source file and the source
 line number where the assert_param error has occurred.
 Input       : - file: pointer to the source file name
 - line: assert_param error line source number
 Output      : None
 Return      : None
 */
void assert_failed(uint8_t* file, uint32_t line)
{
    /*
     User can add his own implementation to report the file name
     and line number, ex:
     printf("Wrong parameters value: file %s on line %d\r\n", file, line)
     */
    /* Infinite loop */
    while (1)
    {
    }
}
#endif

