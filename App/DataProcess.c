#include "includes.h"

extern unsigned long Deviceipaddr;   //自己ip
extern unsigned long Connectedipaddr;   //对方ip
extern unsigned short Server_Port;


unsigned char TxBuffer[64];
unsigned int MsgBegin = 0xA5B4;   //消息识别符
unsigned int MsgLength;  //消息长度
unsigned char CommandId; //命令或相应类型
unsigned int ModelAddress = 0x0001;  //床位号
long SequenceId = 0;   //消息流水号
unsigned char MsgStatus;  //消息状态
unsigned char Terminal_ID[6] = {0x00,0x00,0x00,0x00,0x00,0x01};//唯一标识该终端

#define MessageHeaderLength  18

unsigned short ProtocolVersion;
unsigned short HardwareVersion;
#define Reversed                0x00

unsigned char CurrentSpeed =0;
unsigned int TotalDrip =0;
unsigned char TerminalPowerPrecent =0;
unsigned char WorkingStatus = 0 ;    //工作状态
unsigned char LoginStatus = 0;
unsigned char LogoutStatus = 0;
unsigned char HeartBeatStatus = 0;


unsigned char CC1101RxBuf[64];  //cc1101数据接收缓存
unsigned char CC3000RxBuf[64];  //cc3000数据接收缓存
unsigned char TxBuffer[64];
unsigned char  CC1101DataRecFlag; //cc1101  数据接收更新标志


void MessageHeader()
{
  TxBuffer[0] = MsgBegin;
  TxBuffer[1] = MsgBegin>>8;
  TxBuffer[2] = MsgLength;
  TxBuffer[3] = MsgLength>>8;
  TxBuffer[4] = CommandId;
  TxBuffer[5] = ModelAddress;
  TxBuffer[6] = ModelAddress>>8;
  TxBuffer[7] = SequenceId;
  TxBuffer[8] = SequenceId>>8;
  TxBuffer[9] = SequenceId>>16;
  TxBuffer[10] = SequenceId>>24;       
  TxBuffer[11] = MsgStatus;      
  TxBuffer[12] = Terminal_ID[0];      
  TxBuffer[13] = Terminal_ID[1];        
  TxBuffer[14] = Terminal_ID[2];        
  TxBuffer[15] = Terminal_ID[3]; 
  TxBuffer[16] = Terminal_ID[4];        
  TxBuffer[17] = Terminal_ID[5];       
}

void WorkingStateMsgTransmit(unsigned char RecTarget)
{	
	MsgStatus = WorkingStatus;
  MsgLength = MessageHeaderLength+4;
  CommandId = TerminalWorkingStateCommand;
  MessageHeader();
	
  TxBuffer[18] = CurrentSpeed;
  TxBuffer[19] = TotalDrip;
  TxBuffer[20] = TotalDrip>>8;
  TxBuffer[21] = TerminalPowerPrecent;
   
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgLength); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgLength);
	}
}

void WorkingStateMsgAckTransmit(unsigned char RecTarget)
{
  MsgStatus = WorkingStatus;
  MsgLength = MessageHeaderLength;
  SequenceId++;
  CommandId = TerminalWorkingStateAckCommand;
  MessageHeader();
   
	
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgLength); 
	}
	else if(RecTarget == CC3000Target)
	{
		;
	}
}

void LoginTransmit(unsigned char RecTarget)
{
  MsgStatus = LoginStatus;
  MsgLength = MessageHeaderLength+6;
  CommandId = TerminalLoginCommand;
  MessageHeader();
  TxBuffer[18] = ProtocolVersion;
  TxBuffer[19] = ProtocolVersion>>8;
  TxBuffer[20] = HardwareVersion;
  TxBuffer[21] = HardwareVersion>>8;       
  TxBuffer[22] = Reversed;
  TxBuffer[23] = Reversed;
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgLength); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgLength);
	} 
}

void LoginAckTransmit(unsigned char RecTarget)
{
  MsgStatus = 0;
  MsgLength = MessageHeaderLength;
  SequenceId++;
  CommandId = TerminalLogin_AckCommand;
  MessageHeader();
	if(RecTarget == CC1101Target)
	{
		CC1101SendPacket( TxBuffer, MsgLength); 
	}
	else if(RecTarget == CC3000Target)
	{
		CC3000SendPacket( TxBuffer, MsgLength);
	} 
}

void CC1101DateRecProcess(void)
{
	if(CC1101RxBuf[CommandIdByte] == TerminalWorkingStateCommand)   //工作状态接收包
	{
		if(CC1101RxBuf[StatusByte] == 0) //数据正常
		{
			WorkingStatus = CC1101RxBuf[StatusByte];
			CurrentSpeed = CC1101RxBuf[CurrentSpeedByte];
			TotalDrip = CC1101RxBuf[TotalDripByte]+ (CC1101RxBuf[TotalDripByte+1]<<8);
			TerminalPowerPrecent = CC1101RxBuf[TerminalPowerByte];			
			WorkingStateMsgAckTransmit(CC1101Target);
			WorkingStateMsgTransmit(CC3000Target);
		}
	}
	else if(CC1101RxBuf[CommandIdByte] == TerminalLoginCommand)  //登陆状态接收包
	{
		if(CC1101RxBuf[StatusByte] == 0)  //数据正常
		{
			LoginStatus = CC1101RxBuf[StatusByte];
			ProtocolVersion = CC1101RxBuf[ProtocolVersionByte] + (CC1101RxBuf[ProtocolVersionByte+1]<<8);
			HardwareVersion = CC1101RxBuf[HardwareVersionByte] + (CC1101RxBuf[HardwareVersionByte+1]<<8);
			LoginAckTransmit(CC1101Target);
			LoginTransmit(CC3000Target);
		}
	}
}
