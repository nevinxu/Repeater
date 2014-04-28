#include "includes.h" 
#include "evnt_handler.h"
#include "wifi_application.h"

//��̬��������
static void SysTick_Configuration(void);
volatile unsigned int delayCount = 0;
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

//IO��ʼ��
void HW_IO_init(void)
{
  GPIO_InitTypeDef gpio;
  
  RCC_APB2PeriphClockCmd(LED_CLOCK, ENABLE);    //LEDʱ�ӳ�ʼ��
  
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
  
}




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


void Board_Init(void)
{  
  HW_IO_init();	
  TIM2_Configuration(); 
  Init_CC3000Driver();  
  delay_ms(100);  
}

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

