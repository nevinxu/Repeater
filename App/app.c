#include 	"includes.h" 

OS_STK GstkStart[100];
OS_STK WLANSENDTaskStk[100];
OS_STK WLANRECEIVETaskStk[100];
OS_STK CC1100TaskStk[100];
OS_STK TestTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           3
#define  TASK_WLANSEND_PRIO            13
#define  TASK_WLANRECEIVE_PRIO            11
#define  TASK_CC1100_PRIO            12
#define  TASK_TEST_PRIO            5
#define  TASK_FLASH_PRIO            6
#define  TASK_LCD_PRIO            7

static void  taskStart (void  *parg);
static void  taskwlansend (void  *parg);
static void  taskwlanreceive (void  *parg);
static void  taskcc1100 (void  *parg);
static void taskflash(void *pdata);
static void tasklcd(void *pdata);

OS_EVENT *Rate_Semp;
INT8U err;

unsigned char WiFi_Status;


int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //指针地址会出问题的GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;

	    OSTaskCreate ( taskwlansend,//
                   (void *)0, 
					&WLANSENDTaskStk[99],     //指针地址会出问题的
                   TASK_WLANSEND_PRIO);  
	    OSTaskCreate ( taskwlanreceive,//
                   (void *)0, 
					&WLANRECEIVETaskStk[99],     //指针地址会出问题的
                   TASK_WLANRECEIVE_PRIO);   	
		OSTaskCreate ( taskcc1100,//
                   (void *)0, 
                   &CC1100TaskStk[99], 
                   TASK_CC1100_PRIO);  
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

void  taskwlanreceive (void  *parg)
{
	(void)parg;
	Rate_Semp = OSSemCreate (1); 
	OSSemPend(Rate_Semp,0,&err);
	Board_Init();
	while(1)
	{
	Wifireceive_Function();
	OSTimeDly(OS_TICKS_PER_SEC/2);
    }
}

void  taskwlansend (void  *parg)
{
	(void)parg;
	while(1)
	{
	OSSemPend(Rate_Semp,0,&err);
	Wifisend_Function();
	OSSemPost(Rate_Semp);
    }
}

static void taskcc1100(void *pdata)
{
	unsigned char TxBuf[30]={0};
    pdata = pdata; 
//	CC1101Init();                                    //CC1101 初始化

    while(1)
    {
		OSSemPend(Rate_Semp,0,&err);
//		OSTimeDly(OS_TICKS_PER_SEC); 
//		OSTimeDly(OS_TICKS_PER_SEC); 
		OSTimeDly(OS_TICKS_PER_SEC/10); 				
		OSSemPost(Rate_Semp);
//			SpiCStrobe(CCxxx0_SIDLE);    //进入空闲
//			TxBuf[1]=1 ; 
//			TxBuf[2]=1 ; 
//			SpiCSendPacket(TxBuf,30);
    }
}


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
