/*******************************************************************************
**�ļ�����BIOVO.c 
**˵����ָ��оƬ���ݴ���
**�����Ա�����Ǣ
**��дʱ�䣺2013.04.07
*******************************************************************************/
#include "config.h"
#define UART_BUF_LEN  8
#define BUF_N 	8

unsigned char rBuf[UART_BUF_LEN];          //���շ�����Ϣ
unsigned char tBuf[UART_BUF_LEN];          //���������������
unsigned char g_ucUartRxEnd ;              //���շ�����Ϣ������־
unsigned char g_ucUartRxLen ;             //���շ�����Ϣ����
unsigned char l_ucFPID;                  //�û����

extern unsigned char p;  //  ���ڽ��յ�ַ


/*******************************************************************      
**���ܣ���ʱ����
**������
**���أ���                                                                
*******************************************************************/      
                                                                           
void Delay_ms(unsigned int ms)                                                         
{                                                                          
		unsigned int i=0,j=0;
		for(i=0;i<ms;i++)
		for(j=0;j<123;j++);
}
/*******************************************************************************
**���ܣ��ȴ����ݰ��������
**������
**���أ���
*******************************************************************************/
unsigned char WaitFpData(void)
{
  unsigned char i;
  
  for(i=200; i>0; i--)//�ȴ�ָ��оƬ����
  {
    Delay_ms(40);
    if(g_ucUartRxEnd)break;
  }  
  if(i==0)return FALSE;//ָ��оƬû�з���
  else return TRUE;
}

/*******************************************************************************
**����: ����У��ֵ
**����: Ҫ���͵�����ָ���ַ
**����: У��ֵ
*******************************************************************************/
unsigned char CmdGenCHK(unsigned char wLen,unsigned char *ptr)
{
	unsigned char i,temp = 0;
	
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
void UART_SendPackage(unsigned char wLen,unsigned char *ptr)
{
  unsigned int i=0,len=0;
 
  tBuf[0] = DATA_START;//ָ���
  for(i = 0; i < wLen; i++)      // data in packet 
  {
    tBuf[1+i] = *(ptr+i);
  } 
  
  tBuf[wLen + 1] = CmdGenCHK(wLen, ptr);         //Generate checkout data
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
unsigned char Check_Package(unsigned char cmd)
{
    unsigned char flag = FALSE;
	   if(!WaitFpData()) return flag; //�ȴ����շ�����Ϣ
     p = 0 ;
    if(g_ucUartRxEnd)
      g_ucUartRxEnd = 0;//�����ݰ����ձ�־
    else 
      return flag;
    
  if(rBuf[0] != DATA_START)return flag;
	if(rBuf[1] != cmd)return flag;
	if(rBuf[6] != CmdGenCHK(g_ucUartRxLen - 3, &rBuf[1]))return flag;
	switch(cmd)
	{
	case CMD_ENROLL1:
	case CMD_ENROLL2:
	case CMD_ENROLL3:
		if(ACK_SUCCESS == rBuf[4])flag = TRUE;
		else if(ACK_USER_EXIST == rBuf[4])
		{
//		Spk_HaveUser();
			Delay_ms(1500);
		}
		break;
	case CMD_DELETE:  //ɾ��ָ�����ָ��
	case CMD_CLEAR:    //�������ָ��
	case CMD_IDENTIFY:  //1:1�ȶ�
		if(ACK_SUCCESS == rBuf[4])flag = TRUE;
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
**������
**���أ���
*******************************************************************************/
void FP_Search(void)
{
  unsigned char buf[BUF_N];
  
  *buf = CMD_SEARCH;          //1:N�ȶ�
  *(buf+1) = 0x00;
  *(buf+2) = 0x00;
  *(buf+3) = 0x00;
  *(buf+4) = 0x00;

  UART_SendPackage(5, buf);
}

/*******************************************************************************
**���ܣ���� flash ָ�ƿ�
**������
**���أ���
*******************************************************************************/
void FP_Clear(void)
{
  unsigned char buf[BUF_N];
  
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
void FP_Delete(unsigned int u_id)
{
  unsigned char buf[BUF_N];
  
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
void FP_Identify(unsigned int u_id)
{
  unsigned char buf[BUF_N];
  
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
void Enroll_Step1(unsigned int u_id)
{
  unsigned char buf[BUF_N];
  
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
void Enroll_Step2(unsigned int u_id)
{
  unsigned char buf[BUF_N];
  
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
void Enroll_Step3(unsigned int u_id)
{
  unsigned char buf[BUF_N];
  
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
unsigned char Finger_Enroll(unsigned int u_id)
{
	Enroll_Step1(u_id);
	if(FALSE == Check_Package(CMD_ENROLL1))return FALSE;
	Delay_ms(100);
	
	Enroll_Step2(u_id);
	if(FALSE == Check_Package(CMD_ENROLL2))return FALSE;
	Delay_ms(100);
	
	Enroll_Step3(u_id);
	return Check_Package(CMD_ENROLL3);
}

/*******************************************************************************
**���ָ��
**
**������UserID ָ�ƺ�
*******************************************************************************/
unsigned char Finger_Clear(void)
{
	FP_Clear();
//	if(FALSE == WaitFpData())return FALSE;
	return Check_Package(CMD_CLEAR);
}

/*******************************************************************************
**ɾ��ָ��ָ��
**
**������UserID ָ�ƺ�
*******************************************************************************/
unsigned char Finger_Delete(unsigned int u_id)
{
	FP_Delete(u_id);
//	if(FALSE == WaitFpData())return FALSE;
	return Check_Package(CMD_DELETE);
}
/*******************************************************************************
**��ȡ�û�����
**
**�����N
*******************************************************************************/
unsigned char Finger_Read(void)
{
	 unsigned char buf[BUF_N];
  
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
**
**����
*******************************************************************************/
unsigned char Finger_Search(void)
{
	 FP_Search();
	return Check_Package(CMD_SEARCH);
}








