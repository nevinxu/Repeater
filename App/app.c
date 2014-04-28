#include 	"includes.h" 

OS_STK GstkStart[100];
OS_STK WLANTaskStk[100];
OS_STK CC1101TaskStk[100];
OS_STK UartTaskStk[100];
OS_STK FlashTaskStk[100];
OS_STK LCDTaskStk[100];

#define  TASK_START_PRIO           			3   //ԽС  ���ȼ�Խ�� 
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


OS_EVENT *CC1101Rec_Semp;  //cc1101���ղ��ܴ��   ����Ҫ���ź���
INT8U err;

unsigned char WiFi_Status;

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];


unsigned char  DisplayStatus;   //������ʾ״̬
unsigned char  DataDisplayRefreshFlag =0; 

extern unsigned int ModelAddress;
extern unsigned char Heart_Beat_Flag;


int main(void)
{   
    OSInit(); 
    OSTaskCreate(taskStart, (void *)0, &GstkStart[99], TASK_START_PRIO);	 //ָ���ַ��������GstkStart
    OSStart(); 
}

void taskStart (void  *parg)
{
    (void)parg;

		OSTaskCreate ( 	taskwlan,//
										(void *)0, 
										&WLANTaskStk[99],     //ָ���ַ��������
										TASK_WLAN_PRIO);   	
		OSTaskCreate ( 	taskcc1101,//
										(void *)0, 
										&CC1101TaskStk[99], 
										TASK_CC1101_PRIO);  
		OSTaskCreate ( tasklcd,//
                   (void *)0, 
                   &LCDTaskStk[99], 
                   TASK_LCD_PRIO); 
//			OSTaskCreate ( taskuart,//
//                   (void *)0, 
//                   &TestTaskStk[99], 
//                   TASK_TEST_PRIO); 
//			OSTaskCreate ( taskflash,//
//                   (void *)0, 
//                   &FlashTaskStk[99], 
//                   TASK_FLASH_PRIO); 

    while (1) {   
		BSP_Init();			  //ϵͳʱ�ӳ�ʼ��    modify by  nevinxu 2014.2.8
		OSTaskSuspend(OS_PRIO_SELF);  
    }
}
/*********************************WIFI ������**********************************************/
void  taskwlan(void  *parg)
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
//		HeartBeatTransmit(CC3000Target);
		OSTimeDly(OS_TICKS_PER_SEC/2);
	}
}
/********************************************************************************************/



/*********************************CC1101������**********************************************/
static void taskcc1101(void *pdata)
{
	static unsigned CC1101TimeNum;  //��ʱʱ��
	unsigned char ReceiveAckFlag = 0;   //���ͽ���Ӧ��
	unsigned char ReceiveAckTimeOut;
	pdata = pdata; 
	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 ��ʼ��
	while(1)
	{
			CC1101TimeNum++;
			if(CC1101DataRecFlag&CC1101RECDATABIT)
			{
				
				OSSemPend(CC1101Rec_Semp,0,&err);//�����Ҫ��Ҫ���ź������� 
				CC1101ReceivePacket(CC1101RxBuf);   
				OSSemPost(CC1101Rec_Semp);
				
				DataDisplayRefreshFlag = 1;
				CC1101DateRecProcess();
				Heart_Beat_Flag = 0;
				ReceiveAckFlag = 0;
				CC1101DataRecFlag &=~ CC1101RECDATABIT;
			}
			if(CC1101TimeNum >= 100)   //10�������ն�����������
			{
				if(ReceiveAckFlag ==  0)
				{
					ReceiveAckFlag = 1;
					CC1101AddSet();
					ModelAddress++;
					if(ModelAddress>10)
					{
						ModelAddress = 0;
						CC1101TimeNum = 0;
					}
					CC1101DateSendProcess();	
				}	
				ReceiveAckTimeOut++;
				if(ReceiveAckTimeOut >= 2)  //200ms�ĳ�ʱ
				{
					ReceiveAckTimeOut = 0;
					ReceiveAckFlag = 0;
				}					
			}
			OSTimeDly(OS_TICKS_PER_SEC/10);    //��ʱʱ��Ϊ100ms
	}
}
/********************************************************************************************/




/**********************************FLASH������**********************************************/

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


/***********************************��ʾ������*********************************************/
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
/********************************************************************************************/