#ifndef __GPS_H__
#define __GPS_H__

#include <reg52.h>
#include "UART.h"
#include "AT24C02.h"


void printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = 0;
		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = 0;
			UartPrintf("Save_Data.latitude = ");
			UartPrintf(Save_Data.latitude);
			UartPrintf("\r\n");

			UartPrintf("Save_Data.longitude = ");
			UartPrintf(Save_Data.longitude);
			UartPrintf("\r\n");
			
			Save_Data.isShowData=1;
		}
		else
		{
			UartPrintf("GPS DATA is not usefull!\r\n");
		}
	}
}

void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	char i = 0;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = 0;
		UartPrintf("**************\r\n");
		UartPrintf(Save_Data.GPS_Buffer);
		
		for (i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//��������
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						//case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//��ȡUTCʱ��
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//��ȡ״̬��
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//��ȡά��
						//case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//��ȡN/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//��ȡ����
						//case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//��ȡE/W
						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = 1;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = 1;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = 0;
				}
				else
				{
					errorLog(2);	//��������
				}
			}
		}
	}
}

#endif