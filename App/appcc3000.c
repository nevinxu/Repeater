#include "includes.h"


/*********************************WIFI 任务函数**********************************************/
void  taskcc3000(void  *parg)
{
	(void)parg;
//	DisplayStatus = WifiStatusDisplayStatus;   //显示wifi连接状态
  Init_CC3000Driver();  		//初始化cc3000
	while(1)
	{
			Wifi_event_handler();  //wifi状态切换
//		CC3000DataRecProcess(CurrentAddress);    //接收没有对象之分
//		if(ulWifiEvent == WIFI_SEND_RECV)
//		{
//			OSSemPend(CC1101Rec_Semp,0,&err);//这里必要需要用信号量互斥 
//		//	LoginTransmit(CurrentAddress,CC3000Target);   //test
//				if(TerminalStatus[CurrentAddress] == 2)   //终端登陆
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
//					TerminalStatus[CurrentAddress] = 7;  //等待状态   cc1101发送数据  激活发送状态
//				}
//			OSSemPost(CC1101Rec_Semp);
//		}

		OSTimeDly(OS_TICKS_PER_SEC/50);
	}
}
/********************************************************************************************/




