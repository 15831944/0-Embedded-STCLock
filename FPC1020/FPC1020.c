#include <reg52.h>
#include "90.h"

#define TRUE  	0x01
#define FALSE 	0x00

#define DATA_START			0xf5	//���ݰ���ʼ
#define DATA_END				0xf5	//���ݰ�����

#define CMD_ENROLL1  		0x01	//���ָ�Ʋ���һ
#define CMD_ENROLL2  		0x02	//���ָ�Ʋ����
#define CMD_ENROLL3  		0x03	//���ָ�Ʋ�����
#define CMD_DELETE  		0x04	//ɾ��ָ�����ָ��
#define CMD_CLEAR  			0x05	//�������ָ��
#define CMD_USERNUMB  	0x09	//ȡ�û�����
#define CMD_IDENTIFY  	0x0b	//1:1�ȶ�
#define CMD_SEARCH  		0x0c	//1:N�ȶ�

#define ACK_SUCCESS  		0x00	//�����ɹ�
#define ACK_FAIL	  		0x01	//����ʧ��
#define ACK_FULL	  		0x04	//ָ�����ݿ�����
#define ACK_NOUSER   		0x05	//�޴��û�
#define ACK_USER_EXIST 	0x07 	//�û��Ѵ���
#define ACK_TIMEOUT  		0x08	//�ɼ���ʱ

#define UART_BUF_LEN		8
#define BUF_N 					8

#define uint unsigned int
#define uchar unsigned char

#define BEEP  P42
sbit F_Scan = P0^0;
sbit F_Input = P0^1;
sbit F_Success = P0^2;
sbit F_Fail = P0^3;
sbit O=P1^1;

uchar rBuf[UART_BUF_LEN];          //���շ�����Ϣ
uchar tBuf[UART_BUF_LEN];          //���������������
uchar g_ucUartRxEnd ;              //���շ�����Ϣ������־
uchar g_ucUartRxLen ;       	     //���շ�����Ϣ����
uchar l_ucFPID;             	     //�û����

uchar p;

void UartInit(void)		//19200bps@11.0592MHz
{
	PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFD;			//�趨��ʱ��ֵ
	TH1 = 0xFD;			//�趨��ʱ����װֵ
	ET1 = 0;				//��ֹ��ʱ��1�ж�
	TR1 = 1;				//������ʱ��1
	ES = 1 ;
	EA = 1 ;
}
void SendData(uchar dat)
{
	SBUF=dat;
	while(!TI);
	TI=0;
}
void UartSend(uchar *Datar,uchar cLength)
{
	do{
		SendData(*(Datar++));
	}while(--cLength != 0);
}
void Uart_Isr() interrupt 4 using 1
{    
	if(RI)
	{
		RI = 0;		//Clear receive interrupt flag
		rBuf[p++] = SBUF;
		if(p==8)
			g_ucUartRxEnd = 0xff;
	}
}
void Delay_ms(uint ms)                                                         
{                                                                          
	uint i=0,j=0;
	for(i=0;i<ms;i++)
		for(j=0;j<123;j++);
}
/*******************************************************************************
**���ܣ��ȴ����ݰ��������
*******************************************************************************/
uchar WaitFpData(void)
{
  uchar i;
  
  for(i=200; i>0; i--)	//�ȴ�ָ��оƬ����
  {
    Delay_ms(40);
    if(g_ucUartRxEnd)	break;
  }  
  if(i==0)
		return FALSE;	//ָ��оƬû�з���
  else
		return TRUE;
}
/*******************************************************************************
**����: ����У��ֵ
**����: Ҫ���͵�����ָ���ַ
**����: У��ֵ
*******************************************************************************/
uchar CmdGenCHK(uchar wLen,uchar *ptr)
{
	uchar i,temp = 0;
	
	for(i = 0; i < wLen; i++)
	{
		temp ^= *(ptr + i);
	}
	return temp;
}
/*******************************************************************************
**����: ���Ϳ���ָ��оƬָ��
**����: wLen ���ݳ���
        cpPara ���͵�����
**���أ�void
*******************************************************************************/
void UART_SendPackage(uchar wLen,uchar *ptr)
{
  uint i=0,len=0;
 
  tBuf[0] = DATA_START;	//ָ���
  for(i = 0; i < wLen; i++)	// data in packet 
  {
    tBuf[1+i] = *(ptr+i);
  } 
  
  tBuf[wLen + 1] = CmdGenCHK(wLen, ptr);	//Generate checkout data
  tBuf[wLen + 2] = DATA_END;
  len = wLen + 3;
	
  g_ucUartRxEnd = 0;
  g_ucUartRxLen = len ;
	
	UartSend(tBuf,len);
} 
/*******************************************************************************
**���ܣ�������Ϣ����
**������ cmd ��ͬ���ͬ����
**���أ�������
*******************************************************************************/
uchar Check_Package(uchar cmd)
{
	uchar flag = FALSE;
	if(!WaitFpData())
		return flag; //�ȴ����շ�����Ϣ
	p = 0 ;
	if(g_ucUartRxEnd)
		g_ucUartRxEnd = 0;	//�����ݰ����ձ�־
	else 
		return flag;

  if(rBuf[0] != DATA_START)
		return flag;
	if(rBuf[1] != cmd)
		return flag;
	if(rBuf[6] != CmdGenCHK(g_ucUartRxLen - 3, &rBuf[1]))
		return flag;
	switch(cmd)
	{
		case CMD_ENROLL1:
		case CMD_ENROLL2:
		case CMD_ENROLL3:
			if(ACK_SUCCESS == rBuf[4])
				flag = TRUE;
			else if(ACK_USER_EXIST == rBuf[4])
			{
				//Spk_HaveUser();
				Delay_ms(1500);
			}
			break;
		case CMD_DELETE:  //ɾ��ָ�����ָ��
		case CMD_CLEAR:    //�������ָ��
		case CMD_IDENTIFY:  //1:1�ȶ�
			if(ACK_SUCCESS == rBuf[4])
				flag = TRUE;
			break;
		case CMD_USERNUMB:  //ȡ�û�����
			if(ACK_SUCCESS == rBuf[4])
			{
				flag = TRUE;
				l_ucFPID = rBuf[3];
			}
			break;
		case CMD_SEARCH:   //1:N�ȶ�
			if((1 == rBuf[4])||(2 == rBuf[4])||(3 == rBuf[4]))
			{
				flag = TRUE;
				l_ucFPID = rBuf[3];
			}
			break;
		default:
			break;
	}
    return flag;
}
/*******************************************************************************
**���ܣ���CharBuffer1 ��CharBuffer2 �е������ļ����������򲿷�ָ�ƿ�
*******************************************************************************/
void FP_Search(void)
{
  uchar buf[BUF_N];
  
  *buf = CMD_SEARCH;          //1:N�ȶ�
  *(buf+1) = 0x00;
  *(buf+2) = 0x00;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}
/*******************************************************************************
**���ܣ���� flash ָ�ƿ�
*******************************************************************************/
void FP_Clear(void)
{
  uchar buf[BUF_N];
  
  *buf = CMD_CLEAR;
  *(buf+1) = 0x00;
  *(buf+2) = 0x00;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}
/*******************************************************************************
**���ܣ�ɾ��ָ�����ָ��
**������u_id
**���أ�void
*******************************************************************************/
void FP_Delete(uint u_id)
{
  uchar buf[BUF_N];
  
  *buf = CMD_DELETE;
  *(buf+1) = u_id>>8;
  *(buf+2) = u_id&0xff;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;
  UART_SendPackage(5, buf);
}
/*******************************************************************************
**���ܣ�1:1�ȶ�
**������u_id
**���أ�void
*******************************************************************************/
void FP_Identify(uint u_id)
{
  uchar buf[BUF_N];
  
  *buf = CMD_IDENTIFY;
  *(buf+1) = u_id>>8;
  *(buf+2) = u_id&0xff;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;
  UART_SendPackage(5, buf);
}
/*******************************************************************************
**ע��ָ��
**��������ָ��ע��һ��ָ��ģ��
**������UserID ָ�ƺ�
*******************************************************************************/
void Enroll_Step1(uint u_id)
{
  uchar buf[BUF_N];
  
  *buf = CMD_ENROLL1;
  *(buf+1) = u_id>>8;
  *(buf+2) = u_id&0xff;
  *(buf+3) = 1;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}
/*******************************************************************************
**ע��ָ��
**��������ָ��ע��һ��ָ��ģ��
**������UserID ָ�ƺ�
*******************************************************************************/
void Enroll_Step2(uint u_id)
{
  uchar buf[BUF_N];
  
  *buf = CMD_ENROLL2;
  *(buf+1) = u_id>>8;
  *(buf+2) = u_id&0xff;
  *(buf+3) = 1;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}
/*******************************************************************************
**ע��ָ��
**��������ָ��ע��һ��ָ��ģ��
**������UserID ָ�ƺ�
*******************************************************************************/
void Enroll_Step3(uint u_id)
{
  uchar buf[BUF_N];
  
  *buf = CMD_ENROLL3;
  *(buf+1) = u_id>>8;
  *(buf+2) = u_id&0xff;
  *(buf+3) = 1;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}
/*******************************************************************************
**ע��ָ��
**��������ָ��ע��һ��ָ��ģ��
**������UserID ָ�ƺ�
*******************************************************************************/
uchar Finger_Enroll(uint u_id)
{
	Enroll_Step1(u_id);
	if(FALSE == Check_Package(CMD_ENROLL1))
		return FALSE;
	Delay_ms(100);
	
	Enroll_Step2(u_id);
	if(FALSE == Check_Package(CMD_ENROLL2))
		return FALSE;
	Delay_ms(100);
	
	Enroll_Step3(u_id);
	return Check_Package(CMD_ENROLL3);
}
/*******************************************************************************
**���ָ��
**������UserID ָ�ƺ�
*******************************************************************************/
uchar Finger_Clear(void)
{
	FP_Clear();
	//if(FALSE == WaitFpData())return FALSE;
	return Check_Package(CMD_CLEAR);
}
/*******************************************************************************
**ɾ��ָ��ָ��
**������UserID ָ�ƺ�
*******************************************************************************/
uchar Finger_Delete(uint u_id)
{
	FP_Delete(u_id);
	//if(FALSE == WaitFpData())return FALSE;
	return Check_Package(CMD_DELETE);
}
/*******************************************************************************
**��ȡ�û�����
*******************************************************************************/
uchar Finger_Read(void)
{
	 uchar buf[BUF_N];
  
  *buf = CMD_USERNUMB;
  *(buf+1) = 0x00;
  *(buf+2) = 0x00;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;
  UART_SendPackage(5, buf);
	return Check_Package(CMD_USERNUMB);
}
/*******************************************************************************
**��ȡ�û�����
*******************************************************************************/
uchar Finger_Search(void)
{
	FP_Search();
	return Check_Package(CMD_SEARCH);
}

void BBP(uchar times)
{
	uchar i;  
	switch (times)
	{
		case 1:
			F_Success=0;
			F_Fail=1;
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			break;
			
		case 2:
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			break;
			
		case 3:
			F_Success=1;
			F_Fail=0;
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			Delay_ms(200);
			BEEP=~BEEP;
			break;
			
		default:
			break;
	}
}
void Mode(uchar MODE)
{
	uchar User_ID;
	switch ( MODE )
	{                                                                     
		case 0:
			break;

		case 1:
			MODE = 0;	//����ָ���û�
			if(Finger_Enroll(++User_ID))
				BBP(1);
			else
				BBP(3);
			Delay_ms(2000);
			break;

		case 2:
			MODE = 0;	//ɾ������ָ���û�
			if(Finger_Search())
				BBP(2);
			else
			{
				BBP(3);
				break;
			}
			if(Finger_Delete(l_ucFPID))
				BBP(1);
			else
				BBP(3);
			Delay_ms(1000);
			break;

		case 3:
			MODE = 0;	//1:N����ָ�ƿ�
			if(Finger_Search())
				BBP(1);
			else
				BBP(3);
			Delay_ms(1000);
			break;
		
		case 4:
			MODE = 0;	//��ָ���û�����
			if(Finger_Read())
				BBP(1);
			else
				BBP(3);
			Delay_ms(1000);
			break;

		case 5:
			MODE = 0;	//���ָ�ƿ�
			if(Finger_Clear())
				BBP(1);
			else
				BBP(3);
			Delay_ms(1000);
			break;
		
		default:
			break;
	}
}

void main()
{
	F_Scan=1;
	F_Input=1;
	F_Success=1;
	F_Fail=1;
	O=0;
	P42=0;
	

	UartInit();
	BBP(2);
	while(1)
	{
		F_Success=1;
		F_Fail=1;
		if(!F_Scan)
		{
			while(!F_Scan);
			BBP(2);
			Mode(3);
		}
		else if(!F_Input)
		{
			while(!F_Input);
			BBP(2);
			Mode(1);
		}
	}
}
