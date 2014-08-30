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
static void 	taskuart(void *pdata);


OS_EVENT *CC1101Rec_Semp;  //cc1101���ղ��ܴ��   ����Ҫ���ź���
INT8U err;

unsigned char WiFi_Status;

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];


unsigned char  DisplayStatus;   //Һ������ʾģʽ
unsigned char  DataDisplayRefreshFlag =0; 

extern unsigned char TerminalStatus[CLIENTNUM];  //10���ն˵�½״̬
extern unsigned char Heart_Beat_Flag;

unsigned char CurrentAddress = 0;  //��ǰ��ַ
unsigned char reconnectnum;


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
		BSP_Init();			  //ϵͳʱ�ӳ�ʼ��    modify by  nevinxu 2014.2.8
		OSTaskSuspend(OS_PRIO_SELF);  
    }
}
/*********************************WIFI ������**********************************************/
void  taskwlan(void  *parg)
{

	(void)parg;
	DisplayStatus = WifiStatusDisplayStatus;   //��ʾwifi����״̬
  Init_CC3000Driver();  		//��ʼ��cc3000
	
	while(1)
	{
		Wifi_event_handler();  //wifi״̬�л�

		CC3000DataRecProcess(CurrentAddress);    //����û�ж���֮��
		if(ulWifiEvent == WIFI_SEND_RECV)
		{
			OSSemPend(CC1101Rec_Semp,0,&err);//�����Ҫ��Ҫ���ź������� 
		//	LoginTransmit(CurrentAddress,CC3000Target);   //test
				if(TerminalStatus[CurrentAddress] == 2)   //�ն˵�½
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
					TerminalStatus[CurrentAddress] = 7;  //�ȴ�״̬   cc1101��������  �����״̬
				}
			OSSemPost(CC1101Rec_Semp);
		}

		OSTimeDly(OS_TICKS_PER_SEC/50);
	}
}
/********************************************************************************************/



/*********************************CC1101������**********************************************/
static void taskcc1101(void *pdata)
{
	static unsigned  int CC1101TimeNum;  //��ʱʱ��
	static unsigned char LastAddress  = 0xff;
	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //��һ��״̬
//	unsigned char ReceiveAckFlag = 0;   //���ͽ���Ӧ��
	unsigned char ReceiveAckTimeOut = 0;
	pdata = pdata; 
	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 ��ʼ��
	
	while(1)
	{
			CC1101TimeNum++;
			OSSemPend(CC1101Rec_Semp,0,&err);//�����Ҫ��Ҫ���ź������� 
			if(CC1101TimeNum >= 100)   //2�������ն�����������
			{
//				CC1101TimeNum = 0;
				
/*************************************���ͺ���************************************************/			

				if(LastAddress != CurrentAddress)
				{
					CC1101AddSet(CurrentAddress);   //cc1101��ַ����
				}
				if(TerminalStatus[CurrentAddress] == 0)   //δ��½
				{
					LoginReqTransmit(CurrentAddress,CC1101Target);  //��½ѯ��
					TerminalStatus[CurrentAddress] = 1;
				}
				else if(TerminalStatus[CurrentAddress] == 4) //��½
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
/*************************************���պ���************************************************/				
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
				if(ReceiveAckTimeOut >= 10)  //200ms�ĳ�ʱ
				{
					ReceiveAckTimeOut = 0;
					if(TerminalStatus[CurrentAddress] < 4)   //�޷���½��һֱ���͵�½����
					{
						TerminalStatus[CurrentAddress] = 0;
					}
					else if(TerminalStatus[CurrentAddress] != 8)
					{
						TerminalStatus[CurrentAddress] = 4;
						ReSendCommandTimer[CurrentAddress]++;
						if(ReSendCommandTimer[CurrentAddress] >= 5)   //����5��
						{
							ReSendCommandTimer[CurrentAddress] = 0;
							TerminalStatus[CurrentAddress] = 0;  //�������µ�½����
						}
						
					}
					CurrentAddress++;   //��ַ���л�ʱ��̶�
					if(CurrentAddress>=CLIENTNUM)
					{
						CurrentAddress = 0;
						CC1101TimeNum = 0;
					}
				}		
			}				
		OSSemPost(CC1101Rec_Semp);
		OSTimeDly(OS_TICKS_PER_SEC/50);    //��ʱʱ��Ϊ20ms
	}
}
/********************************************************************************************/



static void taskuart(void *pdata)
{
	pdata = pdata;

}


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
	static unsigned char LastDisplayStatus = 0;  //��ʾ״̬�л� 
	pdata = pdata;
	Initial_Lcd();
	DisplayClearAll();

	while(1)
	{
		OSTimeDly(OS_TICKS_PER_SEC);  //һ��ˢ��һ��
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