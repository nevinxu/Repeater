#include 	"includes.h" 

OS_STK GstkStart[100];
//OS_STK WLANSENDTaskStk[100];
OS_STK WLANRECEIVETaskStk[100];
OS_STK CC1100TaskStk[100];
OS_STK TestTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           			3   //越小  优先级越高 
//#define  TASK_WLANSEND_PRIO            	6
#define  TASK_WLANRECEIVE_PRIO    			7
#define  TASK_CC1100_PRIO            		4
#define  TASK_TEST_PRIO            			7
#define  TASK_FLASH_PRIO            		8
#define  TASK_LCD_PRIO            			5

static void  	taskStart (void  *parg);
//static void 	taskwlansend (void  *parg);
static void  	taskwlanreceive (void  *parg);
static void  	taskcc1100 (void  *parg);
static void 	taskflash(void *pdata);
static void 	tasklcd(void *pdata);

OS_EVENT *CC1101Rec_Semp;  //cc1101接收不能打断   必须要用信号量
INT8U err;

unsigned char WiFi_Status;

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];
unsigned char  DisplayStatus;   //数据显示状态
unsigned char  DataDisplayRefreshFlag =0; 




int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //指针地址会出问题的GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;

//		OSTaskCreate ( taskwlansend,//
//                   (void *)0, 
//					&WLANSENDTaskStk[99],     //指针地址会出问题的
//                   TASK_WLANSEND_PRIO);  
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
			OSTaskCreate ( tasklcd,//
                   (void *)0, 
                   &LCDTaskStk[99], 
                   TASK_LCD_PRIO); 
    while (1) {   
		BSP_Init();			  //系统时钟初始化    modify by  nevinxu 2014.2.8
		OSTaskSuspend(OS_PRIO_SELF);  
    }
}

void  taskwlanreceive (void  *parg)
{
	(void)parg;
	DisplayStatus = WifiStatusDisplayStatus;
	Board_Init();
	while(1)
	{
		Wifireceive_Function();
		if((WiFi_Status&0xf0) == TCPOVER)
		{
			DisplayStatus = DataDisplayStatus;
		}
		else
		{
			DisplayStatus = WifiStatusDisplayStatus;
		}
		OSTimeDly(OS_TICKS_PER_SEC/2);
	}
}

//void  taskwlansend (void  *parg)
//{
//	(void)parg;
//	while(1)
//	{
//		OSTimeDly(OS_TICKS_PER_SEC/2);
////		OSSemPend(Rate_Semp,0,&err);
//		Wifisend_Function();
////		OSSemPost(Rate_Semp);
//	}
//}

static void taskcc1100(void *pdata)
{
	pdata = pdata; 
	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 初始化
//	DisplayStatus =DataDisplayStatus;
    while(1)
    {
//		OSSemPend(Rate_Semp,0,&err);
			OSTimeDly(OS_TICKS_PER_SEC/10); 
			if(CC1101DataRecFlag&0x01)
			{
				OSSemPend(CC1101Rec_Semp,0,&err);
				CC1101ReceivePacket(CC1101RxBuf);  //这里必要需要用信号量互斥  
				OSSemPost(CC1101Rec_Semp);
				DataDisplayRefreshFlag = 1;
				CC1101DateRecProcess();
				CC1101DataRecFlag &=~ 0x01;
			}
		
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
	static unsigned char LastDisplayStatus = 0;
	pdata = pdata;
	Initial_Lcd();
	DisplayClearAll();

	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);
		OSSemPend(CC1101Rec_Semp,0,&err);
		if(DisplayStatus != LastDisplayStatus )
		{
			DisplayClearAll();
		}
		if(DisplayStatus == WifiStatusDisplayStatus)
		{
			WifiStatusDisplay();
		}
		else if(DisplayStatus == DataDisplayStatus)
		{
			DataDisplay();
		}
		LastDisplayStatus = DisplayStatus;
		OSSemPost(CC1101Rec_Semp);
	}
}
