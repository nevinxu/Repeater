#include "includes.h"


/*********************************WIFI ������**********************************************/
void  taskcc3000(void  *parg)
{
	(void)parg;
//	DisplayStatus = WifiStatusDisplayStatus;   //��ʾwifi����״̬
  Init_CC3000Driver();  		//��ʼ��cc3000
	while(1)
	{
			Wifi_event_handler();  //wifi״̬�л�
//		CC3000DataRecProcess(CurrentAddress);    //����û�ж���֮��
//		if(ulWifiEvent == WIFI_SEND_RECV)
//		{
//			OSSemPend(CC1101Rec_Semp,0,&err);//�����Ҫ��Ҫ���ź������� 
//		//	LoginTransmit(CurrentAddress,CC3000Target);   //test
//				if(TerminalStatus[CurrentAddress] == 2)   //�ն˵�½
//				{
//					TerminalStatus[CurrentAddress] = 3;
//					LoginTransmit(CurrentAddress,CC3000Target);
//					reconnectnum++;
//					if(reconnectnum >= 3)
//					{
//						reconnectnum = 0;
//						SetTcpConnect(TCPClientMode);
//					}
//				}
//				else if (TerminalStatus[CurrentAddress] == 6)
//				{
//					WorkingStateMsgTransmit(CurrentAddress,CC3000Target);
//					TerminalStatus[CurrentAddress] = 7;  //�ȴ�״̬   cc1101��������  �����״̬
//				}
//			OSSemPost(CC1101Rec_Semp);
//		}

		OSTimeDly(OS_TICKS_PER_SEC/50);
	}
}
/********************************************************************************************/




