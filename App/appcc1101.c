#include "includes.h"

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];
extern unsigned char WorEnableFlag[CLIENTNUM];
extern unsigned char TerminalStatus[CLIENTNUM];  //10���ն˵�½״̬
unsigned char CurrentAddress = 1;  //��ǰ��ַ

/*********************************CC1101������**********************************************/
void taskcc1101(void *pdata)
{
	static unsigned  int CC1101TimeNum;  //��ʱʱ��
	static unsigned char LastAddress  = 0xff;
	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //��һ��״̬
	unsigned char ReceiveAckTimeOut = 0;
	unsigned char WorReqTimeOut = 0;
	pdata = pdata; 
//	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 ��ʼ��
	
	while(1)
	{
		if(ulWifiEvent == WIFI_SEND_RECV)
		{
		
			CC1101TimeNum++;
//			OSSemPend(CC1101Rec_Semp,0,&err);//�������Ҫ���ź������� 						
/*************************************���պ���************************************************/				
			if(CC1101DataRecFlag&CC1101RECDATABIT)
			{
						CC1101ReceivePacket(CC1101RxBuf);   				
						ReceiveAckTimeOut = 0;
						CC1101DateRecProcess(CurrentAddress);
						CC1101DataRecFlag &=~ CC1101RECDATABIT;
			}	
			else
			{
/************************************��ַѭ��********************************************/
			if(LastAddress != CurrentAddress)
			{
 				CC1101SYNCSet(CurrentAddress);   //cc1101��ַ����
			}
			LastAddress = CurrentAddress;
/*************************************���ͺ���************************************************/					
			if(WorEnableFlag[CurrentAddress] == 0)    //Wor δ����״̬  �����ķ��������
			{
					if(WorReqTimeOut>=20)  //1000ms��ʱ�л�ͨ��
					{
						WorReqTimeOut = 0;
						CurrentAddress++;
						if(CurrentAddress>CLIENTNUM)
						{
							CurrentAddress = 1;
						}
					}
					WorReqTimeOut++;
					WorAckTransmit(CurrentAddress,CC1101Target);
					//SetLEDStatus(LED1,TRUE);
					SetLEDStatus(LED1,FALSE);
			}
			
			else if (WorEnableFlag[CurrentAddress] == 1)  //��������������
			{
				{				
	/*************************************���ͺ���************************************************/			
					{
						if(TerminalStatus[CurrentAddress] == 0)   //δ��½
						{
							LoginReqTransmit(CurrentAddress,CC1101Target);  //��½ѯ��
							TerminalStatus[CurrentAddress] = 1;
							ReceiveAckTimeOut = 0;
						}
						else if(TerminalStatus[CurrentAddress] == 4) //��½
						{
							WorkingStateMsgReqTransmit(CurrentAddress,CC1101Target);
							TerminalStatus[CurrentAddress] = 5;
							ReceiveAckTimeOut = 0;
						}
					}
					ReceiveAckTimeOut++;
/********************************************************************************************/	
					if(ReceiveAckTimeOut >= 20)  //1000ms�ĳ�ʱ
					{
						ReceiveAckTimeOut = 0;
						
						WorReqTimeOut = 0;
						WorEnableFlag[CurrentAddress] = 0;
						
						
						if(TerminalStatus[CurrentAddress] < 4)   //�޷���½��һֱ���͵�½����
						{
							TerminalStatus[CurrentAddress] = 0;    //Ϊ��һ�εķ���׼����
						}
						else if(TerminalStatus[CurrentAddress] == 8)
						{
							TerminalStatus[CurrentAddress] = 4;
							ReSendCommandTimer[CurrentAddress] = 0;
						}
						else if(TerminalStatus[CurrentAddress] != 8)    
						{
							TerminalStatus[CurrentAddress] = 4;
							ReSendCommandTimer[CurrentAddress]++;
							CurrentAddress--;
							if(ReSendCommandTimer[CurrentAddress] >= 5)   //����5��
							{
								ReSendCommandTimer[CurrentAddress] = 0;
								TerminalStatus[CurrentAddress] = 0;  //�������µ�½����
							}				
						}
						
						CurrentAddress++;   //��ַ���л�ʱ��̶�							
						CC1101TimeNum = 0;
						if(CurrentAddress>CLIENTNUM)
						{
							CurrentAddress = 1;
						}
					}
				}
			}	
			else
			{
				WorEnableFlag[CurrentAddress] = 0;
			}
		}
	}
	//	OSSemPost(CC1101Rec_Semp);
		OSTimeDly(OS_TICKS_PER_SEC/20);    //��ʱʱ��Ϊ50ms, ��Ϊʱ�ӽ�����10msһ��
		//OSTimeDly(20);    
		
	}
}
/********************************************************************************************/
