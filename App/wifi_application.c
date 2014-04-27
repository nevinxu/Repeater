#include "includes.h"
#include "wifi_application.h"
#include "wlan.h"
#include "evnt_handler.h"
#include "nvmem.h"
#include "socket.h"
#include "cc3000_common.h"
#include "netapp.h"
#include "cc3000spi.h"
#include "hci.h"
#include "security.h"
#include <string.h>

typedef struct   {
  unsigned long addr;
}ip_addr;


#define CC3000_APP_BUFFER_SIZE                      (32)
#define CC3000_RX_BUFFER_OVERHEAD_SIZE              (20)
#define DISABLE                                     (0)
#define ENABLE                                      (1)
#define SL_VERSION_LENGTH                           (11)

#define NETAPP_IPCONFIG_MAC_OFFSET		    					(20)



extern unsigned char  CC1101DataRecFlag;


unsigned long Deviceipaddr;   //自己ip
unsigned long Connectedipaddr;   //对方ip
unsigned short Server_Port =  5678;
tNetappIpconfigRetArgs Ipconfig;

unsigned char result_ssid[32];  //搜索获取到的ssid值


//unsigned char TCP_Mode = TCPServer_Mode;
unsigned char TCP_Mode = TCPClient_Mode;
 

extern unsigned char WiFi_Status;

extern unsigned short Relay_Flag;
volatile unsigned short Relay_Cty_Flag = 0;

volatile unsigned long ulSmartConfigFinished, ulCC3000Connected, ulCC3000DHCP,OkToDoShutDown, 
                       ulCC3000DHCP_configured;
volatile unsigned char ucStopSmartConfig;

volatile long ulSocket;//Socket 句柄
volatile long ulTcpSocket;//Socket 句柄

volatile unsigned char ulWifiEvent = WIFI_NON_EVENT;
volatile unsigned char ulCC3000Resetflags = 0;
volatile unsigned short Rxlen = 0;
// Simple Config Prefix
char aucCC3000_prefix[] = {'T', 'T', 'T'};

//device name used by smart config response
char device_name[] = "CC3000";

//AES key "smartconfigAES16"
const unsigned char smartconfigkey[] ={ 0x73, 0x6d, 0x61, 0x72, 0x74, 0x63, 0x6f, 0x6e, 0x66, 0x69, 0x67, 0x41, 0x45,0x53, 0x31, 0x36 };

unsigned short WIFIRxLen = 0;
unsigned char WIFIRxBuf[WIFI_RX_BUF_MAX];

unsigned short WIFITxLen = 0;
unsigned char WIFITxBuf[WIFI_TX_BUF_MAX];

char DeviceMac_Addr[6];



/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//#pragma is used for determine the memory location for a specific variable.                            ///
//__no_init is used to prevent the buffer initialization in order to prevent hardware WDT expiration    ///
// before entering to 'main()'.                                                                         ///
//for every IDE, different syntax exists :          1.   __CCS__ for CCS v5                             ///
//                                                  2.  __IAR_SYSTEMS_ICC__ for IAR Embedded Workbench  ///
// *CCS does not initialize variables - therefore, __no_init is not needed.                             ///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// Reception from the air, buffer - the max data length  + headers
//
#ifdef __IAR_SYSTEMS_ICC__
__no_init unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];
#else
unsigned char pucCC3000_Rx_Buffer[CC3000_APP_BUFFER_SIZE + CC3000_RX_BUFFER_OVERHEAD_SIZE];
#endif


volatile unsigned long EventTimeOut[EVENT_MAX_COUNT] = {0,0,0,0,0,0,0,0};
volatile unsigned char SysEvent = 0;

void SetEventTimeOut(unsigned char event,unsigned long  TimeOut)
{
  SysEvent &= ~event;
  if(TimeOut)
  {
   switch(event)
   {
     case 0x01:EventTimeOut[0] = TimeOut;break;
     case 0x02:EventTimeOut[1] = TimeOut;break;
     case 0x04:EventTimeOut[2] = TimeOut;break;
     case 0x08:EventTimeOut[3] = TimeOut;break;
     case 0x10:EventTimeOut[4] = TimeOut;break;
     case 0x20:EventTimeOut[5] = TimeOut;break;
     case 0x40:EventTimeOut[6] = TimeOut;break;
     case 0x80:EventTimeOut[7] = TimeOut;break;
     default:break;
   }
  }
}

void SetEvent(unsigned char  event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent |= event;
}

void ClearEvent(unsigned char event)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(event&(0x01<<i))
    {
      EventTimeOut[i] = 0;
    }
  }
  SysEvent &= ~event;
}

void GetEvent(void)
{
  unsigned char i;
  for(i=0;i<EVENT_MAX_COUNT;i++)
  {
    if(EventTimeOut[i])
    {
     if(--EventTimeOut[i]==0)
     {
       SysEvent |= 0x01<<i;
     }
    }
  }
}


//*****************************************************************************
//
//! sendDriverPatch
//!
//! @param  Length   pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch: since there is
//!				  no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendDriverPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendBootLoaderPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the bootloader patch: since there
//!				  is no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendBootLoaderPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! sendWLFWPatch
//!
//! @param  pointer to the length
//!
//! @return none
//!
//! @brief  The function returns a pointer to the driver patch: since there is
//!				  no patch (patches are taken from the EEPROM and not from the host
//!         - it returns NULL
//
//*****************************************************************************
char *sendWLFWPatch(unsigned long *Length)
{
  *Length = 0;
  return NULL;
}

//*****************************************************************************
//
//! CC3000_UsynchCallback
//!
//! @param  lEventType   Event type
//! @param  data
//! @param  length
//!
//! @return none
//!
//! @brief  The function handles asynchronous events that come from CC3000
//!		      device and operates a LED1 to have an on-board indication
//
//*****************************************************************************
void CC3000_UsynchCallback(long lEventType, char *data, unsigned char length)
{
  switch(lEventType)
  {
  case HCI_EVNT_WLAN_UNSOL_CONNECT:
    {
      ulCC3000Connected = 1;
    }break;
  case HCI_EVNT_WLAN_UNSOL_DISCONNECT:
    {
      if(ulWifiEvent == WIFI_SEND_RECV)
      {
        ulCC3000Connected = 0;
        ulCC3000DHCP = 0;
        ulCC3000DHCP_configured = 0;
        ulWifiEvent = WIFI_CONNECTING;
        ulCC3000Resetflags = 1;
      }
    }break;
  case HCI_EVNT_WLAN_ASYNC_SIMPLE_CONFIG_DONE:
    {
      ulSmartConfigFinished = 1;
      ucStopSmartConfig = 1;
    }break;
  case HCI_EVNT_WLAN_UNSOL_INIT: break;
  case HCI_EVNT_WLAN_UNSOL_DHCP:
    {
      if ( *(data + NETAPP_IPCONFIG_MAC_OFFSET) == 0)
      {
        ulCC3000DHCP = 1;
      }
      else
      {
        ulCC3000DHCP = 0;
      }
    }break;
  case HCI_EVNT_WLAN_ASYNC_PING_REPORT:break;
  case HCI_EVNT_WLAN_KEEPALIVE:break;
  case HCI_EVNT_WLAN_TX_COMPLETE:break;
  case HCI_EVENT_CC3000_CAN_SHUT_DOWN:
    {
      OkToDoShutDown = 1;
    }break;
  default:break;
  }
}


void Set_staticIP(unsigned long ip,unsigned char DHCP)
{
  unsigned long pucIP_Addr;
  unsigned long pucIP_DefaultGWAddr;
  unsigned long pucSubnetMask;
  unsigned long pucDNS;
  
  if(DHCP)
  {
    pucIP_Addr = 0;
    pucIP_DefaultGWAddr = 0;
    pucSubnetMask = 0;
    pucDNS = 0;   
  }
  else
  {
    pucIP_Addr = ip;
    pucIP_DefaultGWAddr = (ip&0x00ffffff) + 0x01000000;
    pucSubnetMask = 0x00ffffff;
    pucDNS = 0;
  }
  
  netapp_dhcp(&pucIP_Addr, &pucSubnetMask, &pucIP_DefaultGWAddr, &pucDNS);
}

//*****************************************************************************
//
//! initDriver
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function initializes a CC3000 device and triggers it to start
//!          operation
//
//*****************************************************************************
int Init_CC3000Driver(void)
{
  volatile int i;
  
  ucStopSmartConfig = 0;
  ulSmartConfigFinished = 0;
  ulCC3000Connected = 0;
  ulCC3000DHCP = 0;
  OkToDoShutDown = 0;
  ulCC3000DHCP_configured = 0;
  // Init Spi
  init_spi();
  
  // WLAN On API Implementation
  wlan_init(CC3000_UsynchCallback, sendWLFWPatch, sendDriverPatch,
            sendBootLoaderPatch, ReadWlanInterruptPin, WlanInterruptEnable,
            WlanInterruptDisable, WriteWlanPin);
  
  // Trigger a WLAN device
  wlan_start(0);
  
  // Mask out all non-required events from CC3000
  wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
  
  Set_staticIP(0,TRUE);  //自动获取ip
 
  ulWifiEvent = WIFI_CONNECTING;
  
  return (0);
  
}

//设置Socket网络服务
void Set_ulSocket(unsigned char Mode)
{
	unsigned char flag = SOCK_OFF;
	unsigned long aucDHCP=14400, aucARP=3600, aucKeepalive=10, timeout = 4;
	if(Mode == UDP_Mode)
	{
		ulSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); 
	}
	else
	{
		ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
	}
  delay_ms(100); 
  setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_RECV_TIMEOUT, &timeout, 4);
	setsockopt(ulSocket, SOL_SOCKET, SOCKOPT_ACCEPT_NONBLOCK, &flag, 2);
	

//  setsockopt(ulSocket,SOL_SOCKET,SOCKOPT_RECV_TIMEOUT,&timeout,sizeof(timeout));
  setsockopt(ulSocket,SOL_SOCKET,SOCKOPT_RECV_NONBLOCK,&timeout,sizeof(timeout));

////  timeout = 300;//5 minutes
//	timeout = 10;//5 minutes
//  netapp_timeout_values(&aucDHCP, &aucARP, &aucKeepalive, &timeout);

}

//绑定端口号
unsigned char Set_TCP(unsigned long IP,unsigned short Port,unsigned char Mode)
{
  sockaddr tSocketAddr, tSocketRecAddr;
	socklen_t tSockRecLen;
	signed long ret;
  
  tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = MSB(Port);
  tSocketAddr.sa_data[1] = LSB(Port);
  
  tSocketAddr.sa_data[2] = IP & 0xff;
  tSocketAddr.sa_data[3] = (IP & 0xff00) >> 8;
  tSocketAddr.sa_data[4] = (IP & 0xff0000) >> 16;
  tSocketAddr.sa_data[5] = IP >> 24;
  
  
	if(Mode == TCPServer_Mode)	
	{
		ret = bind(ulSocket, &tSocketAddr, sizeof(sockaddr));
		ret = listen(ulSocket, 100);
		ulTcpSocket = accept(ulSocket, &tSocketRecAddr, &tSockRecLen);
		if(ret>0)  //获取有效句柄
		{
		Connectedipaddr = (tSocketRecAddr.sa_data[2]<<24) + (tSocketRecAddr.sa_data[3]<<16) + (tSocketRecAddr.sa_data[4]<<8) + tSocketRecAddr.sa_data[5];
		}
		
	}	
	else if(Mode == TCPClient_Mode)
	{
		while(connect(ulSocket, &tSocketAddr, sizeof(sockaddr))== -1)
		{
			closesocket(ulSocket);
			ulSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
			delay_ms(100);
	//		return -1;
		}
	}
	return 0;
}


//重新连接 SOCKET 
void  ReConnectSocket(unsigned long IP,unsigned short Port,unsigned char Mode)
{
	
	sockaddr tSocketAddr, tSocketRecAddr;
	socklen_t tSockRecLen;
	signed long ret;
  
  tSocketAddr.sa_family = AF_INET;
  
  // the source port
  tSocketAddr.sa_data[0] = MSB(Port);
  tSocketAddr.sa_data[1] = LSB(Port);
  
	tSocketAddr.sa_data[2] = IP & 0xff;
	tSocketAddr.sa_data[3] = (IP & 0xff00) >> 8;
	tSocketAddr.sa_data[4] = (IP & 0xff0000) >> 16;
	tSocketAddr.sa_data[5] = IP >> 24;
	if(Mode == TCPClient_Mode)
	{
			closesocket(ulSocket);
			Set_ulSocket(Mode);
			while(connect(ulSocket, &tSocketAddr, sizeof(sockaddr))== -1)
			{
				closesocket(ulSocket);
				Set_ulSocket(Mode);
			}
	}
}

static int
ipaddr_aton(const char *cp, ip_addr *addr)
{
    unsigned long val;
    unsigned char base;
    char c;
    unsigned long parts[4];
    unsigned long *pp = parts;

    c = *cp;
    for (;;) {
        if (!isdigit(c))
            return (0);
        val = 0;
        base = 10;
        if (c == '0') {
            c = *++cp;
            if (c == 'x' || c == 'X') {
                base = 16;
                c = *++cp;
            } else
                base = 8;
        }
        for (;;) {
            if (isdigit(c)) {
                val = (val * base) + (int)(c - '0');
                c = *++cp;
            } else if (base == 16 && isxdigit(c)) {
                val = (val << 4) | (int)(c + 10 - (islower(c) ? 'a' : 'A'));
                c = *++cp;
            } else
                break;
        }
        if (c == '.') {
            if (pp >= parts + 3) {
                return (0);
            }
            *pp++ = val;
            c = *++cp;
        } else
            break;
    }
    if (c != '\0' && !isspace(c)) {
        return (0);
    }
    switch (pp - parts + 1) {

    case 0:
        return (0);

    case 1:        
        break;

    case 2:        
        if (val > 0xffffffUL) {
            return (0);
        }
        val |= parts[0] << 24;
        break;

    case 3:        
        if (val > 0xffff) {
            return (0);
        }
        val |= (parts[0] << 24) | (parts[1] << 16);
        break;

    case 4:        
        if (val > 0xff) {
            return (0);
        }
        val |= (parts[0] << 24) | (parts[1] << 16) | (parts[2] << 8);
        break;
    default:
        break;
    }
    if (addr) {
        ip4_addr_set_u32(addr, htonl(val));
    }
    return (1);
}

char initClient(unsigned long* Socket,sockaddr* tSocketAddr, char * hname, int port)
{
    ip_addr ipaddr;
    unsigned char *ptr=&(tSocketAddr->sa_data[2]);
	
    if(ipaddr_aton(hname,&ipaddr)<1)
   	return 1;
   	
    *Socket=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*Socket == -1)
    {
        wlan_stop();
        return 1;
    }
    tSocketAddr->sa_family = 2;
    tSocketAddr->sa_data[0] = (port & 0xFF00) >> 8;
    tSocketAddr->sa_data[1] = (port & 0x00FF);
    ptr=UINT32_TO_STREAM(ptr,ipaddr.addr);
    if (connect(*Socket, tSocketAddr, sizeof(tSocketAddr)) < 0)
    {
        return 1;
    }
    return 0;
}


void Init_Client(unsigned long *Socket,char * hname, int port)
{
  sockaddr tSocketAddr;
  initClient(Socket,&tSocketAddr,hname,port);
}

static void Resetcc3000(void)
{
  wlan_stop();
  delay_ms(1000); 
  wlan_start(0);
  // Mask out all non-required events
  wlan_set_event_mask(HCI_EVNT_WLAN_KEEPALIVE|HCI_EVNT_WLAN_UNSOL_INIT|HCI_EVNT_WLAN_ASYNC_PING_REPORT);
}

//*****************************************************************************
//
//! StartSmartConfig
//!
//!  @param  None
//!
//!  @return none
//!
//!  @brief  The function triggers a smart configuration process on CC3000.
//!			it exists upon completion of the process
//
//*****************************************************************************
void StartSmartConfig(void)
{
  ulSmartConfigFinished = 0; 
  ucStopSmartConfig = 0;
  ulCC3000Connected = 0;
  ulCC3000DHCP = 0;
  OkToDoShutDown=0;

  if(ulWifiEvent == WIFI_SMARTCONFIG)
  {   
    // Reset all the previous configuration
    wlan_ioctl_set_connection_policy(DISABLE, DISABLE, DISABLE);
      
#ifdef  DEBUG_WU 
    sprintf(uart_msg_str,(char *)"Delete profile\n");
    UartSendPacket(uart_msg_str, strlen(uart_msg_str));
#endif
      
    wlan_ioctl_del_profile(255);
    
    //Wait until CC3000 is disconnected
    while (ulCC3000Connected == 1)
    {
      delay_ms(1);
      hci_unsolicited_event_handler();
    }
      
    Resetcc3000();
      
      
    wlan_smart_config_set_prefix((char*)aucCC3000_prefix);
    // Start the SmartConfig start process
    wlan_smart_config_start(0);
    
    // Wait for Smart config to finish
    while (ulSmartConfigFinished == 0)
    {	
      delay_ms(5);   
    }    
    
    // create new entry for AES encryption key
    nvmem_create_entry(NVMEM_AES128_KEY_FILEID,16);
    
    // write AES key to NVMEM
    aes_write_key((unsigned char *)(&smartconfigkey[0]));
    
    // Decrypt configuration information and add profile
    wlan_smart_config_process();
        
    // Configure to connect automatically to the AP retrieved in the
    // Smart config process
    while(wlan_ioctl_set_connection_policy(DISABLE, DISABLE, ENABLE));
    
    delay_ms(50);
    
    // reset the CC3000
    Resetcc3000();
      
    ulWifiEvent = WIFI_CONNECTING;

    while((ulCC3000DHCP == 0)||(ulCC3000Connected ==0))
    {
        delay_ms(200); 
        hci_unsolicited_event_handler();  
    }
      
    if((ucStopSmartConfig == 1) && (ulCC3000DHCP == 1) && (ulCC3000Connected == 1))
    {
      unsigned char loop_index = 0;	
      while (loop_index < 5)
      {
        mdnsAdvertiser(1,device_name,strlen(device_name));
        loop_index++;
      }
      ucStopSmartConfig = 0;
    }
    
//    Set_ulSocket();
//    
//    Set_Port(DEVICE_LAN_IP,DEVICE_LAN_PORT);
    
    ulWifiEvent = WIFI_SMARTCONFIG_FINISED;
    
    GetdeviceInfo();
  }
}

unsigned char ConnectionAP(void)
{ 
	unsigned char ConnectedTimeout = 50;

  if(ulWifiEvent == WIFI_CONNECTING)
  {   
    while (((ulCC3000DHCP == 0) || (ulCC3000Connected == 0))&&(ConnectedTimeout > 0))
    {    
      delay_ms(200); 
      hci_unsolicited_event_handler();
      ConnectedTimeout--;
    } 
    if(ConnectedTimeout == 0)
    {
      if(ulCC3000DHCP == 0 || ulCC3000Connected == 0)
      {
        Resetcc3000();
        ulWifiEvent = WIFI_SMARTCONFIG;
        return 0;
      }
    }     
    GetdeviceInfo();  
  }
  return 1;
}

int TCPClient()
{
	unsigned ret;
	Set_ulSocket(TCPClient_Mode);    
	delay_ms(1000);       //nevinxu    必须要延时这么久!!!!!!
	ret = Set_TCP(DEVICE_LAN_IP,DEVICE_LAN_PORT,TCPClient_Mode);
	if(!ret)
	{
		ulWifiEvent = WIFI_CONNECT_FINISED;
	}
	else
	{
		WiFi_Status &=0xf0;  //清掉原来的
		WiFi_Status +=  RECONNECTED;
		Resetcc3000();
		delay_ms(1000);
		ulWifiEvent = WIFI_CONNECTING;
	}
}

int TCPServer()
{
	Set_ulSocket(TCPServer_Mode);    
	delay_ms(1000);     //nevinxu    必须要延时这么久!!!!!!
	Set_TCP(INADDR_ANY,SERVER_LAN_PORT,TCPServer_Mode);
	ulWifiEvent = WIFI_CONNECT_FINISED;
}

int ConnectUsingSSID(char * ssidName,unsigned char *ssidSecurity)
{   
    wlan_ioctl_set_connection_policy(0, 0, 0);
    wlan_disconnect();
    delay_ms(100); 
    wlan_connect(WLAN_SEC_WPA2, ssidName, strlen(ssidName), 
                 NULL, ssidSecurity,strlen((char*)ssidSecurity));   
		while(!ulCC3000Connected)
		{
			delay_ms(5); 
		}
		delay_ms(500);   //延时就要这么多  
		GetdeviceInfo();   //获取连接状态    ipconfig
		WiFi_Status &=0xf0;  //清掉原来的
		WiFi_Status +=  CONNECTED;

		WiFi_Status &=0x0f;  //清掉原来的
		WiFi_Status += TCPSETTING;
		if(TCP_Mode == TCPClient_Mode)
		{
			TCPClient();
		}
		else if(TCP_Mode == TCPServer_Mode)
		{
			TCPServer();
		}
		WiFi_Status &=0x0f;  //清掉原来的
		WiFi_Status +=  TCPOVER;
    return 0;      
}



unsigned char  GetdeviceInfo(void)
{

	netapp_ipconfig(&Ipconfig);
  
	Deviceipaddr = ((Ipconfig.aucIP[0]<<24)+(Ipconfig.aucIP[1]<<16)+(Ipconfig.aucIP[2]<<8)+Ipconfig.aucIP[3]);
//	if(ipaddr_aton(Ipconfig.aucIP,&ipaddr)<1)
//  	return 1;
//	else
//	return 0;
}

void  Wifi_Scan()
{
	long ret;
	unsigned char ScanResult[1000];
	unsigned long aiIntervalList[2];
	unsigned char i,j;
	long result_num;
	aiIntervalList[0] = 2000;
	aiIntervalList[1] = 2000;
	ret = wlan_ioctl_set_scan_params(1, 100,
													100,
													5,
													0x7ff,-120,
													0,
													205,
													aiIntervalList);
	delay_ms(5000);
	if(ret == 0)
	{
		ret = wlan_ioctl_get_scan_results(0,
                            ScanResult);
	}
	result_num = ScanResult[0]+(ScanResult[1]<<8)+(ScanResult[2]<<16)+(ScanResult[3]<<24);
	WiFi_Status &=0xf0;  //清掉原来的
	WiFi_Status +=  SCANOVER;
//	for(i=0;i<result_num;i++)
//	{
//		for(j = 0; j<32; j++)
//		{
//			result_ssid[j] = ScanResult[7 + 42*i +  18*(i+1) +  + j];
//			if((result_ssid[i]>='A')&&(result_ssid[i]<='Z'))
//			{
//				result_ssid[i] = result_ssid[i] - 32;
//			}
//		}
//		delay_ms(1000);
//	}
	delay_ms(5000);
}

void Wifi_send_data(unsigned short Port,unsigned long IP,unsigned char *data,unsigned short Len)
{
  sockaddr tSocketAddr_S;
  
  static unsigned char timesCnt = 0;
  
  if(ulWifiEvent == WIFI_SEND_RECV)
  {
    
    tSocketAddr_S.sa_family = AF_INET;
    
    // the source port
    tSocketAddr_S.sa_data[0] = MSB(Port);
    tSocketAddr_S.sa_data[1] = LSB(Port);
    
    tSocketAddr_S.sa_data[2] = IP & 0xff;
    tSocketAddr_S.sa_data[3] = (IP & 0xff00) >> 8;
    tSocketAddr_S.sa_data[4] = (IP & 0xff0000) >> 16;
    tSocketAddr_S.sa_data[5] = IP >> 24;
   
    if((WIFI_TX_BUF_MAX - WIFITxLen) >= Len)
    {
      memcpy(&WIFITxBuf[WIFITxLen],data,Len);
      WIFITxLen += Len; 
    }
    if (send(ulTcpSocket, WIFITxBuf, WIFITxLen, 0))
			
    {
      timesCnt = 0; 
    }
    else
    {
      if(timesCnt++ >= 10)
      {
        timesCnt = 0;
        ulWifiEvent = WIFI_NON_CONNECTING;
      }
    }
    WIFITxLen = 0;  
  }
  else
  {
    timesCnt = 0;
  }
}

void Wifi_recv_data(void)
{
  signed long iReturnValue;
  socklen_t tRxPacketLength;
  sockaddr tSocketAddr_R;
  
  if(ulWifiEvent == WIFI_SEND_RECV)
  {
    do
    {
	  iReturnValue = recv(ulSocket, pucCC3000_Rx_Buffer, 
							  CC3000_APP_BUFFER_SIZE, 0);


      if(iReturnValue > 0)
      {
        if((WIFI_RX_BUF_MAX - Rxlen) > iReturnValue)
        {
          memcpy(&WIFIRxBuf[Rxlen],pucCC3000_Rx_Buffer,iReturnValue);
          Rxlen += iReturnValue; 
        }

      }
      else
      {
        iReturnValue = 0;
      }
    }while(iReturnValue==CC3000_APP_BUFFER_SIZE);
  }
  
  return;
}


void Wifi_event_handler(void)
{
  if(ulWifiEvent != WIFI_SEND_RECV)
  {
    switch(ulWifiEvent)
    {
    case WIFI_NON_CONNECTING:
      {
        ClearEvent(ALL_EVENT_HANDLER);
        ulWifiEvent = WIFI_CONNECTING;
				WiFi_Status = NOCONNECT;
				WiFi_Status &=0xf0;  //清掉原来的
				WiFi_Status +=  SCANOVER;
      } break;
    case WIFI_CONNECTING:
      {
        ClearEvent(ALL_EVENT_HANDLER);
//        ConnectionAP();
			WiFi_Status &=0xf0;  //清掉原来的
				WiFi_Status +=  SCANNING;
//				Wifi_Scan();
				WiFi_Status &=0xf0;  //清掉原来的
				WiFi_Status +=  CONNECTING;
				ConnectUsingSSID(DEFAULT_OUT_OF_BOX_SSID,AP_SECURITY);
        ClearEvent(ALL_EVENT_HANDLER);
      }break;
    case WIFI_CONNECT_FINISED:
      {
        SetEvent(RECV_EVENT_HANDLER | SEND_EVENT_HANDLER |LED_EVENT_HANDLER);
//		OSSemPost(Rate_Semp);
        ulWifiEvent = WIFI_SEND_RECV;
      }break;
    case WIFI_SMARTCONFIG:
      {
        ClearEvent(ALL_EVENT_HANDLER);
        StartSmartConfig();
        ClearEvent(ALL_EVENT_HANDLER);
      }break;
    case WIFI_SMARTCONFIG_FINISED:
      {
        SetEvent(RECV_EVENT_HANDLER | SEND_EVENT_HANDLER | LED_EVENT_HANDLER);      
        ulWifiEvent = WIFI_SEND_RECV;
      }break;
    case WIFI_SEND_RECV:break;
    default: break;
    }
  }
  else 
  {
    Wifi_recv_data();
    return;
  }
}

void CC3000SendPacket(u8 *TxBuffer,u8 Size)
{
	
	Wifi_send_data(Server_Port,Connectedipaddr,TxBuffer,Size);
}


void Wifisend_Function()
{
	if(CC1101DataRecFlag&0x10)
	{
		
		WorkingStateMsgTransmit(CC3000Target);
		CC1101DataRecFlag &=~ 0x10;
	}
}

void Wifireceive_Function()
{
	static unsigned char Heart_Beat_Flag = 0;
	Wifi_event_handler();
//	if(SysEvent & RECV_EVENT_HANDLER)
//	{
//	  Heart_Beat_Flag++;
//      ClearEvent(RECV_EVENT_HANDLER);
//      if(Rxlen)
//      {
//        if(WIFIRxBuf[Rxlen-1]== '1')
//        {
//          Heart_Beat_Flag = 0;
//        } 
//        memset (WIFIRxBuf, 0, Rxlen);  
//        Rxlen = 0;
//      }
//		if(Heart_Beat_Flag >= 2)
//		{
//			Heart_Beat_Flag = 0;
//			ReConnectSocket(DEVICE_LAN_IP,DEVICE_LAN_PORT,TCPClient_Mode);				
//		}
//      SetEventTimeOut(RECV_EVENT_HANDLER,50);
//	}
}
