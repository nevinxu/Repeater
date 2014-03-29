#include "includes.h"

extern unsigned char TCP_Mode;
extern unsigned char WiFi_Status;

extern unsigned char CurrentSpeed;
extern unsigned int TotalDrip;
extern unsigned char TerminalPowerPrecent;
extern unsigned char WorkingStatus;    //工作状态


const unsigned char CurrentSpeedCode[] = "currentspeed: ";
const unsigned char TotalDripCode[] = "totaldrip: ";
const unsigned char TerminalPowerPrecentCode[] = "powerprecent: ";
const unsigned char WorkingStatusCode[] = "workingstatus: ";

const unsigned char  WiFiStatusCode[] = "wifi status: ";


void DisplayItoA(u8 x_start, u8 y_start, u16 data)
{
  unsigned char DisplayCode[6];
	unsigned char i,j;
	DisplayCode[0]=data/10000+'0';
  DisplayCode[1]=data%10000/1000+'0';
  DisplayCode[2]=data%1000/100+'0';
  DisplayCode[3]=data%100/10+'0';
  DisplayCode[4]=data%10+'0';
	DisplayCode[5]=0;
	if(data>=10000)
	{
		i = 0;
	}
	else if(data>=1000)
	{
		i = 1;
	}
	else if(data>=100)
	{
		i = 2;
	}
	else if(data>=10)
	{
		i = 3;
	}
	else
	{
		i=4;
	}
	while(i)
	{
		for(j=0;j<5;j++)
		{
			DisplayCode[j] = DisplayCode[j+1];
		}
		i--;
	}
	displayString(x_start, y_start,DisplayCode);
	
}
//void DisplayItoA(u8 x_start, u8 y_start, u16 data)
//{
//  unsigned char SS,S,H,T,A;
//	unsigned char i = 4;
//	SS=data/10000;
//  S=data%10000/1000;
//  H=data%1000/100;
//  T=data%100/10;
//  A=data%10;
//	
//	DisplayNum(x_start+20*i,y_start,A);
//  if(data >=10)
//  {
//		DisplayNum(x_start+20*(i-1),y_start,T);
//  }
//  else
//  {
//		DisplayNum(x_start+20*(i-1),y_start,0);
//  }
//  if(data >=100)
//  {
//		DisplayNum(x_start+20*(i-2),y_start,H);
//  }
//  else
//  {
//		DisplayNum(x_start+20*(i-2),y_start,0);
//  }
//  if(data >=1000)
//  {
//		DisplayNum(x_start+20*(i-3),y_start,S);
//  }
//  else
//  {
//		DisplayNum(x_start+20*(i-3),y_start,0);
//  }
//	if(data >=10000)
//  {
//		DisplayNum(x_start+20*(i-4),y_start,SS);
//  }
//  else
//  {
//		DisplayNum(x_start+20*(i-4),y_start,0);
//  }
//}
void Display_WiFi_Status_Code()
{
	displayString(0,0,WiFiStatusCode);
}

void Display_WiFi_Status_NoConnect()
{
	displayString(0,16,"no connect");
}

void Display_WiFi_Status_Connecting()
{
	displayString(0,16,"connecting");
}

void Display_Line_Clear(unsigned char Line)
{
	
	clear_screen(0,Line<<4,128,16,WHILE);
}

void Display_WiFi_Status_ReConnecting()
{
	displayString(0,16,"fail restart");
	Display_Line_Clear(2);
	Display_Line_Clear(3);
}



void Display_WiFi_Status_Connected()
{
	displayString(0,16,"connected");
}

void Display_WiFi_Router_Name()
{
	unsigned char *string2;
	string2=DEFAULT_OUT_OF_BOX_SSID;
	displayString(0,32,"router name:");
	displayString(0,48,string2);
}

void Display_Connect_Type()
{
	displayString(0,32,"socket type:");
	if(TCP_Mode == TCPClient_Mode)
	{
		displayString(0,48,"tcp client");
	}
	else if(TCP_Mode ==TCPServer_Mode)
	{
		displayString(0,48,"tcp server");
	}
}

void Display_WiFi_Status_Scanning()
{
	displayString(0,16,"scanning ...");
}

void Display_WiFi_Status_Scanfinish()
{
	displayString(0,16,"scan	finish");
	displayString(0,32,"result ssid:");
}

void Display_WiFi_TCP_Status()
{
	displayString(0,32,"tcp status:");
	if((WiFi_Status&0xf0)  == TCPSETTING)
	{
		displayString(0,48,"setting");
	}
	else if((WiFi_Status&0xf0)  == TCPOVER)
	{
		displayString(0,48,"set finish");
	}
	
}

void  WifiStatusDisplay()
{
	static unsigned char Display_Index = 0;
	Display_WiFi_Status_Code();
			if((WiFi_Status&0x0f) == NOCONNECT)
			{
				Display_WiFi_Status_NoConnect();
			}
			else if((WiFi_Status&0x0f)  == SCANNING)
			{
				Display_WiFi_Status_Scanning();
			}
			else if((WiFi_Status&0x0f)  == SCANOVER)
			{
				Display_WiFi_Status_Scanfinish();
			}
			else if ((WiFi_Status&0x0f)  == CONNECTING)
			{
				if((Display_Index%4)==0)
				{
					Display_WiFi_Status_Connecting();
				}
				else if((Display_Index%4)==2)
				{
					Display_Line_Clear(1);
				}
				Display_WiFi_Router_Name();
			}
			else if ((WiFi_Status&0x0f)  == CONNECTED)
			{
				Display_WiFi_Status_Connected();
				if((Display_Index%15)==0)
				{
					Display_Connect_Type();
				}
				else if((Display_Index%15)==5)
				{
					Display_WiFi_Router_Name();
				}
				else if((Display_Index%15)==10)
				{
					Display_WiFi_TCP_Status();
				}
				
			}
			else if ((WiFi_Status&0x0f)  == RECONNECTED)
			{
				Display_WiFi_Status_ReConnecting();
			}
			Display_Index++;
}


void DataDisplayCode(void)
{
	displayString(0,0,CurrentSpeedCode);
	displayString(0,16,TotalDripCode);
	displayString(0,32,TerminalPowerPrecentCode);
	displayString(0,48,WorkingStatusCode);
}

void DataDisplay(void)
{
	static unsigned char LastCurrentSpeed;
	static unsigned int LastTotalDrip;
	static unsigned char LastTerminalPowerPrecent;
	static unsigned char LastWorkingStatus;    //工作状态 
	static unsigned char Display_Index;
	if((Display_Index%8)<=5)
	{
		if((Display_Index%8) == 0)
		{
			displayString(0,0,CurrentSpeedCode);
			displayString(0,32,TotalDripCode);
		}
		DisplayItoA(0,16,CurrentSpeed);
		DisplayItoA(0,48,TotalDrip);
	}
	else if((Display_Index%8)>5)
	{
		if((Display_Index%8) == 6)
		{
		displayString(0,0,TerminalPowerPrecentCode);
		displayString(0,32,WorkingStatusCode);
		}
		DisplayItoA(0,16,TerminalPowerPrecent);
		DisplayItoA(0,48,WorkingStatus);
	}
	Display_Index++;
}

