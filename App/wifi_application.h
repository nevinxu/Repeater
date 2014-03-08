#ifndef WIFI_APPLICATION_H
#define WIFI_APPLICATION_H

#ifdef __cplusplus
extern "C"
{
#endif
#define WIFI_NON_CONNECTING          0
#define WIFI_CONNECTING              1
#define WIFI_CONNECT_FINISED         2
#define WIFI_SMARTCONFIG             3
#define WIFI_SMARTCONFIG_FINISED     4
#define WIFI_SEND_RECV               5
#define WIFI_NON_EVENT               6
	
	
//#define DEFAULT_OUT_OF_BOX_SSID       "caiyongfeng"
//#define AP_SECURITY                   "cai88619171"

#define DEFAULT_OUT_OF_BOX_SSID       "111111111"
#define AP_SECURITY                   "147huihui258fangfang369"	
	
	
#define DEVICE_LAN_PORT         5678            //默认端口号
#define DEVICE_LAN_IP        0x6601A8C0
//#define DEVICE_LAN_IP        0x0201A8C0

//#define DEVICE_LAN_PORT         5678            //默认端口号
//#define DEVICE_LAN_IP        0x6601A8C0


#define SERVER_LAN_PORT         5678            //默认端口号
#define SERVER_LAN_IP        0x6FA80166     //111
#define INADDR_ANY						0
	

  
#define WIFI_RX_BUF_MAX    1024
#define WIFI_TX_BUF_MAX    1024
  
#define in_range(c, lo, up)  ((unsigned char)c >= lo && (unsigned char)c <= up)

  
#define ip4_addr_set_u32(dest_ipaddr, src_u32) ((dest_ipaddr)->addr = (src_u32))  
	
#define TCPClient_Mode 1
#define TCPServer_Mode 2	
#define UDP_Mode 3	
  
extern volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,OkToDoShutDown, ulCC3000DHCP_configured;
extern volatile unsigned char ucStopSmartConfig;

extern volatile unsigned char ulWifiEvent;

extern unsigned short WIFIRxLen;
extern unsigned char WIFIRxBuf[WIFI_RX_BUF_MAX];

extern unsigned short WIFITxLen;
extern unsigned char WIFITxBuf[WIFI_TX_BUF_MAX];

extern char DeviceMac_Addr[6];

extern int Init_CC3000Driver(void);

extern unsigned char  GetdeviceInfo(void);

extern void Wifi_recv_data(void);

extern void Wifi_send_data(unsigned short Port,unsigned long IP,unsigned char *data,unsigned short Len);

extern unsigned char ConnectionAP(void);

extern void Wifi_event_handler(void);

extern void StartSmartConfig(void);

extern unsigned char ConnectionAP(void);
extern void  ReConnectSocket(unsigned long IP,unsigned short Port,unsigned char Mode);
extern void Init_Client(unsigned long* Socket,char * hname, int port);
extern void  Wifi_Scan();
   
#ifdef __cplusplus
}
#endif

#endif
