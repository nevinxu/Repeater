/*********************************CC1101������**********************************************/
static void taskcc1101(void *pdata)
{
//	static unsigned  int CC1101TimeNum;  //��ʱʱ��
//	static unsigned char LastAddress  = 0xff;
//	unsigned char ReSendCommandTimer[CLIENTNUM+1];   //��һ��״̬
////	unsigned char ReceiveAckFlag = 0;   //���ͽ���Ӧ��
//	unsigned char ReceiveAckTimeOut = 0;
//	pdata = pdata; 
//	CC1101Rec_Semp = OSSemCreate (1); 
//	CC1101Init();                                    //CC1101 ��ʼ��
//	
//	while(1)
//	{
//			CC1101TimeNum++;
//			OSSemPend(CC1101Rec_Semp,0,&err);//�����Ҫ��Ҫ���ź������� 
//			if(CC1101TimeNum >= 100)   //2�������ն�����������
//			{
////				CC1101TimeNum = 0;
//				
///*************************************���ͺ���************************************************/			

//				if(LastAddress != CurrentAddress)
//				{
//					CC1101AddSet(CurrentAddress);   //cc1101��ַ����
//				}
//				if(TerminalStatus[CurrentAddress] == 0)   //δ��½
//				{
//					LoginReqTransmit(CurrentAddress,CC1101Target);  //��½ѯ��
//					TerminalStatus[CurrentAddress] = 1;
//				}
//				else if(TerminalStatus[CurrentAddress] == 4) //��½
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
///*************************************���պ���************************************************/				
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
//				if(ReceiveAckTimeOut >= 10)  //200ms�ĳ�ʱ
//				{
//					ReceiveAckTimeOut = 0;
//					if(TerminalStatus[CurrentAddress] < 4)   //�޷���½��һֱ���͵�½����
//					{
//						TerminalStatus[CurrentAddress] = 0;
//					}
//					else if(TerminalStatus[CurrentAddress] != 8)
//					{
//						TerminalStatus[CurrentAddress] = 4;
//						ReSendCommandTimer[CurrentAddress]++;
//						if(ReSendCommandTimer[CurrentAddress] >= 5)   //����5��
//						{
//							ReSendCommandTimer[CurrentAddress] = 0;
//							TerminalStatus[CurrentAddress] = 0;  //�������µ�½����
//						}
//						
//					}
//					CurrentAddress++;   //��ַ���л�ʱ��̶�
//					if(CurrentAddress>=CLIENTNUM)
//					{
//						CurrentAddress = 0;
//						CC1101TimeNum = 0;
//					}
//				}		
//			}				
//		OSSemPost(CC1101Rec_Semp);
//		OSTimeDly(OS_TICKS_PER_SEC/50);    //��ʱʱ��Ϊ20ms
//	}
}
/********************************************************************************************/
