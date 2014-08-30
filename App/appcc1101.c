#include "includes.h"

extern unsigned char  CC1101DataRecFlag;
extern unsigned char CC1101RxBuf[64];
extern unsigned char WorEnableFlag[CLIENTNUM];
extern unsigned char TerminalStatus[CLIENTNUM];  //10个终端登陆状态
unsigned char CurrentAddress = 1;  //当前地址

/*********************************CC1101任务函数**********************************************/
void taskcc1101(void *pdata)
{
	static unsigned  int CC1101TimeNum;  //计时时间
	static unsigned char LastAddress  = 0xff;
	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //上一次状态
	unsigned char ReceiveAckTimeOut = 0;
	unsigned char WorReqTimeOut = 0;
	pdata = pdata; 
//	CC1101Rec_Semp = OSSemCreate (1); 
	CC1101Init();                                    //CC1101 初始化
	
	while(1)
	{
		if(ulWifiEvent == WIFI_SEND_RECV)
		{
		
			CC1101TimeNum++;
//			OSSemPend(CC1101Rec_Semp,0,&err);//这里必需要用信号量互斥 						
/*************************************接收函数************************************************/				
			if(CC1101DataRecFlag&CC1101RECDATABIT)
			{
						CC1101ReceivePacket(CC1101RxBuf);   				
						ReceiveAckTimeOut = 0;
						CC1101DateRecProcess(CurrentAddress);
						CC1101DataRecFlag &=~ CC1101RECDATABIT;
			}	
			else
			{
/************************************地址循环********************************************/
			if(LastAddress != CurrentAddress)
			{
 				CC1101SYNCSet(CurrentAddress);   //cc1101地址设置
			}
			LastAddress = CurrentAddress;
/*************************************发送函数************************************************/					
			if(WorEnableFlag[CurrentAddress] == 0)    //Wor 未触发状态  连续的发送请求包
			{
					if(WorReqTimeOut>=20)  //1000ms超时切换通道
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
			
			else if (WorEnableFlag[CurrentAddress] == 1)  //发送正常的数据
			{
				{				
	/*************************************发送函数************************************************/			
					{
						if(TerminalStatus[CurrentAddress] == 0)   //未登陆
						{
							LoginReqTransmit(CurrentAddress,CC1101Target);  //登陆询问
							TerminalStatus[CurrentAddress] = 1;
							ReceiveAckTimeOut = 0;
						}
						else if(TerminalStatus[CurrentAddress] == 4) //登陆
						{
							WorkingStateMsgReqTransmit(CurrentAddress,CC1101Target);
							TerminalStatus[CurrentAddress] = 5;
							ReceiveAckTimeOut = 0;
						}
					}
					ReceiveAckTimeOut++;
/********************************************************************************************/	
					if(ReceiveAckTimeOut >= 20)  //1000ms的超时
					{
						ReceiveAckTimeOut = 0;
						
						WorReqTimeOut = 0;
						WorEnableFlag[CurrentAddress] = 0;
						
						
						if(TerminalStatus[CurrentAddress] < 4)   //无法登陆，一直发送登陆命令
						{
							TerminalStatus[CurrentAddress] = 0;    //为下一次的发送准备的
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
							if(ReSendCommandTimer[CurrentAddress] >= 5)   //重连5次
							{
								ReSendCommandTimer[CurrentAddress] = 0;
								TerminalStatus[CurrentAddress] = 0;  //发送重新登陆命令
							}				
						}
						
						CurrentAddress++;   //地址的切换时间固定							
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
		OSTimeDly(OS_TICKS_PER_SEC/20);    //定时时间为50ms, 因为时钟节拍是10ms一个
		//OSTimeDly(20);    
		
	}
}
/********************************************************************************************/
