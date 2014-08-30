#include "includes.h"

MSG_HEAD MsgHead[CLIENTNUM];   //消息头
Terminal_Working_State_MSG_BODY MsgWorkStateBody[CLIENTNUM];  //数据状态 消息体


unsigned char TxBuffer[64];
unsigned int MsgBegin = 0xA5B4;   //消息识别符
unsigned int MsgLength;  //消息长度  `
long SequenceId = 0;   //消息流水号

unsigned char CommandId[CLIENTNUM]; //命令或相应类型
unsigned int ModelAddress[CLIENTNUM];  //床位号
unsigned char MsgStatus[CLIENTNUM];  //消息状态
unsigned char TerminalID[CLIENTNUM][6];//唯一标识该终端
unsigned char TerminalStatus[CLIENTNUM] = {0,0,0};  //10个终端登陆状态



unsigned char LastTimeDelay;
unsigned char PackageLoseNum;

#define MessageHeaderLength  12

unsigned short ProtocolVersion[CLIENTNUM];
unsigned short HardwareVersion[CLIENTNUM];

#define Reversed                0x00

unsigned char CurrentSpeed[CLIENTNUM];
unsigned int TotalDrip[CLIENTNUM];
unsigned char TerminalPowerPrecent[CLIENTNUM];
unsigned char MaxSpeedLimited[CLIENTNUM];
unsigned char MinSpeedLimited[CLIENTNUM];

unsigned char WorkingStatus[CLIENTNUM];    //工作状态
unsigned char LoginStatus[CLIENTNUM];
unsigned char LogoutStatus[CLIENTNUM];
unsigned char HeartBeatStatus[CLIENTNUM];


unsigned char CC1101RxBuf[64];  //cc1101数据接收缓存
unsigned char CC3000RxBuf[64];  //cc3000数据接收缓存
unsigned char TxBuffer[64];
unsigned char CC3000Rxlen = 0;
unsigned char  CC1101DataRecFlag; //cc1101  数据接收更新标志

extern unsigned char CurrentAddress;
extern unsigned char reconnectnum;;


void MessageHeader(unsigned char ClientNum)
{
	unsigned char i;
  MsgHead[ClientNum].m_MSGBegin = MsgBegin;
  MsgHead[ClientNum].m_ModelAddress = ModelAddress[ClientNum];
	for(i = 0;i<8;i++)
	{
		MsgHead[ClientNum].m_Terminal_ID[i] =  TerminalID[ClientNum][i];
	}
		memset(&TxBuffer,0,sizeof(TxBuffer));
		memcpy(&TxBuffer,&(MsgHead[ClientNum]),sizeof(MSG_HEAD));  
}

void WorkingStateMsgReqTransmit(unsigned char ClientNum, unsigned char RecTarget)
{	
	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateReqCommand;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
  MessageHeader(ClientNum);
   
	
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		;
	}
}

void WorkingStateMsgTransmit(unsigned char ClientNum, unsigned char RecTarget)
{	
	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength+sizeof(Terminal_Working_State_MSG_BODY);
  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateCommand;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
  MessageHeader(ClientNum);
	
  MsgWorkStateBody[ClientNum].m_Current_Speed = CurrentSpeed[ClientNum];
//	  MsgWorkStateBody[ClientNum].m_Current_Speed = 0x55;
  MsgWorkStateBody[ClientNum].m_Total_Drip = TotalDrip[ClientNum];
//	  MsgWorkStateBody[ClientNum].m_Total_Drip = 0x77;
	MsgWorkStateBody[ClientNum].m_Terminal_Power = TerminalPowerPrecent[ClientNum];
	MsgWorkStateBody[ClientNum].m_Max_Speed_Limited = MaxSpeedLimited[ClientNum];
	MsgWorkStateBody[ClientNum].m_Min_Speed_Limited = MinSpeedLimited[ClientNum];
	
	memset(&TxBuffer[0]+sizeof(MSG_HEAD),0,sizeof(Terminal_Working_State_MSG_BODY));
	memcpy(&TxBuffer[0]+sizeof(MSG_HEAD),&MsgWorkStateBody[ClientNum],sizeof(Terminal_Working_State_MSG_BODY));
   
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
	}
}


//void HeartBeatTransmit(unsigned char RecTarget)
//{
//  MsgStatus = HeartBeatStatus;
//  MsgLength = MessageHeaderLength+2;
////  SequenceId++;
//  CommandId = HEARTBEATCommand;
//  TxBuffer[18] = LastTimeDelay;
//  TxBuffer[19] = PackageLoseNum;
//  MessageHeader();
//	if(RecTarget == CC1101Target)
//	{
//		CC1101SendPacket( TxBuffer, MsgLength); 
//	}
//	else if(RecTarget == CC3000Target)
//	{
//		CC3000SendPacket( TxBuffer, MsgLength);
//	}
//}


void WorkingStateMsgAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{

	
	MsgHead[ClientNum].m_Status = WorkingStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
  MsgHead[ClientNum].m_Command_Id = TerminalWorkingStateAckCommand;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
  MessageHeader(ClientNum);
   
	
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		;
	}
}

void LoginReqTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
  MsgHead[ClientNum].m_Command_Id = TerminalLogin_ReqCommand ;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
  MessageHeader(ClientNum);
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
	} 
}


void LoginTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength+6;
  MsgHead[ClientNum].m_Command_Id = TerminalLoginCommand;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
  MessageHeader(ClientNum);
  TxBuffer[18] = ProtocolVersion[ClientNum];
  TxBuffer[19] = ProtocolVersion[ClientNum]>>8;
  TxBuffer[20] = HardwareVersion[ClientNum];
  TxBuffer[21] = HardwareVersion[ClientNum]>>8;       
  TxBuffer[22] = Reversed;
  TxBuffer[23] = Reversed;
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
	} 
}

void LoginAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
  MsgHead[ClientNum].m_Command_Id = TerminalLogin_AckCommand;
	MsgHead[ClientNum].m_Sequence_Id = SequenceId;
	SequenceId++;
	
  MessageHeader(ClientNum);
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgHead[ClientNum].m_Total_Length);
	} 
}

void CC1101DateRecProcess(unsigned char ClientNum)
{
	if(CC1101RxBuf[CommandIdByte] == TerminalWorkingStateCommand)   //工作状态接收包
	{
		if(CC1101RxBuf[StatusByte] == 0) //数据正常
		{
			if(ClientNum == CC1101RxBuf[ModelAddressByte])
			{
				ModelAddress[ClientNum] = CC1101RxBuf[ModelAddressByte];
				WorkingStatus[ClientNum] = CC1101RxBuf[StatusByte];
				CurrentSpeed[ClientNum] = CC1101RxBuf[CurrentSpeedByte];
				TotalDrip[ClientNum] = CC1101RxBuf[TotalDripByte]+ (CC1101RxBuf[TotalDripByte+1]<<8);
				TerminalPowerPrecent[ClientNum] = CC1101RxBuf[TerminalPowerByte];		
				MaxSpeedLimited[ClientNum] = 30;
				MinSpeedLimited[ClientNum] = 140;			
		//		WorkingStateMsgAckTransmit(ClientNum,CC1101Target);
				if(TerminalStatus[ClientNum] == 5)
				TerminalStatus[ClientNum] = 6;
			}
		}
	}
	else if(CC1101RxBuf[CommandIdByte] == TerminalLoginCommand)  //登陆状态接收包
	{
		if(CC1101RxBuf[StatusByte] == 0)  //数据正常
		{
			if (ClientNum == CC1101RxBuf[ModelAddressByte])
			{
				ModelAddress[ClientNum] = CC1101RxBuf[ModelAddressByte];
				ProtocolVersion[ClientNum] = CC1101RxBuf[ProtocolVersionByte] + (CC1101RxBuf[ProtocolVersionByte+1]<<8);
				HardwareVersion[ClientNum] = CC1101RxBuf[HardwareVersionByte] + (CC1101RxBuf[HardwareVersionByte+1]<<8);
				LoginAckTransmit(ClientNum,CC1101Target);
				if(TerminalStatus[ClientNum] == 1)
				TerminalStatus[ClientNum] = 2;
			}
		}
	}
}

void CC1101DateSendProcess(unsigned char ClientNum)
{
	WorkingStateMsgReqTransmit(ClientNum,CC1101Target);
}



void CC3000DataRecProcess(unsigned char ClientNum)
{
	if(CC3000Rxlen != 0)
	{
		memset(&MsgHead[ClientNum],0,sizeof(MsgHead));
		memcpy(&MsgHead[ClientNum],CC3000RxBuf,sizeof(MSG_HEAD));
		if(MsgHead[ClientNum].m_MSGBegin == 0xA5B4)
		{
			if(MsgHead[ClientNum].m_Command_Id == TerminalLogin_AckCommand) 
			{
				if(MsgHead[ClientNum].m_Status == 0)
				{
					TerminalStatus[ClientNum] = 4;  
	//				CurrentAddress++;
				}
						reconnectnum = 0;
			}
			else if(MsgHead[ClientNum].m_Command_Id == TerminalWorkingStateAckCommand) 
			{
				if(MsgHead[ClientNum].m_Status == 0)
				{
					TerminalStatus[ClientNum] = 8;  
//					CurrentAddress++;
				}
						reconnectnum = 0;
			}
		}
		memset (CC3000RxBuf, 0, sizeof(CC3000RxBuf));     //清除
	}
}

	