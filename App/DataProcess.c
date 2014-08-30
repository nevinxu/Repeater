#include "includes.h"

MSG_HEAD MsgHead[CLIENTNUM+1];   //ÏûÏ¢Í·
Terminal_Working_State_MSG_BODY MsgWorkStateBody[CLIENTNUM+1];  //Êý¾Ý×´Ì¬ ÏûÏ¢Ìå


unsigned char TxBuffer[64];
unsigned int MsgBegin = 0xA5B4;   //ÏûÏ¢Ê¶±ð·û
unsigned int MsgLength;  //ÏûÏ¢³¤¶È  `
long SequenceId = 0;   //ÏûÏ¢Á÷Ë®ºÅ

unsigned char CommandId[CLIENTNUM+1]; //ÃüÁî»òÏàÓ¦ÀàÐÍ
unsigned int ModelAddress[CLIENTNUM+1];  //´²Î»ºÅ
unsigned char MsgStatus[CLIENTNUM+1];  //ÏûÏ¢×´Ì¬
unsigned char TerminalID[CLIENTNUM+1][6];//Î¨Ò»±êÊ¶¸ÃÖÕ¶Ë
unsigned char TerminalStatus[CLIENTNUM+1] = {0,0,0,0,0,0,0};  //10¸öÖÕ¶ËµÇÂ½×´Ì¬
unsigned char WorEnableFlag[CLIENTNUM+1] = {0,0,0,0,0,0,0};



unsigned char LastTimeDelay;
unsigned char PackageLoseNum;

#define MessageHeaderLength  20

unsigned short ProtocolVersion[CLIENTNUM+1];
unsigned short HardwareVersion[CLIENTNUM+1];

#define Reversed                0x00

unsigned char CurrentSpeed[CLIENTNUM+1];
unsigned int TotalDrip[CLIENTNUM+1];
unsigned char TerminalPowerPrecent[CLIENTNUM+1];
unsigned char MaxSpeedLimited[CLIENTNUM+1];
unsigned char MinSpeedLimited[CLIENTNUM+1];

unsigned char WorkingStatus[CLIENTNUM+1];    //¹¤×÷×´Ì¬
unsigned char LoginStatus[CLIENTNUM+1];
unsigned char LogoutStatus[CLIENTNUM+1];
unsigned char HeartBeatStatus[CLIENTNUM+1];


unsigned char CC1101RxBuf[64];  //cc1101Êý¾Ý½ÓÊÕ»º´æ
unsigned char CC3000RxBuf[64];  //cc3000Êý¾Ý½ÓÊÕ»º´æ
unsigned char TxBuffer[64];
unsigned char CC3000Rxlen = 0;
unsigned char  CC1101DataRecFlag; //cc1101  Êý¾Ý½ÓÊÕ¸üÐÂ±êÖ¾

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

void WorAckTransmit(unsigned char ClientNum,unsigned char RecTarget)
{
	MsgHead[ClientNum].m_Status = LoginStatus[ClientNum];
  MsgHead[ClientNum].m_Total_Length = MessageHeaderLength;
  MsgHead[ClientNum].m_Command_Id = TerminalWOR_AckCommand ;
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
// 	TxBuffer[0] = 0x01;
// 	TxBuffer[1] = 0x02;
// 	TxBuffer[2] = 0x03;
// 	TxBuffer[3] = 0x04;
// 	TxBuffer[4] = 0x05;
// 	CC1101SendPacket(TxBuffer,1);
}


void CC1101DateRecProcess(unsigned char ClientNum)
{
	if(CC1101RxBuf[CommandIdByte] == TerminalWorkingStateCommand)   //¹¤×÷×´Ì¬½ÓÊÕ°ü
	{
		if(CC1101RxBuf[StatusByte] == 0) //Êý¾ÝÕý³£
		{
			if(ClientNum == CC1101RxBuf[ModelAddressByte])
			{
				ModelAddress[ClientNum] = CC1101RxBuf[ModelAddressByte];
				WorkingStatus[ClientNum] = CC1101RxBuf[StatusByte];
				CurrentSpeed[ClientNum] = CC1101RxBuf[CurrentSpeedByte];
				TotalDrip[ClientNum] = CC1101RxBuf[TotalDripByte]+ (CC1101RxBuf[TotalDripByte+1]<<8);
				TerminalPowerPrecent[ClientNum] = CC1101RxBuf[TerminalPowerByte];		
				//MaxSpeedLimited[ClientNum] = 30; 
				//MinSpeedLimited[ClientNum] = 140;
				MaxSpeedLimited[ClientNum] = CC1101RxBuf[MaxSpeedLimitedByte];
				MinSpeedLimited[ClientNum] = CC1101RxBuf[MinSpeedLimitedByte];
				WorkingStateMsgAckTransmit(ClientNum,CC1101Target);
				if(TerminalStatus[ClientNum] == 5)  //ÎªÊ²Ã´å
				TerminalStatus[ClientNum] = 6;
			}
		}
	}
	else if(CC1101RxBuf[CommandIdByte] == TerminalLoginCommand)  //µÇÂ½×´Ì¬½ÓÊÕ°ü
	{
		if(CC1101RxBuf[StatusByte] == 0)  //Êý¾ÝÕý³£
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
	else if(CC1101RxBuf[CommandIdByte] == TerminalWOR_ReqCommand)  //Wor ÇëÇó»ØÓ¦°ü½ÓÊÕ
	{
		if (ClientNum == CC1101RxBuf[ModelAddressByte])
		{
			WorEnableFlag[CurrentAddress] = 1;
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
		memset (CC3000RxBuf, 0, sizeof(CC3000RxBuf));     //Çå³ý
	}
}

	