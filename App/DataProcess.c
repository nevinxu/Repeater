#include "includes.h"

extern unsigned long Deviceipaddr;   //�Լ�ip
extern unsigned long Connectedipaddr;   //�Է�ip
extern unsigned short Server_Port;


unsigned char TxBuffer[64];
unsigned int MsgBegin = 0xA5B4;   //��Ϣʶ���
unsigned int MsgLength;  //��Ϣ����
unsigned char CommandId; //�������Ӧ����
unsigned int ModelAddress = 0x0001;  //��λ��
long Sequence_Id = 0;   //��Ϣ��ˮ��
unsigned char MsgStatus;  //��Ϣ״̬
unsigned char Terminal_ID[6] = {0x00,0x00,0x00,0x00,0x00,0x01};//Ψһ��ʶ���ն�

#define MessageHeaderLength  18

#define ProtocolVersion         0x0001
#define HardwareVersion         0x0001
#define Reversed                0x00

unsigned char CurrentSpeed =0;
unsigned int TotalDrip =0;
unsigned char TerminalPowerPrecent =0;
unsigned char WorkingStatus = 0 ;    //����״̬


unsigned char CC1101RxBuf[64];  //cc1101���ݽ��ջ���
unsigned char CC3000RxBuf[64];  //cc3000���ݽ��ջ���
unsigned char TxBuffer[64];
unsigned char  CC1101DataRecFlag; //cc1101  ���ݽ��ո��±�־


void MessageHeader()
{
  TxBuffer[0] = MsgBegin;
  TxBuffer[1] = MsgBegin>>8;
  TxBuffer[2] = MsgLength;
  TxBuffer[3] = MsgLength>>8;
  TxBuffer[4] = CommandId;
  TxBuffer[5] = ModelAddress;
  TxBuffer[6] = ModelAddress>>8;
  TxBuffer[7] = Sequence_Id;
  TxBuffer[8] = Sequence_Id>>8;
  TxBuffer[9] = Sequence_Id>>16;
  TxBuffer[10] = Sequence_Id>>24;       
  TxBuffer[11] = MsgStatus;      
  TxBuffer[12] = Terminal_ID[0];      
  TxBuffer[13] = Terminal_ID[1];        
  TxBuffer[14] = Terminal_ID[2];        
  TxBuffer[15] = Terminal_ID[3]; 
  TxBuffer[16] = Terminal_ID[4];        
  TxBuffer[17] = Terminal_ID[5];       
}

void WorkingStateMsgAckTransmit(unsigned char RecTarget)
{
  MsgStatus = WorkingStatus;
  MsgLength = MessageHeaderLength;
  Sequence_Id++;
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

void CC1101DateRecProcess(void)
{
	if(CC1101RxBuf[CommandIdByte] == TerminalWorkingStateCommand)
	{
		if(CC1101RxBuf[StatusByte] == 0)
		{
			WorkingStatus = CC1101RxBuf[StatusByte];
			CurrentSpeed = CC1101RxBuf[CurrentSpeedByte];
			TotalDrip = CC1101RxBuf[TotalDripByte]+ (CC1101RxBuf[TotalDripByte+1]<<8);
			TerminalPowerPrecent = CC1101RxBuf[TerminalPowerByte];			
			WorkingStateMsgAckTransmit(CC1101Target);
		}
	}
}
