#include "includes.h"

extern unsigned long Deviceipaddr;   //自己ip
extern unsigned long Connectedipaddr;   //对方ip
extern unsigned short Server_Port;

void SendCommand(unsigned int Command, unsigned char *Data,unsigned int Datalength)
{
  unsigned char  SendBuffer[200];
  unsigned long sum = 0;
  unsigned int Length;
  unsigned int i;
  static unsigned int SEQ = 0;  //SEQ字段数据长度固定为1 WORD，默认初始值从0x0000开始，每次成功发次一个包后加1。
  SendBuffer[0] = SOI;
  SEQ++;
  SendBuffer[3] = SEQ>>8;
  SendBuffer[4] = SEQ;
  sum += ((SEQ>>8) + (SEQ&0x00ff));
  SendBuffer[5] = Command>>8;
  SendBuffer[6] =  Command;
  sum += ( (Command>>8) + (Command&0x00ff));
  for(i = 0; i< Datalength; i++)
  {
    SendBuffer[7+i] = Data[i];
    sum += Data[i];
  }
  Length = 12 + i;
  sum += ((Length>>8)+ (Length&0x00ff));
  SendBuffer[1] = Length>>8;
  SendBuffer[2] = Length;
  SendBuffer[7+i] = sum>>24;
  SendBuffer[8+i] = sum>>16;
  SendBuffer[9+i] = sum>>8;
  SendBuffer[10+i] = sum;
  SendBuffer[11+i] = EOI;
  Wifi_send_data(Server_Port,Connectedipaddr,SendBuffer,Length);
}

void SendProtocolVersion(unsigned char Version_num)
{
  unsigned char Data[1];
  Data[0] = Version_num;
  SendCommand(0x0011,Data,1);
}

void SendSoftHardWareVersion(unsigned long HardWare,unsigned long SoftWare)
{
  unsigned char Data[7];
  Data[0] = HardWare;
  Data[1] = HardWare>>8;
  Data[2] = HardWare>>16;
  Data[3] = 0;
  Data[4] = HardWare;
  Data[5] = HardWare>>8;
  Data[6] = HardWare>>16;
  SendCommand(0x0013,Data,7);
}

void SendRateData(unsigned int RateData)
{
  unsigned char Data[2];
  Data[0] = RateData>>8;
  Data[1] = RateData;

  SendCommand(0x0054,Data,2);
}
