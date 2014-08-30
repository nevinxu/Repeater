#ifndef _DATAPROCESS_
#define _DATAPROCESS_

#define SOI   0x24
#define EOI   0x23

#define TerminalLoginCommand  0x01
#define TerminalLogin_AckCommand  0x02
#define TerminalLogoutCommand  0x03
#define TerminalLogoutAckCommand  0x04
#define HEARTBEATCommand  0x05
#define HEARTBEATAckCommand  0x06
#define TerminalWorkingStateCommand  0x07
#define TerminalWorkingStateAckCommand  0x08
#define TerminalWorkingStateReqCommand  0x09
#define TerminalLogin_ReqCommand  0x0a
#define TerminalWOR_AckCommand  0x0b
#define TerminalWOR_ReqCommand  0x0c

#define	MSGBeginByte							0
#define	TotalLengthByte						2
#define	ModelAddressByte					4
#define	CommandIdByte							6
#define	StatusByte								7
#define	SequenceIdByte						8

#define	TerminalIDByte						12


#define	ProtocolVersionByte				20
#define	HardwareVersionByte				22

#define	CurrentSpeedByte					20
#define	MaxSpeedLimitedByte						21
#define	MinSpeedLimitedByte						22
#define	TerminalPowerByte					23
#define	TotalDripByte							24


#define CC1101Target							0x01
#define CC3000Target							0x02

#define CLIENTNUM    6          // 终端个数


typedef struct _MSG_HEAD
{
 unsigned short m_MSGBegin;//消息识别符,固定用0xA5B4填充
 unsigned short m_Total_Length;//total length
 unsigned short m_ModelAddress;//代表床号
 unsigned char m_Command_Id;// Command ID
 unsigned char  m_Status;//具体见协议定义
 unsigned int m_Sequence_Id;//msg typ;
 char m_Terminal_ID[8];//终端SN，预留，目前不用
 }MSG_HEAD;

typedef struct _Login_MSG_BODY
{
   unsigned short Protocol_version;//软件版本
   unsigned short Hardware_version;//硬件版本
}Login_MSG_BODY;


typedef struct _Terminal_Working_State_MSG_BODY
{
   unsigned char m_Current_Speed;//当前滴速
	 unsigned char m_Max_Speed_Limited;//最大滴速
	 unsigned char m_Min_Speed_Limited;//最小滴速
	 unsigned char m_Terminal_Power;//当前电量
	 unsigned short m_Total_Drip;//当前总滴数
}Terminal_Working_State_MSG_BODY;


typedef struct _HEART_BEAT_Ack_BODY
{
   unsigned char Last_Delay_Time;//上一次延迟时间,单位，秒
   unsigned char Lost_Packet_Sum;//丢包数，单位个
}HEART_BEAT_Ack_BODY;


extern void SendCommand(unsigned int Command, unsigned char *Data,unsigned int Datalength);
extern void SendRateData(unsigned int RateData);
extern void CC1101DateRecProcess(unsigned char ClientNum);
extern void CC1101DateSendProcess(unsigned char ClientNum);
extern void WorkingStateMsgTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void WorkingStateMsgAckTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void WorkingStateMsgReqTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void LoginTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void LoginAckTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void LoginReqTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void HeartBeatTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void WorAckTransmit(unsigned char ClientNum,unsigned char RecTarget);
extern void CC3000DataRecProcess(unsigned char ClientNum);
#endif
