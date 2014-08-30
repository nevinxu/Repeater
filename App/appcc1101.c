/*********************************CC1101任务函数**********************************************/
static void taskcc1101(void *pdata)
{
//	static unsigned  int CC1101TimeNum;  //计时时间
//	static unsigned char LastAddress  = 0xff;
//	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //上一次状态
////	unsigned char ReceiveAckFlag = 0;   //发送接收应答
//	unsigned char ReceiveAckTimeOut = 0;
//	pdata = pdata; 
//	CC1101Rec_Semp = OSSemCreate (1); 
//	CC1101Init();                                    //CC1101 初始化
//	
//	while(1)
//	{
//			CC1101TimeNum++;
//			OSSemPend(CC1101Rec_Semp,0,&err);//这里必要需要用信号量互斥 
//			if(CC1101TimeNum >= 100)   //2秒向发送终端请求发送数据
//			{
////				CC1101TimeNum = 0;
//				
///*************************************发送函数************************************************/			

//				if(LastAddress != CurrentAddress)
//				{
//					CC1101AddSet(CurrentAddress);   //cc1101地址设置
//				}
//				if(TerminalStatus[CurrentAddress] == 0)   //未登陆
//				{
//					LoginReqTransmit(CurrentAddress,CC1101Target);  //登陆询问
//					TerminalStatus[CurrentAddress] = 1;
//				}
//				else if(TerminalStatus[CurrentAddress] == 4) //登陆
//				{
//					WorkingStateMsgReqTransmit(CurrentAddress,CC1101Target);
//					TerminalStatus[CurrentAddress] = 5;
//				}
//				else if(TerminalStatus[CurrentAddress] == 8)
//				{
//					TerminalStatus[CurrentAddress] = 4;
//					ReSendCommandTimer[CurrentAddress] = 0;
//					ReceiveAckTimeOut = 10;
//				}
//				
//				LastAddress = CurrentAddress;
//				
//				ReceiveAckTimeOut++;
//				if(ReceiveAckTimeOut >= 3)
//				{
///*************************************接收函数************************************************/				
//					if(CC1101DataRecFlag&CC1101RECDATABIT)
//					{
//						CC1101ReceivePacket(CC1101RxBuf);   				
//						DataDisplayRefreshFlag = 1;
//						ReceiveAckTimeOut = 0;
//						CC1101DateRecProcess(CurrentAddress);
//						Heart_Beat_Flag = 0;
//						CC1101DataRecFlag &=~ CC1101RECDATABIT;
//					}
///********************************************************************************************/	
//				}
//				if(ReceiveAckTimeOut >= 10)  //200ms的超时
//				{
//					ReceiveAckTimeOut = 0;
//					if(TerminalStatus[CurrentAddress] < 4)   //无法登陆，一直发送登陆命令
//					{
//						TerminalStatus[CurrentAddress] = 0;
//					}
//					else if(TerminalStatus[CurrentAddress] != 8)
//					{
//						TerminalStatus[CurrentAddress] = 4;
//						ReSendCommandTimer[CurrentAddress]++;
//						if(ReSendCommandTimer[CurrentAddress] >= 5)   //重连5次
//						{
//							ReSendCommandTimer[CurrentAddress] = 0;
//							TerminalStatus[CurrentAddress] = 0;  //发送重新登陆命令
//						}
//						
//					}
//					CurrentAddress++;   //地址的切换时间固定
//					if(CurrentAddress>=CLIENTNUM)
//					{
//						CurrentAddress = 0;
//						CC1101TimeNum = 0;
//					}
//				}		
//			}				
//		OSSemPost(CC1101Rec_Semp);
//		OSTimeDly(OS_TICKS_PER_SEC/50);    //定时时间为20ms
//	}
}
/********************************************************************************************/
