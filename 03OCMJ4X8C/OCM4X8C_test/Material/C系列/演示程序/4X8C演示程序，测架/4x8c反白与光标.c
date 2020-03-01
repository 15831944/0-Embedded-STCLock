#include <reg52.h>

#define uint  unsigned int
#define uchar unsigned char
#define x1    0x80
#define x2    0x88
#define y     0x80
#define comm  0
#define dat   1

sbit rs = P0^0;   //H=data; L=command;
sbit rw = P0^1;   //H=read; L=write;
sbit  e = P0^2;   //input enable;
sbit psb= P0^3;   //H=����; L=����;
sbit rst= P0^4;   //Reset Signal �͵�ƽ��Ч

sbit busy=P1^7;   //lcd busy bit

void wr_lcd (uchar dat_comm,uchar content);
void chk_busy (void);
void delay (uint us);

uchar code tab1[]={
"��ϵ������ģ����"
"����λ�÷�����ʾ"
"�ö����ֿ⣬����"
"ʹ�ø���������"
};

uchar code tab2[]={
"�����Ƽ����޹�˾"
"Golden Palm TECH"
"�¿���Һ����ʾ��"
"TEL:0758-2263858"
};


/*------------------��ʼ��-----------------*/
void init_lcd (void)
{
  rst=1;
  psb=1;
  wr_lcd (comm,0x30);  /*30---����ָ���*/   
  wr_lcd (comm,0x01);  /*��������ַָ��ָ��00H*/
  wr_lcd (comm,0x06);  /*�����ƶ�����*/
  wr_lcd (comm,0x0c);  /*����ʾ�����α�*/
}
/*---------------��ʾ���ֻ��ַ�----------------*/
void chn_disp (uchar code *chn)
{
  uchar i,j;
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x80);
  for (j=0;j<4;j++)
  {
    for (i=0;i<16;i++)
    wr_lcd (dat,chn[j*16+i]);
  }
}
/*----------------��ʾͼ��-----------------*/
void img_disp (uchar code *img)
{
  uchar i,j;
  for(j=0;j<32;j++)
  {
    for(i=0;i<8;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j);
      wr_lcd (comm,x1+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,img[j*16+i*2]);
      wr_lcd (dat,img[j*16+i*2+1]);
    }
  }
  for(j=32;j<64;j++)
  {
    for(i=0;i<8;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j-32);
      wr_lcd (comm,x2+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,img[j*16+i*2]);
      wr_lcd (dat,img[j*16+i*2+1]);
    }
  }
  wr_lcd (comm,0x36);
}
/*--------------��ʾ����----------------*/
void lat_disp (uchar data1,uchar data2)
{
  uchar i,j,k,x;
  x=x1;
  for(k=0;k<2;k++)
  {
    for(j=0;j<16;j++)
    {
      for(i=0;i<8;i++)
      {
        wr_lcd (comm,0x34);
        wr_lcd (comm,y+j*2);
        wr_lcd (comm,x+i);
        wr_lcd (comm,0x30);
        wr_lcd (dat,data1);
        wr_lcd (dat,data1);
      }
      for(i=0;i<8;i++)
      {
        wr_lcd (comm,0x34);
        wr_lcd (comm,y+j*2+1);
        wr_lcd (comm,x+i);
        wr_lcd (comm,0x30);
        wr_lcd (dat,data2);
        wr_lcd (dat,data2);
      }
    }
    x=x2;
  }
  wr_lcd (comm,0x36);
}
/*-----------------------------------------------*/
//��data1=0xff,data2=0xffʱ,��x0,y0��������ʾ16xl*yl.
//��data1=0xff,data2=0x00ʱ,��x0,y0��ǰ�뷴����ʾ8xl*yl.
//��data1=0x00,data2=0xffʱ,��x0,y0����뷴����ʾ8xl*yl.
void con_disp (uchar data1,uchar data2,uchar x0,uchar y0,uchar xl,uchar yl)
{
  uchar i,j;
  for(j=0;j<yl;j++)
  {
    for(i=0;i<xl;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y0+j);
      wr_lcd (comm,x0+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,data1);
      wr_lcd (dat,data2);
    }
  }
  wr_lcd (comm,0x36);
}
/*--------------------------------------------*/
//��data1=0xff,data2=0x00ʱ,��x0,y0����ʾǰ����.
//��data1=0x00,data2=0xffʱ,��x0,y0����ʾ�����.
//��data1=0x00,data2=0x00ʱ,��x0,y0������ʾ���.
//y0=0x8f or y0=0x9f.
void cur_disp (uchar data1,uchar data2,uchar x0,uchar y0)
{
  wr_lcd (comm,0x34);
  wr_lcd (comm,y0);
  wr_lcd (comm,x0);
  wr_lcd (comm,0x30);
  wr_lcd (dat,data1);
  wr_lcd (dat,data2);
  wr_lcd (comm,0x36);
}
/*--------------��DDRAM------------------*/
void clrram (void)
{
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x01);
}
/*---------------------------------------*/
void wr_lcd (uchar dat_comm,uchar content)
{
  chk_busy ();
  if(dat_comm)
   {
    rs=1;   //data
    rw=0;   //write
   }
  else 
   {
    rs=0;   //command
    rw=0;   //write
   }
  P1=content;   //output data or comm
  e=1;
  ;
  e=0;
}

void chk_busy (void)
{
  P1=0xff;
  rs=0;
  rw=1;
  e =1;
  while(busy==1);
  e =0;
}

void delay (uint us)   //delay time
{
  while(us--);
}
void delay1 (uint ms)
{
  uint i,j;
  for(i=0;i<ms;i++)
  for(j=0;j<15;j++)
  delay(1);
}
/*------------------������--------------------*/
void main ()
{
  uchar i;
  init_lcd ();
  while (1)
  {
/*    lat_disp (0x00,0x00);
    chn_disp (tab1);
    con_disp (0xff,0x00,0x8c,0x80,2,16);
    delay1 (8000);
  }
}*/
    lat_disp (0x00,0x00);
    chn_disp (tab2);
    for(i=0;i<8;i++)
    {
      cur_disp (0xff,0x00,0x88+i,0x8f);
      delay1 (2000);
      cur_disp (0x00,0x00,0x88+i,0x8f);
      cur_disp (0x00,0xff,0x88+i,0x8f);
      delay1 (2000);
      cur_disp (0x00,0x00,0x88+i,0x8f);
    }
    for(i=0;i<8;i++)
    {
      cur_disp (0xff,0x00,0x88+i,0x9f);
      delay1 (2000);
      cur_disp (0x00,0x00,0x88+i,0x9f);
      cur_disp (0x00,0xff,0x88+i,0x9f);
      delay1 (2000);
      cur_disp (0x00,0x00,0x88+i,0x9f);
    }
  }
}
