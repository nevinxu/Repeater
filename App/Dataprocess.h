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

#define	MSGBeginByte							0
#define	TotalLengthByte						2
#define	CommandIdByte							4
#define	ModelAddressByte					5
#define	SequenceIdByte						7
#define	StatusByte								11
#define	TerminalIDByte						12


#define	protocolversionByte				18

#define	CurrentSpeedByte					18
#define	TotalDripByte							19
#define	TerminalPowerByte					21


#define CC1101Target							0x01
#define CC3000Target							0x02
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define
//#define


extern void SendCommand(unsigned int Command, unsigned char *Data,unsigned int Datalength);
extern void SendRateData(unsigned int RateData);
extern void CC1101DateRecProcess(void);
#endif
