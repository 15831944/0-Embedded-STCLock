#ifndef _AT24C02_H_
#define _AT24C02_H_

#include <reg52.h>
#include <index.h>
#include <intrins.h>	//����_nop_();

sbit SCL = P1^6;			//24C02����ʱ��
sbit SDA = P1^7;			//24C02��������

/*-----�ղ���-----*/
void Nop()
{
 	_nop_();	//һ����Լ1us
 	_nop_();
 	_nop_();
 	_nop_();
}
/*-----��ʼ����-----*/
void Start()
{
 	SDA=1;
 	SCL=1;
 	Nop();
 	SDA=0;
 	Nop();
}
/*-----ֹͣ����-----*/
void Stop()
{
 	SDA=0;
 	SCL=1;
 	Nop();
 	SDA=1;
 	Nop();
}
/*-----Ӧ��λ-----*/
void Ack()
{
	SDA=0;
	Nop();
	SCL=1;
	Nop();
	SCL=0;
}
/*-----����Ӧ��λ-----*/
void DeAck()
{
 	SDA=1;
 	Nop();
 	SCL=1;
 	Nop();
 	SCL=0;
}

/*-----���������ӳ���DataΪҪ���͵�����-----*/
void Send(uchar Data_Send)
{
   	uchar BitCounter=8;
   	uchar Temp;
   	do
   	{
   		Temp=Data_Send;	//�������͵������ݴ�Temp
   		SCL=0;
   		Nop();
   		if((Temp&0x80)==0x80)	//������������&0x80
				SDA=1;
   		else 
				SDA=0;
   		SCL=1;
   		Temp=Data_Send<<1;	//��������
   		Data_Send=Temp;	//�����������¸�ֵData_Send
   		BitCounter--;	//����������0�����ݴ������
   	}
   	while(BitCounter);	//�ж��Ƿ������
   	SCL=0;
}
/*-----��һ�ֽڵ����ݣ��������ֽ�ֵ-----*/
uchar Read()
{
  uchar Data_Read=0;
	uchar Temp=0;
	uchar BitCounter=8;
	SDA=1;
	do
	{
		SCL=0;
		Nop();
		SCL=1;
		Nop();
		if(SDA)	//����λΪ1
			Data_Read=Data_Read|0x01;	//Data_Read�����λΪ1��|0x01�����ǽ����λ��Ϊ1��
		else	//����λΪ0
			Data_Read=Data_Read&0xfe;	//Data_Read���λΪ0��&0xfe 11111110 ���ǽ����λ��Ϊ0��
		if(BitCounter-1)	//BitCounter��1���Ƿ�Ϊ��
		{
			Temp=Data_Read<<1;	//Data_Read����
			Data_Read=Temp;
		}
		BitCounter--;	//BitCounter����0ʱ�����ݽ������
	}
	while(BitCounter);	//�ж��Ƿ�������
	return Data_Read;
}

/*-----��ROMд����-----*/
void Write_ROM(uchar Data_Write[],uchar Address,uchar Num)
{
  uchar i;
  uchar *PData;
  PData=Data_Write;
  for(i=0;i<Num;i++)
  {
		Start();
		Send(0xa0);
		Ack();
		Send(Address+i);
		Ack();
		Send(*(PData+i));
		Ack();
		Stop();
		DelayMs(20);
  }
}
/*-----��ROM������-----*/
void Read_ROM(uchar Data_Read[],uchar Address,uchar Num)
{
  uchar i;
  uchar *PData;
  PData=Data_Read;
  for(i=0;i<Num;i++)
  {
		Start();
		Send(0xa0);
		Ack();
		Send(Address+i);
		Ack();
		Start();
		Send(0xa1);
		Ack();
		*(PData+i)=Read();
		SCL=0;
		DeAck();
		Stop();
  }
}

/*
void main()
{
	uchar Password_Write[]={"262626"};
	uchar Password_Read[]={"      "};

	SP=0x5f;
  Init_Lcd();

	Write_ROM(Password_Write,0,6);
	Read_ROM(Password_Read,0,6);
	//Change_Screen(Password_Write);
	//DelayMs(3000);
	Change_Screen(Password_Read);
	DelayMs(3000);
}
*/

#endif