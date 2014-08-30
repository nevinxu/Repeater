#include "includes.h" 
#include "evnt_handler.h"
#include "wifi_application.h"

GPIO_TypeDef* LED_PORT[LEDn] = {LED1_GPIO_PORT, LED2_GPIO_PORT, LED3_GPIO_PORT,
                                LED4_GPIO_PORT};
const uint16_t LED_PIN[LEDn] ={
    LED1_GPIO_PIN, LED2_GPIO_PIN, LED3_GPIO_PIN,
    LED4_GPIO_PIN
};
	

//静态函数申明
static void SysTick_Configuration(void);
volatile unsigned int delayCount = 0;
volatile unsigned char Key_event_Flag = 0;
volatile unsigned short Key_event_timeout = 0;
volatile unsigned short Relay_Flag = 0;



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


//获取当前设置频率
uint32_t  OS_CPU_SysTickClkFreq (void)
{
  uint32_t  freq;
	freq = BSP_CPU_ClkFreq();
  return (freq);
}

//延时函数
void delay_ms(unsigned int ms)
{
  delayCount = (int) ms/10;
  while (delayCount > 0);
}

//IO初始化
void HW_IO_init(void)
{
  GPIO_InitTypeDef gpio;
  
  RCC_APB2PeriphClockCmd(LED1_CLOCK, ENABLE);    //LED时钟初始化
	RCC_APB2PeriphClockCmd(LED2_CLOCK, ENABLE);    //LED时钟初始化
	RCC_APB2PeriphClockCmd(LED3_CLOCK, ENABLE);    //LED时钟初始化
	RCC_APB2PeriphClockCmd(LED4_CLOCK, ENABLE);    //LED时钟初始化
  
//  //Remap JTAG (SWD ONLY)
//  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
  
  //状态指示灯
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED1_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED2_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED2_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED3_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED3_GPIO_PORT, &gpio);
	
	gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  gpio.GPIO_Pin = LED4_GPIO_PIN;
  gpio.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED4_GPIO_PORT, &gpio);

  SetLEDStatus(LED1,FALSE);  //上电后默认为关闭状态指示灯
	SetLEDStatus(LED2,FALSE);  //上电后默认为关闭状态指示灯
	SetLEDStatus(LED3,FALSE);  //上电后默认为关闭状态指示灯
	SetLEDStatus(LED4,FALSE);  //上电后默认为关闭状态指示灯
  
}




void TIM2_Configuration(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
  
  TIM_DeInit(TIM2);                                            //复位TIM2定时器
  
  /* TIM2 configuration */
  TIM_TimeBaseStructure.TIM_Period = 200;                     // 自动重装寄存器的值(定时周期100ms)  
  TIM_TimeBaseStructure.TIM_Prescaler = 35999;                 // 时钟预分频数     
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      // 时钟分频 
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    
  TIM_ClearFlag(TIM2, TIM_FLAG_Update);                        // Clear TIM2 update pending flag[清除TIM2溢出中断标志] 
   
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);                   // Enable TIM2 Update interrupt [TIM2溢出中断允许]
    
  TIM_Cmd(TIM2, ENABLE);                                       // TIM2 enable counter [允许tim2计数]
  
  //设置优先级组
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
  //设置TIM2线
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  //配置抢断优先级
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  //配置响应优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  //使能优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  //设置存入寄存器
  NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
}


void BSP_Init(void)
{
#ifdef DEBUG_IN_RAM
    NVIC_SetVectorTable(NVIC_VectTab_RAM, 0);
#endif 
    SysTick_Configuration();
	
	  HW_IO_init();					//LED灯初始化
		TIM2_Configuration();    //定时器初始化  用于按键   其他延时等
}


void SetLEDStatus(Led_TypeDef Led, unsigned char status)
{
  if(!status)
  {
    GPIO_SetBits(LED_PORT[Led], LED_PIN[Led]);
  }
  else
  {
    GPIO_ResetBits(LED_PORT[Led], LED_PIN[Led]);
  }
}

void SetLEDToggle(Led_TypeDef Led)
{
  if(GPIO_ReadInputDataBit(LED_PORT[Led],LED_PIN[Led]))
  {
    GPIO_ResetBits(LED_PORT[Led],LED_PIN[Led]);
  }
  else
  {
    GPIO_SetBits(LED_PORT[Led], LED_PIN[Led]); 
  }
}

void SetRELAYToggle(Led_TypeDef Led)
{
  if(GPIO_ReadInputDataBit(LED_PORT[Led],LED_PIN[Led] ))
  {
    GPIO_ResetBits(LED_PORT[Led],LED_PIN[Led] );
    Relay_Flag = 0;
  }
  else
  {
    GPIO_SetBits(LED_PORT[Led],LED_PIN[Led]);
    Relay_Flag = 1; 
  }
}

//定时器2中断函数
void TIM2_IRQHandler(void)
{ 
  static unsigned char hci_unsolicited_timeout = 0;
  static unsigned char wifi_status_led_timeout = 0;
  static unsigned char sendratadata_timeout = 0;

  if(TIM_GetITStatus(TIM2,TIM_IT_Update) == SET)   //定时器时间到
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
       
      SetLEDStatus(LED1,TRUE); 
    }  
    else if(ulWifiEvent == WIFI_CONNECTING)  //正在连接WIFI
    {
      if(wifi_status_led_timeout++>=5)
      {
        wifi_status_led_timeout = 0;
        SetLEDToggle(LED3);
      }
    }
		else if(ulWifiEvent == WIFI_SOCKETCONNECTING)  //正在连接TCP/IP
    {
      if(wifi_status_led_timeout++>=5)
      {
        wifi_status_led_timeout = 0;
        SetLEDToggle(LED2);
      }
    }
		
    else if(ulWifiEvent == WIFI_SMARTCONFIG)
    {
      SetLEDToggle(LED1);
    }
    else
    {
;
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

