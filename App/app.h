#include 	"includes.h" 

OS_STK GstkStart[100];
OS_STK WLANTaskStk[100];
OS_STK CC1101TaskStk[100];
OS_STK UartTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           			3   //越小  优先级越高 
#define  TASK_WLAN_PRIO    							7
#define  TASK_CC1101_PRIO            		4
#define  TASK_UART_PRIO            			7
#define  TASK_FLASH_PRIO            		8
#define  TASK_LCD_PRIO            			5

static void  	taskStart (void  *parg);
static void  	taskwlan (void  *parg);
static void  	taskcc1101(void  *parg);
static void 	taskflash(void *pdata);
static void 	tasklcd(void *pdata);
static void 	taskuart(void *pdata);


OS_EVENT *CC1101Rec_Semp;  //cc1101接收不能打断   必须要用信号量
INT8U err;

unsigned char WiFi_Status;

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];


unsigned char  DisplayStatus;   //液晶屏显示模式
unsigned char  DataDisplayRefreshFlag =0; 

extern unsigned char TerminalStatus[CLIENTNUM];  //10个终端登陆状态
extern unsigned char Heart_Beat_Flag;

unsigned char CurrentAddress = 0;  //当前地址
unsigned char reconnectnum;


int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //指针地址会出问题的GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;

		OSTaskCreate ( 	taskwlan,//
										(void *)0, 
										&WLANTaskStk[99],     //指针地址会出问题的
										TASK_WLAN_PRIO);   	
		OSTaskCreate ( 	taskcc1101,//
										(void *)0, 
										&CC1101TaskStk[99], 
										TASK_CC1101_PRIO);  
//		OSTaskCreate ( tasklcd,//
//                   (void *)0, 
//                   &LCDTaskStk[99], 
//                   TASK_LCD_PRIO); 
			OSTaskCreate ( taskuart,//
                   (void *)0, 
                   &UartTaskStk[99], 
                   TASK_UART_PRIO); 
//			OSTaskCreate ( taskflash,//
//                   (void *)0, 
//                   &FlashTaskStk[99], 
//                   TASK_FLASH_PRIO); 

    while (1) {   
		BSP_Init();			  //系统时钟初始化    modify by  nevinxu 2014.2.8
		OSTaskSuspend(OS_PRIO_SELF);  
    }
}
/*********************************WIFI 任务函数**********************************************/
void  taskwlan(void  *parg)
{

	(void)parg;
	DisplayStatus = WifiStatusDisplayStatus;   //显示wifi连接状态
  Init_CC3000Driver();  		//初始化cc3000
	
	while(1)
	{
		Wifi_event_handler();  //wifi状态切换

		CC3000DataRecProcess(CurrentAddress);    //接收没有对象之分
		if(ulWifiEvent == WIFI_SEND_RECV)
		{
			OSSemPend(CC1101Rec_Semp,0,&err);//这里必要需要用信号量互斥 
		//	LoginTransmit(CurrentAddress,CC3000Target);   //test
				if(TerminalStatus[CurrentAddress] == 2)   //终端登陆
				{
					TerminalStatus[CurrentAddress] = 3;
					LoginTransmit(CurrentAddress,CC3000Target);
					reconnectnum++;
					if(reconnectnum >= 3)
					{
						reconnectnum = 0;
						SetTcpConnect(TCPClientMode);
					}
				}
				else if (TerminalStatus[CurrentAddress] == 6)
				{
					WorkingStateMsgTransmit(CurrentAddress,CC3000Target);
					TerminalStatus[CurrentAddress] = 7;  //等待状态   cc1101发送数据  激活发送状态
				}
			OSSemPost(CC1101Rec_Semp);
		}

		OSTimeDly(OS_TICKS_PER_SEC/50);
	}
}
/********************************************************************************************/



/*********************************CC1101任务函数**********************************************/
static void taskcc1101(void *pdata)
{
	static unsigned  int CC1101TimeNum;  //计时时间
	static unsigned char LastAddress  = 0xff;
	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //上一次状态
//	unsigned char ReceiveAckFlag = 0;   //发送接收应答
	unsigned char ReceiveAckTimeOut = 0;
	pdata = pdata; 
	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 初始化
	
	while(1)
	{
			CC1101TimeNum++;
			OSSemPend(CC1101Rec_Semp,0,&err);//这里必要需要用信号量互斥 
			if(CC1101TimeNum >= 100)   //2秒向发送终端请求发送数据
			{
//				CC1101TimeNum = 0;
				
/*************************************发送函数************************************************/			

				if(LastAddress != CurrentAddress)
				{
					CC1101AddSet(CurrentAddress);   //cc1101地址设置
				}
				if(TerminalStatus[CurrentAddress] == 0)   //未登陆
				{
					LoginReqTransmit(CurrentAddress,CC1101Target);  //登陆询问
					TerminalStatus[CurrentAddress] = 1;
				}
				else if(TerminalStatus[CurrentAddress] == 4) //登陆
				{
					WorkingStateMsgReqTransmit(CurrentAddress,CC1101Target);
					TerminalStatus[CurrentAddress] = 5;
				}
				else if(TerminalStatus[CurrentAddress] == 8)
				{
					TerminalStatus[CurrentAddress] = 4;
					ReSendCommandTimer[CurrentAddress] = 0;
					ReceiveAckTimeOut = 10;
				}
				
				LastAddress = CurrentAddress;
				
				ReceiveAckTimeOut++;
				if(ReceiveAckTimeOut >= 3)
				{
/*************************************接收函数************************************************/				
					if(CC1101DataRecFlag&CC1101RECDATABIT)
					{
						CC1101ReceivePacket(CC1101RxBuf);   				
						DataDisplayRefreshFlag = 1;
						ReceiveAckTimeOut = 0;
						CC1101DateRecProcess(CurrentAddress);
						Heart_Beat_Flag = 0;
						CC1101DataRecFlag &=~ CC1101RECDATABIT;
					}
/********************************************************************************************/	
				}
				if(ReceiveAckTimeOut >= 10)  //200ms的超时
				{
					ReceiveAckTimeOut = 0;
					if(TerminalStatus[CurrentAddress] < 4)   //无法登陆，一直发送登陆命令
					{
						TerminalStatus[CurrentAddress] = 0;
					}
					else if(TerminalStatus[CurrentAddress] != 8)
					{
						TerminalStatus[CurrentAddress] = 4;
						ReSendCommandTimer[CurrentAddress]++;
						if(ReSendCommandTimer[CurrentAddress] >= 5)   //重连5次
						{
							ReSendCommandTimer[CurrentAddress] = 0;
							TerminalStatus[CurrentAddress] = 0;  //发送重新登陆命令
						}
						
					}
					CurrentAddress++;   //地址的切换时间固定
					if(CurrentAddress>=CLIENTNUM)
					{
						CurrentAddress = 0;
						CC1101TimeNum = 0;
					}
				}		
			}				
		OSSemPost(CC1101Rec_Semp);
		OSTimeDly(OS_TICKS_PER_SEC/50);    //定时时间为20ms
	}
}
/********************************************************************************************/



static void taskuart(void *pdata)
{
	pdata = pdata;

}


/**********************************FLASH任务函数**********************************************/

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
/********************************************************************************************/


/***********************************显示任务函数*********************************************/
static void tasklcd(void *pdata)
{
	static unsigned char LastDisplayStatus = 0;  //显示状态切换 
	pdata = pdata;
	Initial_Lcd();
	DisplayClearAll();

	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);  //一秒刷新一次
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
/********************************************************************************************/