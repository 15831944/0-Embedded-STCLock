#ifndef __UART_H__
#define __UART_H__

#include <string.h>
#define uchar unsigned char

uchar GPS_Buffer[]={0};
uchar GPS_Data[]={0};
uchar isGetData=0;		//�Ƿ��ȡ��GPS����
uchar beginGetData=0;	//��ʼ��ȡ����
uchar isRightData=0;	//�Ƿ���ȷ����
uchar isParseData=0;	//�Ƿ�������
uchar isUsefull=0;		//��λ��Ϣ�Ƿ���Ч
uchar RX_Count=0;
uchar GPS_Length=0; 

void Uart_Init()					  			   
{
	SCON = 0X50;  //UART��ʽ1��8λUART
	REN  = 1;     //�����пڽ�������
	PCON = 0x00;  //SMOD=0;�����ʲ��ӱ�
	TMOD = 0x20;  //T1��ʽ2�����ڲ���������
	TH1  = 0xFD;  //װ��ֵ
	TL1  = 0xFD;
	TR1  = 1;     //������ʱ��1
	EA   = 1;     //��ȫ���жϿ���
	ES   = 1;     //�򿪴��п��ж�	
}

void UartPrintf(uchar *p)	//�����ַ���
{	
 	while(*p)
 	{
	 	SBUF=*(p++);
		while(TI==0) {};
		TI=0;
	}   
}
void UartPrintASCII(uchar c)	//����һ���ַ�
{ 
    TI=0;   
    SBUF=c;   
    while(TI==0) {};   
    TI=0;   
}
void Clear_GPS()
{
	isGetData = 0;
	beginGetData = 0;
	isRightData = 0;
	isParseData = 0;
	isUsefull = 0;
	memset(GPS_Buffer, 0, 29);
}
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}

void Receive_Data(void) interrupt 4 using 3
{ 
	uchar GPS_Temp = 0;
	ES=0;
	GPS_Temp = SBUF;
	RI = 0;
	
	//UartPrintf("Receive...\r\n");
	if(GPS_Temp == '$' && !beginGetData)
	{
		beginGetData = 1;
		memset(GPS_Buffer, 0, 29);
		UartPrintf("beginGetData..\r\n");
	}
	if(GPS_Temp == 'M' && beginGetData)
	{
		isRightData=1;
		UartPrintf("isRightData..\r\n");
	}
	if(GPS_Temp == 'A' && isRightData)
	{
		UartPrintf("GetA..\r\n");
		RX_Count = 0;
		GPS_Buffer[RX_Count++] = GPS_Temp;
	}
	if(GPS_Buffer[0] == 'A')
	{
		//UartPrintf("Good..\r\n");
		GPS_Buffer[RX_Count] = GPS_Temp;
		if(GPS_Buffer[RX_Count] == 'E')									   
		{
			UartPrintf("----------GetE..\r\n");
			memset(GPS_Data, 0, RX_Count);
			memcpy(GPS_Data, GPS_Buffer, RX_Count);
			isGetData = 1;
			GPS_Buffer[RX_Count+1] = '\0';//��ӽ�����
			GPS_Length = RX_Count+1;
			memset(GPS_Buffer, 0, GPS_Length);
			RX_Count = 0;
			beginGetData = 0;
			isRightData = 0;
		}
		else
			//UartPrintf("NoE..\r\n");
		RX_Count++;
	}
	ES=1;
}

void Print_GPSData()
{
	if (isGetData)
	{
		isGetData = 0;
		UartPrintf("**************\r\n");
		UartPrintf(GPS_Data);
	}
	else
	{
		//UartPrintf("NoUse\r\n");
	}
}

#endif