#include <reg52.h>

#define uint  unsigned int
#define uchar unsigned char
#define x1    0x80	//第一大行的X坐标
#define x2    0x88	//第二大行的X坐标
#define y     0x80	//一二大行共用一个Y坐标，XY确定位置
#define comm  0
#define dat   1

sbit cs   = P0^0;
sbit std  = P0^1;
sbit sclk = P0^2;
sbit psb  = P0^3;   //H=并口; L=串口;
sbit rst  = P0^4;   //Reset Signal 低电平有效

void wr_lcd (uchar dat_comm,uchar content);
void delay (uint us);

uchar code tab1[]={
"本系列中文模块内"
"任意位置反白显示"
"置二级字库，可在"
"使用更方便更灵活"
};

uchar code tab31[]={
"金鹏科技有限公司"
"Golden Palm TECH"
};
uchar code tab32[]={
/*--  调入了一幅图像：F:\梁\画图\HOCO12832.bmp  --*/
/*--  宽度x高度=128x32  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x1E,0x00,0x00,0x00,0x00,0x00,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x7F,0x80,0x00,0x00,0x00,0x00,0x18,0x0C,0x00,0x00,0x01,0x00,0x00,0x00,0x00,
0x01,0xFF,0x80,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x13,0x10,0x03,0xFE,0x00,
0x03,0xFF,0xC0,0x00,0x00,0x00,0x00,0x58,0x00,0x00,0x00,0x3F,0x30,0x1F,0xFF,0xC0,
0x03,0xFF,0xE0,0x00,0x00,0x00,0x00,0x8C,0x03,0xF0,0x00,0x7F,0xE0,0x7C,0x01,0xE0,
0x03,0xFF,0xF0,0x00,0x00,0x00,0x01,0x36,0x06,0xC0,0x00,0x5F,0xC0,0xFF,0xFC,0x60,
0x01,0xFF,0xF0,0x00,0x00,0x00,0x02,0x1B,0x0F,0x80,0x00,0xFF,0x01,0xFE,0x0F,0x30,
0x00,0xEF,0xF0,0x00,0x00,0x00,0x02,0x6D,0x9F,0x00,0x00,0x3E,0x03,0xFF,0xF1,0x90,
0x00,0xFF,0xF8,0x00,0x00,0x00,0x04,0x36,0xFE,0x00,0x01,0xFF,0x07,0xFF,0xFC,0x90,
0x00,0xEF,0xFF,0xFF,0x80,0x00,0x04,0xDB,0x7E,0x00,0x03,0xFF,0x87,0xFF,0xFC,0xD0,
0x00,0x0F,0xFF,0xFF,0xC0,0x00,0x04,0x6D,0xFC,0x00,0x07,0xFF,0x8F,0xFF,0xFE,0x50,
0x00,0x0F,0xFF,0xFF,0xE0,0x00,0x04,0x36,0xFC,0x10,0x07,0xFF,0x8F,0xFF,0xFE,0x90,
0x00,0x0F,0xFF,0xFF,0xE0,0x00,0x04,0x1B,0xF8,0x10,0x07,0xFF,0xCF,0xFF,0xFE,0x80,
0x00,0x0F,0xFF,0xFF,0xF0,0x00,0x04,0x0F,0xF8,0x10,0x07,0xFF,0xFF,0xFF,0xFA,0x00,
0x00,0x07,0xFF,0xFF,0xF0,0x00,0x04,0x07,0xF0,0x10,0x07,0xFF,0xFF,0xFF,0xFA,0x00,
0x00,0xFF,0xFF,0xFF,0xF8,0x00,0x02,0x03,0xF0,0x20,0x07,0xFF,0xFF,0xFF,0xBA,0x00,
0x00,0xFD,0xFF,0xFF,0xFC,0x00,0x02,0x03,0xF0,0x20,0x03,0xFF,0xFF,0xDF,0xB8,0x00,
0x00,0xC1,0xC0,0x3F,0xFC,0x00,0x01,0x01,0xE0,0x40,0x00,0xFF,0xFF,0xDF,0xB0,0x00,
0x00,0x81,0xC0,0x3F,0xCE,0x00,0x00,0x81,0xE0,0x80,0x00,0x7F,0xFF,0xDF,0xA0,0x00,
0x00,0x81,0x80,0x1D,0xCF,0x00,0x00,0x41,0xE1,0x00,0x00,0x3F,0xFF,0x9B,0x00,0x00,
0x01,0x83,0x80,0x1F,0xC7,0x80,0x00,0x21,0xE2,0x00,0x00,0x1F,0xFD,0xB6,0x00,0x00,
0x01,0xC3,0x00,0x0E,0xE6,0x80,0x00,0x19,0xEC,0x00,0x00,0x07,0xFE,0x20,0x00,0x00,
0x00,0xC3,0x00,0x07,0x67,0x40,0x00,0x07,0xF0,0x00,0x00,0x03,0x3E,0x00,0x00,0x00,
0x00,0x02,0x00,0x03,0xE7,0xA0,0x00,0x00,0x00,0x00,0x00,0x02,0x8E,0x00,0x00,0x00,
0x00,0x06,0x00,0x03,0x83,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x03,0x00,0x00,0x00,
0x00,0x06,0x00,0x07,0x03,0x00,0x77,0x46,0x74,0x24,0x80,0x06,0x04,0x00,0x00,0x00,
0x00,0x1C,0x00,0x06,0x00,0x00,0x55,0x45,0x54,0x57,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x1C,0x00,0x0E,0x00,0x00,0x45,0x45,0x74,0x57,0x80,0x08,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1C,0x00,0x00,0x55,0x45,0x44,0x74,0x80,0xF0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x38,0x00,0x00,0x77,0x76,0x47,0x54,0x80,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};

uchar code tab5[]={
/*--  调入了一幅图像：F:\梁\画图\COCK.bmp  --*/
/*--  宽度x高度=128x64  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x01,0x00,0x80,0x00,0x0F,0xFF,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0C,0x43,0x01,0x80,0x00,0x7F,0xFF,0xF0,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x19,0xFF,0x07,0x00,0x07,0xFF,0xFF,0xFE,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0x3F,0xFC,0x1E,0x00,0x1F,0xFF,0xFF,0xFF,0x80,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xBF,0xFF,0xFC,0x00,0x7F,0xFC,0x00,0x7F,0xC0,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xFF,0xFF,0xF0,0x00,0xFF,0xC0,0x00,0x0F,0xE0,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xFF,0xFF,0xE0,0x03,0xFF,0xFF,0xFC,0x01,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xDF,0xFF,0xC0,0x07,0xFF,0xFF,0xFF,0x80,0xF0,0x00,0x00,0x00,
0x00,0x00,0x00,0x03,0xFF,0xFF,0x00,0x0F,0xFF,0xFF,0xFF,0xE0,0x38,0x00,0x00,0x00,
0x00,0x00,0x00,0x07,0xFF,0xF8,0x00,0x1F,0xFF,0xF0,0x03,0xF8,0x38,0x00,0x00,0x00,
0x00,0x00,0x00,0x07,0xFF,0xE0,0x00,0x3F,0xFF,0xFF,0xC0,0x7C,0x18,0x00,0x00,0x00,
0x00,0x00,0x00,0x04,0x7F,0xF0,0x00,0x3F,0xFF,0xFF,0xF8,0x1E,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xFF,0xF8,0x00,0x7F,0xFF,0xFF,0xFE,0x0F,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0xFF,0xFC,0x00,0xFF,0xFF,0xFF,0xFF,0x87,0x08,0x00,0x00,0x00,
0x00,0x00,0x00,0x1F,0xFF,0xFE,0x00,0xFF,0xFF,0xFF,0xFF,0x83,0x88,0x00,0x00,0x00,
0x00,0x00,0x00,0x3F,0xFF,0xFE,0x01,0xFF,0xFF,0xFF,0xFF,0xC3,0x88,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0xFF,0xFE,0x01,0xFF,0xFF,0xFF,0xFF,0xE1,0x88,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFE,0x03,0xFF,0xFF,0xFF,0xFF,0xE1,0x88,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFE,0x03,0xFF,0xFF,0xFF,0xFF,0xF1,0x88,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFE,0x03,0xFF,0xFF,0xFF,0xFF,0xF3,0x08,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0x07,0xFF,0xFF,0xFF,0xFF,0xF2,0x10,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0x8F,0xFF,0xFF,0xFF,0xFF,0xF0,0x20,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF0,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x70,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0x70,0x00,0x00,0x00,0x00,
0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFE,0xFF,0x70,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0x7F,0xFF,0x7F,0x20,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xBF,0xFF,0x7F,0x20,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFF,0xFF,0xDF,0xFF,0x7E,0x20,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFF,0xFF,0xDF,0xFF,0x3E,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x1F,0xFF,0xFF,0xFF,0xFF,0xCF,0xFF,0x3C,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0xFF,0xFF,0xFF,0xFF,0xCF,0xFE,0x38,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x07,0xFF,0xFF,0xFF,0xFF,0x8F,0xFE,0x38,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x01,0xFF,0xFF,0xFF,0xFF,0x8F,0xFE,0x30,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x1F,0x7C,0x20,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x7F,0xFF,0xFF,0xFE,0x1E,0x78,0x00,0x40,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x3F,0xFF,0xFF,0xFE,0x1E,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x1F,0xFF,0xFF,0xDA,0x3C,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x07,0xFF,0xFF,0xF2,0x30,0x80,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0xFF,0xFF,0xF1,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xFE,0x7F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x7C,0x3F,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x78,0x1F,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x72,0x07,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x70,0x00,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x78,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x60,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0xC0,0x03,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x40,0x04,0xD8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x01,0x00,0x01,0x08,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x02,0x0C,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x02,0x40,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x0C,0x48,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x38,0x41,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x0F,0xE2,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};


/*------------------初始化-----------------*/
void init_lcd (void)
{
  rst=1;
  psb=0;
  wr_lcd (comm,0x30);  /*30---基本指令动作*/   
  wr_lcd (comm,0x01);  /*清屏，地址指针指向00H*/
  delay (100);
  wr_lcd (comm,0x06);  /*光标的移动方向*/
  wr_lcd (comm,0x0c);  /*开显示，关游标*/
}
/*---------------显示汉字或字符----------------*/
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
/*-----------上半屏显示汉字或字符------------*/
void chn_disp1 (uchar code *chn)
{
  uchar i,j;
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x80);
  j=0;
  for (i=0;i<16;i++)
  wr_lcd (dat,chn[j*16+i]);
  wr_lcd (comm,0x90);
  j=1;
  for (i=0;i<16;i++)
  wr_lcd (dat,chn[j*16+i]);
}
/*----------------显示图形-----------------*/
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
/*-------------下半屏显示图形--------------*/
void img_disp1 (uchar code *img)
{
  uchar i,j;
  for(j=0;j<32;j++)
  {
    for(i=0;i<8;i++)
    {
      wr_lcd (comm,0x34);
      wr_lcd (comm,y+j);
      wr_lcd (comm,x2+i);
      wr_lcd (comm,0x30);
      wr_lcd (dat,img[j*16+i*2]);
      wr_lcd (dat,img[j*16+i*2+1]);
    }
  }
  wr_lcd (comm,0x36);
}
/*--------------显示点阵----------------*/
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
//当data1=0xff,data2=0xff时,在x0,y0处反白显示16xl*yl.
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
/*--------------清DDRAM------------------*/
void clrram (void)
{
  wr_lcd (comm,0x30);
  wr_lcd (comm,0x01);
  delay (180);
}
/*---------------------------------------*/
void wr_lcd (uchar dat_comm,uchar content)
{
  uchar a,i,j;
  delay (50);
  a=content;
  cs=1;
  sclk=0;
  std=1;
  for(i=0;i<5;i++)
  {
    sclk=1;
    sclk=0;
  }
  std=0;
  sclk=1;
  sclk=0;
  if(dat_comm)
    std=1;   //data
  else
    std=0;   //command
  sclk=1;
  sclk=0;
  std=0;
  sclk=1;
  sclk=0;
  for(j=0;j<2;j++)
  {
    for(i=0;i<4;i++)	//
    {
      a=a<<1;
      std=CY;	//CY->Carry,
      sclk=1;
      sclk=0;
    }
    std=0;
    for(i=0;i<4;i++)
    {
      sclk=1;
      sclk=0;
    }
  }
}
/*-----------------------------------*/
void delay (uint us)   //delay time us
{
  while(us--);
}
void delay1 (uint ms)	//delay time ms
{
  uint i,j;
  for(i=0;i<ms;i++)
  for(j=0;j<15;j++)
  delay(1);
}
/*------------------主程序--------------------*/
void main ()
{
  SP=0x5f;
  init_lcd ();
  while (1)
  {
    lat_disp (0x00,0x00);
    chn_disp (tab1);
    con_disp (0xff,0xff,0x8c,0x80,2,16);
    delay1 (8000);
    clrram();
    lat_disp (0xcc,0xcc);
    delay1 (8000);
    lat_disp (0x00,0x00);
    chn_disp1 (tab31);
    img_disp1 (tab32);
    delay1 (8000);
    clrram();
    lat_disp (0xff,0x00);
    delay1 (8000);
    img_disp (tab5);
    delay1 (8000);
  }
}
