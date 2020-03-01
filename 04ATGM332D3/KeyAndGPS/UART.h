#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define GPS_Buffer_Length 80
#define latitude_Length 11
#define longitude_Length 12

typedef struct SaveData 
{
	char GPS_Buffer[GPS_Buffer_Length];
	char isGetData;		//�Ƿ��ȡ��GPS����
	char isParseData;	//�Ƿ�������
	char latitude[latitude_Length];		//γ��
	char longitude[longitude_Length];		//����
	char isUsefull;		//��λ��Ϣ�Ƿ���Ч
	char isShowData;	//�Ѿ���ʾ��Ϣ
} xdata _SaveData;


#define gpsRxBufferLength  76
char idata gpsRxBuffer[gpsRxBufferLength];
unsigned char RX_Count = 0;
_SaveData Save_Data;

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
void Uart_End()					  			   
{
	REN  = 0;     //ȡ�����пڽ�������
	TR1  = 0;     //�رն�ʱ��1
	ES   = 0;     //�رմ��п��ж�
	EA   = 0;     //�ر�ȫ���жϿ���
}

void UartPrintf(unsigned char *p)	//�����ַ���
{	
 	while(*p)
 	{
	 	SBUF=*(p++);
		while(TI==0) {};
		TI=0;
	}   
}

void UartPrintASCII(unsigned char c)	//����һ���ַ�
{ 
    TI=0;   
    SBUF=c;   
    while(TI==0) {};   
    TI=0;   
}


void RECEIVE_DATA(void) interrupt 4 using 3	  		
{ 
	unsigned char temp = 0;
	char i = 0;
	ES=0;
	temp = SBUF;
	RI = 0;
	
	if(temp == '$')
	{
		RX_Count = 0;	
	}
		
	if(RX_Count <= 5)
	{
	   gpsRxBuffer[RX_Count++] = temp;
	}
	else if(gpsRxBuffer[0] == '$' &gpsRxBuffer[4] == 'M' && gpsRxBuffer[5] == 'C')	//ȷ���Ƿ��յ�"GPRMC/GNRMC"��һ֡����
	{
		gpsRxBuffer[RX_Count++] = temp;
		if(temp == '\n')									   
		{
			memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);	//���
			memcpy(Save_Data.GPS_Buffer, gpsRxBuffer, RX_Count);	//��������
			Save_Data.isGetData = 1;
			RX_Count = 0;
			memset(gpsRxBuffer, 0, gpsRxBufferLength);	//���				
		}
		
		if(RX_Count >= 75)
		{
			RX_Count = 75;
			gpsRxBuffer[RX_Count] = '\0';//��ӽ�����
		}			
	}
	ES=1; 
}

void clrStruct()
{
	Save_Data.isGetData = 0;
	Save_Data.isParseData = 0;
	Save_Data.isUsefull = 0;
	Save_Data.isShowData = 0;
	memset(Save_Data.GPS_Buffer, 0, GPS_Buffer_Length);
	memset(Save_Data.latitude, 0, latitude_Length);
	memset(Save_Data.longitude, 0, longitude_Length);
}
void Delay_ms(unsigned int n)
{
	unsigned int  i,j;
	for(i=0;i<n;i++)
		for(j=0;j<123;j++);
}

void errorLog(int num)
{
	
	while (1)
	{
	  UartPrintf("ERROR");
		UartPrintASCII(num+0x30);
		UartPrintf("\r\n");
	}
}

#endif