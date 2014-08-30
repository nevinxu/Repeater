#include 	"includes.h" 

OS_STK GstkStart[100];
OS_STK WLANTaskStk[100];
OS_STK UartTaskStk[100];

static void taskStart (void  *parg);

#define TASK_START_PRIO     3   //ԽС���ȼ�Խ��
#define TASK_WLAN_PRIO      5
#define TASK_UART_PRIO      4

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

		OSTaskCreate ( 	taskcc3000,//
										(void *)0, 
										&WLANTaskStk[99],     //ָ���ַ��������
										TASK_WLAN_PRIO);   	
//		OSTaskCreate ( 	taskcc1101,//
//										(void *)0, 
//										&CC1101TaskStk[99], 
//										TASK_CC1101_PRIO);  
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


