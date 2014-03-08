#include 	"includes.h" 


//我的修改 2014.3.5

OS_STK GstkStart[100];
OS_STK WLANTaskStk[100];
OS_STK CC1100TaskStk[100];
OS_STK TestTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           3
#define  TASK_WLAN_PRIO            9
#define  TASK_CC1100_PRIO            4
#define  TASK_TEST_PRIO            5
#define  TASK_FLASH_PRIO            6
#define  TASK_LCD_PRIO            7

static void  taskStart (void  *parg);
static void  taskwlan (void  *parg);
static void  taskcc1100 (void  *parg);
//static void tasktest(void *pdata);
static void taskflash(void *pdata);
static void tasklcd(void *pdata);


extern unsigned short Rxlen;
extern unsigned short WIFIRxLen;
extern unsigned char WIFIRxBuf[WIFI_RX_BUF_MAX];

extern unsigned short WIFITxLen;
extern unsigned char WIFITxBuf[WIFI_TX_BUF_MAX];

extern unsigned short Relay_Flag;

volatile unsigned short Relay_Cty_Flag = 0;


//unsigned short Server_Port;
//unsigned long Server_IP;

unsigned char WiFi_Status;


volatile unsigned long EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
volatile unsigned char SysEvent = 0;

void SetEventTimeOut(unsigned char event,unsigned long  TimeOut)
{
  SysEvent &= ~event;
  if(TimeOut)
  {
   switch(event)
   {
     case 0x01:EventTimeOut[0] = TimeOut;break;
     case 0x02:EventTimeOut[1] = TimeOut;break;
     case 0x04:EventTimeOut[2] = TimeOut;break;
     case 0x08:EventTimeOut[3] = TimeOut;break;
     case 0x10:EventTimeOut[4] = TimeOut;break;
     case 0x20:EventTimeOut[5] = TimeOut;break;
     case 0x40:EventTimeOut[6] = TimeOut;break;
     case 0x80:EventTimeOut[7] = TimeOut;break;
     default:break;
   }
  }
}

void SetEvent(unsigned char  event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent |= event;
}

void ClearEvent(unsigned char event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent &= ~event;
}

void GetEvent(void)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(EventTimeOut[i])
    {
     if(--EventTimeOut[i]==0)
     {
       SysEvent |= 0x01<<i;
     }
    }
  }
}

int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //指针地址会出问题的GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;

	    OSTaskCreate ( taskwlan,//
                   (void *)0, 
										&WLANTaskStk[99],     //指针地址会出问题的
                   TASK_WLAN_PRIO);     
//			OSTaskCreate ( taskcc1100,//
//                   (void *)0, 
//                   &CC1100TaskStk[99], 
//                   TASK_CC1100_PRIO);  
//			OSTaskCreate ( tasktest,//
//                   (void *)0, 
//                   &TestTaskStk[99], 
//                   TASK_TEST_PRIO); 
//			OSTaskCreate ( taskflash,//
//                   (void *)0, 
//                   &FlashTaskStk[99], 
//                   TASK_FLASH_PRIO); 
//			OSTaskCreate ( tasklcd,//
//                   (void *)0, 
//                   &LCDTaskStk[99], 
//                   TASK_LCD_PRIO); 

  
    while (1) {   
				BSP_Init();			  //系统时钟初始化    modify by  nevinxu 2014.2.8
        OSTaskSuspend(OS_PRIO_SELF);  
    }
}

void  taskwlan (void  *parg)
{
	unsigned char Heart_Beat_Flag = 0;
  (void)parg;
	Board_Init();
	while(1)
	{
		Wifi_event_handler();
    if(SysEvent & RECV_EVENT_HANDLER)
    {
			Heart_Beat_Flag++;
      ClearEvent(RECV_EVENT_HANDLER);
      if(Rxlen)
      {
        if(WIFIRxBuf[Rxlen-1]== '1')
        {
          Heart_Beat_Flag = 0;
        } 
        memset (WIFIRxBuf, 0, Rxlen);  
        Rxlen = 0;
      }
			if(Heart_Beat_Flag >= 5)
			{
				Heart_Beat_Flag = 0;
				ReConnectSocket(DEVICE_LAN_IP,DEVICE_LAN_PORT,TCPClient_Mode);
				OSTimeDly(OS_TICKS_PER_SEC);
				
			}
      SetEventTimeOut(RECV_EVENT_HANDLER,50);
      continue;
    }
    if(SysEvent & SEND_EVENT_HANDLER)
    {
      ClearEvent(SEND_EVENT_HANDLER);
			SendRateData(70);
      SetEventTimeOut(SEND_EVENT_HANDLER,50);
      continue;
    }
    if(SysEvent & LED_EVENT_HANDLER)
    {
      ClearEvent(LED_EVENT_HANDLER);
      if(Relay_Cty_Flag)
      {
        Relay_Cty_Flag = 0;
        SetRELAYToggle();
      }
      SetEventTimeOut(LED_EVENT_HANDLER,50);
      continue;
    }
    if(SysEvent & LOCAL_CONTROL_EVENT_HANDLER)
    {
      ClearEvent(ALL_EVENT_HANDLER);
      Relay_Cty_Flag = 1;
      SetEvent(RECV_EVENT_HANDLER | SEND_EVENT_HANDLER | LED_EVENT_HANDLER);  
    }
			OSTimeDly(OS_TICKS_PER_SEC);
    }
}

static void taskcc1100(void *pdata)
{
		unsigned char TxBuf[30]={0};
    pdata = pdata;
		CC1101Init();                                    //CC1101 初始化

    while(1)
    {
			OSTimeDly(OS_TICKS_PER_SEC/2); 
			SpiCStrobe(CCxxx0_SIDLE);    //进入空闲
			TxBuf[1]=1 ; 
			TxBuf[2]=1 ; 
			SpiCSendPacket(TxBuf,30);
    }
}

//static void tasktest(void *pdata)
//{
////    INT8U led_state;
////		unsigned char ptr[8] = "hello\n";
//    pdata = pdata;
////    led_state = 1;
////    LED_ON();
////		Com1Init(9600);
//    while(1)
//    {
//       OSTimeDly(OS_TICKS_PER_SEC); 
////        if(led_state == 0) {
////            led_state = 1;
//////					Com1Send(ptr,6);
////            LED_ON();
////        } else {
////            led_state = 0;
////            LED_OFF();
////        }

//    }
//}

static void taskflash(void *pdata)
{
	volatile unsigned char	DeviceID = 0;
	volatile unsigned int FlashID = 0;
	pdata = pdata;
	SPI_FLASH_Init();
  /* Get SPI Flash Device ID */
  DeviceID = SPI_FLASH_ReadDeviceID();  
	Delay( 200 );
  /* Get SPI Flash ID */
  FlashID = SPI_FLASH_ReadID();
    while(1)
    {
			OSTimeDly(OS_TICKS_PER_SEC/2); 
    }
}

static void tasklcd(void *pdata)
{
	static unsigned Display_Index=0;
	pdata = pdata;
	Initial_Lcd();
	clear_All();
	Display_WiFi_Status_Code();
	while(1)
	{
			OSTimeDly(OS_TICKS_PER_SEC/2);
			if((WiFi_Status&0x0f) == NOCONNECT)
			{
				Display_WiFi_Status_NoConnect();
			}
			else if((WiFi_Status&0x0f)  == SCANNING)
			{
				Display_WiFi_Status_Scanning();
			}
			else if((WiFi_Status&0x0f)  == SCANOVER)
			{
				Display_WiFi_Status_Scanfinish();
			}
			else if ((WiFi_Status&0x0f)  == CONNECTING)
			{
				if((Display_Index%4)==0)
				{
					Display_WiFi_Status_Connecting();
				}
				else if((Display_Index%4)==2)
				{
					Display_Line_Clear(1);
				}
				Display_WiFi_Router_Name();
			}
			else if ((WiFi_Status&0x0f)  == CONNECTED)
			{
				Display_WiFi_Status_Connected();
				if((Display_Index%15)==0)
				{
					Display_Connect_Type();
				}
				else if((Display_Index%15)==5)
				{
					Display_WiFi_Router_Name();
				}
				else if((Display_Index%15)==10)
				{
					Display_WiFi_TCP_Status();
				}
				
			}
			else if ((WiFi_Status&0x0f)  == RECONNECTED)
			{
				Display_WiFi_Status_ReConnecting();
			}
			Display_Index++;
	}
}
