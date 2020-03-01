#include <reg52.h>

#define uchar unsigned char
#define uint unsigned int

uchar code KC[]={0xFE,0xFD,0xFB,0xF7};	//����ɨ������б�

/*-----10ms�ӳ�-----*/
void Delay_10Ms()
{
	uint TempCyc=11104;
	while(TempCyc--);
}
/*-----������Ⲣ���ذ���ֵ-----*/
uchar Key_Check()
{
 	uchar Row,Col,i;
 	P2=0xf0;	//P2�ӿڳ�ʼ��
 	if((P2&0xf0)!=0xf0)
 	{
		Delay_10Ms();
		if((P2&0xf0)!=0xf0)
		{
			Row=P2^0xf0;	//ȷ������
			i=0;
			P2=KC[i];	//��ȷ��λ
			while(i<4)
			{
	 			if((P2&0xf0)!=0xf0)
				{
					Col=~(P2&0xff);	//ȷ������
					break;	//�Ѷ�λ����ǰ�˳�
	  		}
				else 
				{
					i++;
					P2=KC[i];
				}
			}
		}
		else 
		{
			return 0;
		}
		while((P2&0xf0)!=0xf0);
		P0=Row|Col;	//P0��8��LED��ʾ����ֵ
		return Row|Col;	//������������Ϻ󷵻�
 	}
 	else return 0;	//�޼�����ʱ����0
}

void main()
{
	while(1) 
	{ 
		Key_Check();
	}
}
