#ifndef _DATAPROCESS_
#define _DATAPROCESS_


#define SOI   0x24
#define EOI   0x23

extern void SendCommand(unsigned int Command, unsigned char *Data,unsigned int Datalength);
extern void SendRateData(unsigned int RateData);
#endif
