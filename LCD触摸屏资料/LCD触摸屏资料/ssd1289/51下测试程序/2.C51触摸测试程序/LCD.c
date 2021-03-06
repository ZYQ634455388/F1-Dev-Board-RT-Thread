#include <reg51.h>
#include <intrins.h>

/*
单片机工作电压3.3V
程序默认IO连接方式：

控制线：RS-P3^5;    WR-P3^6;   RD-P3^7;   CS-P1^0;   REST-P1^1;

数据线: DB0-DB7依次连接P0^0-P0^7;  DB8-DB15依次连接P2^0-P2^7;

触摸功能连接方式：(不使用触摸可不连接)
D_CLK-P1^2;  D_CS-P1^3;  D_DIN-P1^4;  D_OUT-P1^6;  D_PENIRQ-P1^7;

SD卡接线；(不使用触摸可不连接)
SD_MISO-P31;  SD_SCK-P32;  SD_MOSI-P33;  SD_CS-P34;
*/
#define  LCD_DataPortH P2     //高8位数据口
#define  LCD_DataPortL P0     //低8位数据口,请确认P0口已经上拉10K电阻,不宜太小，最小4.7K,推荐10K.
sbit LCD_RS = P3^5;  		 //数据/命令切换
sbit LCD_WR = P3^6;		  //写控制
sbit LCD_RD =P3^7;		     //读控制
sbit LCD_CS=P1^0;	     	//片选	
sbit LCD_REST = P1^1;	      //复位   	

/* touch panel interface define */
sbit DCLK	   =    P1^2;
sbit CS        =    P1^3;
sbit DIN       =    P1^4;	
sbit DOUT      =    P1^6;
sbit Penirq       =    P1^7;   //检测触摸屏响应信号
unsigned int TP_X,TP_Y;	   //当前触控坐标

 
//8*12字符
unsigned char code zifu[]={         
/*--  文字:     --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  !  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,0x00,

/*--  文字:  "  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x28,0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  #  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x28,0x28,0xFC,0x28,0x50,0xFC,0x50,0x50,0x00,0x00,

/*--  文字:  $  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x20,0x78,0xA8,0xA0,0x60,0x30,0x28,0xA8,0xF0,0x20,0x00,

/*--  文字:  %  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x48,0xA8,0xB0,0x50,0x28,0x34,0x54,0x48,0x00,0x00,

/*--  文字:  &  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x50,0x50,0x78,0xA8,0xA8,0x90,0x6C,0x00,0x00,

/*--  文字:  '  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x40,0x40,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  (  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x04,0x08,0x10,0x10,0x10,0x10,0x10,0x10,0x08,0x04,0x00,

/*--  文字:  )  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x40,0x20,0x10,0x10,0x10,0x10,0x10,0x10,0x20,0x40,0x00,

/*--  文字:  *  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x20,0xA8,0x70,0x70,0xA8,0x20,0x00,0x00,0x00,

/*--  文字:  +  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x20,0x20,0xF8,0x20,0x20,0x20,0x00,0x00,0x00,

/*--  文字:  ,  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x40,0x80,

/*--  文字:  -  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  .  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x40,0x00,0x00,

/*--  文字:  /  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x08,0x10,0x10,0x10,0x20,0x20,0x40,0x40,0x40,0x80,0x00,

/*--  文字:  0  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00,

/*--  文字:  1  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x60,0x20,0x20,0x20,0x20,0x20,0x70,0x00,0x00,

/*--  文字:  2  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x10,0x20,0x40,0x80,0xF8,0x00,0x00,

/*--  文字:  3  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x08,0x30,0x08,0x08,0x88,0x70,0x00,0x00,

/*--  文字:  4  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x10,0x30,0x50,0x50,0x90,0x78,0x10,0x18,0x00,0x00,

/*--  文字:  5  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x80,0x80,0xF0,0x08,0x08,0x88,0x70,0x00,0x00,

/*--  文字:  6  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x90,0x80,0xF0,0x88,0x88,0x88,0x70,0x00,0x00,

/*--  文字:  7  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x90,0x10,0x20,0x20,0x20,0x20,0x20,0x00,0x00,

/*--  文字:  8  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x70,0x88,0x88,0x88,0x70,0x00,0x00,

/*--  文字:  9  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x88,0x78,0x08,0x48,0x70,0x00,0x00,

/*--  文字:  :  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x20,0x00,0x00,

/*--  文字:  ;  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x20,0x00,

/*--  文字:  <  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x04,0x08,0x10,0x20,0x40,0x20,0x10,0x08,0x04,0x00,0x00,

/*--  文字:  =  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,

/*--  文字:  >  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x40,0x20,0x10,0x08,0x04,0x08,0x10,0x20,0x40,0x00,0x00,

/*--  文字:  ?  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x10,0x20,0x20,0x00,0x20,0x00,0x00,

/*--  文字:  @  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x98,0xA8,0xA8,0xB8,0x80,0x78,0x00,0x00,

/*--  文字:  A  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x20,0x30,0x50,0x50,0x78,0x48,0xCC,0x00,0x00,

/*--  文字:  B  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF0,0x48,0x48,0x70,0x48,0x48,0x48,0xF0,0x00,0x00,

/*--  文字:  C  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x78,0x88,0x80,0x80,0x80,0x80,0x88,0x70,0x00,0x00,

/*--  文字:  D  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF0,0x48,0x48,0x48,0x48,0x48,0x48,0xF0,0x00,0x00,

/*--  文字:  E  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x48,0x50,0x70,0x50,0x40,0x48,0xF8,0x00,0x00,

/*--  文字:  F  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x48,0x50,0x70,0x50,0x40,0x40,0xE0,0x00,0x00,

/*--  文字:  G  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x38,0x48,0x80,0x80,0x9C,0x88,0x48,0x30,0x00,0x00,

/*--  文字:  H  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xCC,0x48,0x48,0x78,0x48,0x48,0x48,0xCC,0x00,0x00,

/*--  文字:  I  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x20,0x20,0x20,0x20,0x20,0x20,0xF8,0x00,0x00,

/*--  文字:  J  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x7C,0x10,0x10,0x10,0x10,0x10,0x10,0x90,0xE0,0x00,

/*--  文字:  K  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xEC,0x48,0x50,0x60,0x50,0x50,0x48,0xEC,0x00,0x00,

/*--  文字:  L  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xE0,0x40,0x40,0x40,0x40,0x40,0x44,0xFC,0x00,0x00,

/*--  文字:  M  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xD8,0xD8,0xD8,0xD8,0xA8,0xA8,0xA8,0xA8,0x00,0x00,

/*--  文字:  N  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xDC,0x48,0x68,0x68,0x58,0x58,0x48,0xE8,0x00,0x00,

/*--  文字:  O  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,0x00,

/*--  文字:  P  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF0,0x48,0x48,0x70,0x40,0x40,0x40,0xE0,0x00,0x00,

/*--  文字:  Q  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x70,0x88,0x88,0x88,0x88,0xE8,0x98,0x70,0x18,0x00,

/*--  文字:  R  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF0,0x48,0x48,0x70,0x50,0x48,0x48,0xEC,0x00,0x00,

/*--  文字:  S  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x78,0x88,0x80,0x60,0x10,0x08,0x88,0xF0,0x00,0x00,

/*--  文字:  T  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0xA8,0x20,0x20,0x20,0x20,0x20,0x70,0x00,0x00,

/*--  文字:  U  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xCC,0x48,0x48,0x48,0x48,0x48,0x48,0x30,0x00,0x00,

/*--  文字:  V  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xCC,0x48,0x48,0x50,0x50,0x30,0x20,0x20,0x00,0x00,

/*--  文字:  W  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xA8,0xA8,0xA8,0x70,0x50,0x50,0x50,0x50,0x00,0x00,

/*--  文字:  X  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xD8,0x50,0x50,0x20,0x20,0x50,0x50,0xD8,0x00,0x00,

/*--  文字:  Y  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xD8,0x50,0x50,0x20,0x20,0x20,0x20,0x70,0x00,0x00,

/*--  文字:  Z  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xF8,0x90,0x10,0x20,0x20,0x40,0x48,0xF8,0x00,0x00,

/*--  文字:  [  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x38,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x38,0x00,

/*--  文字:  \  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x40,0x40,0x40,0x20,0x20,0x10,0x10,0x10,0x08,0x00,0x00,

/*--  文字:  ]  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x70,0x00,

/*--  文字:  ^  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x20,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  _  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFC,

/*--  文字:  `  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

/*--  文字:  a  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x30,0x48,0x38,0x48,0x3C,0x00,0x00,

/*--  文字:  b  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xC0,0x40,0x40,0x70,0x48,0x48,0x48,0x70,0x00,0x00,

/*--  文字:  c  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x38,0x48,0x40,0x40,0x38,0x00,0x00,

/*--  文字:  d  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x18,0x08,0x08,0x38,0x48,0x48,0x48,0x3C,0x00,0x00,

/*--  文字:  e  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x30,0x48,0x78,0x40,0x38,0x00,0x00,

/*--  文字:  f  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x1C,0x20,0x20,0x78,0x20,0x20,0x20,0x78,0x00,0x00,

/*--  文字:  g  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x3C,0x48,0x30,0x40,0x78,0x44,0x38,

/*--  文字:  h  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xC0,0x40,0x40,0x70,0x48,0x48,0x48,0xEC,0x00,0x00,

/*--  文字:  i  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x20,0x00,0x00,0x60,0x20,0x20,0x20,0x70,0x00,0x00,

/*--  文字:  j  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x10,0x00,0x00,0x30,0x10,0x10,0x10,0x10,0x10,0xE0,

/*--  文字:  k  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xC0,0x40,0x40,0x5C,0x50,0x70,0x48,0xEC,0x00,0x00,

/*--  文字:  l  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0xE0,0x20,0x20,0x20,0x20,0x20,0x20,0xF8,0x00,0x00,

/*--  文字:  m  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xF0,0xA8,0xA8,0xA8,0xA8,0x00,0x00,

/*--  文字:  n  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xF0,0x48,0x48,0x48,0xEC,0x00,0x00,

/*--  文字:  o  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x30,0x48,0x48,0x48,0x30,0x00,0x00,

/*--  文字:  p  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xF0,0x48,0x48,0x48,0x70,0x40,0xE0,

/*--  文字:  q  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x38,0x48,0x48,0x48,0x38,0x08,0x1C,

/*--  文字:  r  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xD8,0x60,0x40,0x40,0xE0,0x00,0x00,

/*--  文字:  s  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x78,0x40,0x30,0x08,0x78,0x00,0x00,

/*--  文字:  t  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x20,0x20,0x70,0x20,0x20,0x20,0x18,0x00,0x00,

/*--  文字:  u  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xD8,0x48,0x48,0x48,0x3C,0x00,0x00,

/*--  文字:  v  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xEC,0x48,0x50,0x30,0x20,0x00,0x00,

/*--  文字:  w  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xA8,0xA8,0x70,0x50,0x50,0x00,0x00,

/*--  文字:  x  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xD8,0x50,0x20,0x50,0xD8,0x00,0x00,

/*--  文字:  y  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0xEC,0x48,0x50,0x30,0x20,0x20,0xC0,

/*--  文字:  z  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x00,0x00,0x00,0x00,0x78,0x10,0x20,0x20,0x78,0x00,0x00,

/*--  文字:  {  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x18,0x10,0x10,0x10,0x20,0x10,0x10,0x10,0x10,0x18,0x00,

/*--  文字:  |  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,0x10,

/*--  文字:  }  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x00,0x60,0x20,0x20,0x20,0x10,0x20,0x20,0x20,0x20,0x60,0x00,

/*--  文字:  ~  --*/
/*--  宋体9;  此字体下对应的点阵为：宽x高=6x12   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=8x12  --*/
0x40,0xA4,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

    
};  

//**********************************************************
void spistar()                                     //SPI开始
{
CS=1;
DCLK=1;
DIN=1;
}
//**********************************************************
void WriteCharTo7843(unsigned char num)          //SPI写数据
{
unsigned char count=0;
DCLK=0;
for(count=0;count<8;count++)
{
num<<=1;
DIN=CY;
DCLK=0; _nop_();_nop_();_nop_();                //上升沿有效
DCLK=1; _nop_();_nop_();_nop_();
}
}
//**********************************************************
unsigned int ReadFromCharFrom7843()             //SPI 读数据
{
unsigned char count=0;
unsigned int Num=0;
for(count=0;count<12;count++)
{
Num<<=1;
DCLK=1; _nop_();_nop_();_nop_();                //下降沿有效
DCLK=0; _nop_();_nop_();_nop_();
if(DOUT) Num++;
}
return(Num);
}

void delayms(int count)  // /* X1ms */
{
        int i,j;
        for(i=0;i<count;i++)
                for(j=0;j<1000;j++);
}
void Lcd_Write_Com( int  DH)	 //命令
{	
    LCD_RS=0;
	LCD_CS =0;	 
	LCD_DataPortH=DH>>8;	
	LCD_DataPortL=DH;		
	LCD_WR=0;
	LCD_WR=1;
	LCD_CS =1;	
}
void Lcd_Write_Data(int DH)	//数据	
{
    LCD_RS=1;
	LCD_CS =0;	  				
	LCD_DataPortH=DH>>8;	
	LCD_DataPortL=DH;					
	LCD_WR=0;
	LCD_WR=1;
	LCD_CS =1;	
}


void Lcd_Write_Com_Data( int com1,dat1)	//命令数据一起 
{
   Lcd_Write_Com(com1);
   Lcd_Write_Data(dat1);
}



void Address_set(unsigned int x1,unsigned int y1,unsigned int x2,unsigned int y2)
{
	Lcd_Write_Com_Data(0x0044,(x2<<8)+x1);
	Lcd_Write_Com_Data(0x0045,y1);
	Lcd_Write_Com_Data(0x0046,y2);
	Lcd_Write_Com_Data(0x004e,x1);
	Lcd_Write_Com_Data(0x004f,y1);
    Lcd_Write_Com(0x0022);	   
}	
void Lcd_Init(void)
{
    LCD_RD=1;
    LCD_REST=1;
    delayms(5);	
	LCD_REST=0;
	delayms(20);
	LCD_REST=1;
	LCD_CS=1;
	LCD_RD=1;
	LCD_WR=1;
	delayms(40);

Lcd_Write_Com_Data(0x0000,0x0001);    delayms(1);  //打开晶振
    Lcd_Write_Com_Data(0x0003,0xA8A4);    delayms(1);   //0xA8A4
    Lcd_Write_Com_Data(0x000C,0x0000);    delayms(1);   
    Lcd_Write_Com_Data(0x000D,0x080C);    delayms(1);   
    Lcd_Write_Com_Data(0x000E,0x2B00);    delayms(1);   
    Lcd_Write_Com_Data(0x001E,0x00B0);    delayms(1);   
    Lcd_Write_Com_Data(0x0001,0x2B3F);    delayms(1);   //驱动输出控制320*240  0x6B3F
    Lcd_Write_Com_Data(0x0002,0x0600);    delayms(1);
    Lcd_Write_Com_Data(0x0010,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0011,0x6070);    delayms(1);        //0x4030           //定义数据格式  16位色 
    Lcd_Write_Com_Data(0x0005,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0006,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0016,0xEF1C);    delayms(1);
    Lcd_Write_Com_Data(0x0017,0x0003);    delayms(1);
    Lcd_Write_Com_Data(0x0007,0x0233);    delayms(1);        //0x0233       
    Lcd_Write_Com_Data(0x000B,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x000F,0x0000);    delayms(1);        //扫描开始地址
    Lcd_Write_Com_Data(0x0041,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0042,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0048,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0049,0x013F);    delayms(1);
    Lcd_Write_Com_Data(0x004A,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x004B,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0044,0xEF00);    delayms(1);
    Lcd_Write_Com_Data(0x0045,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0046,0x013F);    delayms(1);
    Lcd_Write_Com_Data(0x0030,0x0707);    delayms(1);
    Lcd_Write_Com_Data(0x0031,0x0204);    delayms(1);
    Lcd_Write_Com_Data(0x0032,0x0204);    delayms(1);
    Lcd_Write_Com_Data(0x0033,0x0502);    delayms(1);
    Lcd_Write_Com_Data(0x0034,0x0507);    delayms(1);
    Lcd_Write_Com_Data(0x0035,0x0204);    delayms(1);
    Lcd_Write_Com_Data(0x0036,0x0204);    delayms(1);
    Lcd_Write_Com_Data(0x0037,0x0502);    delayms(1);
    Lcd_Write_Com_Data(0x003A,0x0302);    delayms(1);
    Lcd_Write_Com_Data(0x003B,0x0302);    delayms(1);
    Lcd_Write_Com_Data(0x0023,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0024,0x0000);    delayms(1);
    Lcd_Write_Com_Data(0x0025,0x8000);    delayms(1);
    Lcd_Write_Com_Data(0x004f,0);        //行首址0
    Lcd_Write_Com_Data(0x004e,0);        //列首址0
	Lcd_Write_Com(0x0022);

}


void Pant(unsigned int color)
{
	int i,j;
	Address_set(0,0,239,319);

    for(i=0;i<320;i++)
	 {
	  for (j=0;j<240;j++)
	   	{
         Lcd_Write_Data(color);
	    }

	  }		
}
void inttostr(int dd,unsigned char *str)
{
	str[0]=dd/10000+48;
	str[1]=(dd/1000)-((dd/10000)*10)+48;
	str[2]=(dd/100)-((dd/1000)*10)+48;
	str[3]=(dd/10)-((dd/100)*10)+48;
	str[4]=dd-((dd/10)*10)+48;
	str[5]=0;
}
void AD7843(void)              //外部中断0 用来接受键盘发来的数据
{
//delayms(1);                     //中断后延时以消除抖动，使得采样数据更准确
//spistar();                       //启动SPI
//while(BUSY);                //如果BUSY信号不好使可以删除不用
//delayms(1);
CS=0;
WriteCharTo7843(0x90);        //送控制字 10010000 即用差分方式读X坐标 详细请见有关资料
//while(BUSY);               //如果BUSY信号不好使可以删除不用
//delayms(1);
DCLK=1; _nop_();_nop_();_nop_();_nop_();
DCLK=0; _nop_();_nop_();_nop_();_nop_();
TP_Y=ReadFromCharFrom7843();
WriteCharTo7843(0xD0);       //送控制字 11010000 即用差分方式读Y坐标 详细请见有关资料
DCLK=1; _nop_();_nop_();_nop_();_nop_();
DCLK=0; _nop_();_nop_();_nop_();_nop_();
TP_X=ReadFromCharFrom7843();
CS=1;
}

//在指定位置显示一个字符(8*12大小)
//dcolor为内容颜色，gbcolor为背静颜色
void showzifu(unsigned int x,unsigned int y,unsigned char value,unsigned int dcolor,unsigned int bgcolor)	
{  
	unsigned char i,j;
	unsigned char *temp=zifu;    
    Address_set(x,y,x+7,y+11); //设置区域      
	temp+=(value-32)*12;
	for(j=0;j<12;j++)
	{
		for(i=0;i<8;i++)
		{ 		     
		 	if((*temp&(1<<(7-i)))!=0)
			{
				Lcd_Write_Data(dcolor);
			} 
			else
			{
				Lcd_Write_Data(bgcolor);
			}   
		}
		temp++;
	 }
}
//在指定位置显示一个字符串(8*12大小)
//dcolor为内容颜色，gbcolor为背静颜色
void showzifustr(unsigned int x,unsigned int y,unsigned char *str,unsigned int dcolor,unsigned int bgcolor)	  
{  
	unsigned int x1,y1;
	x1=x;
	y1=y;
	while(*str!='\0')
	{	
		showzifu(x1,y1,*str,dcolor,bgcolor);
		x1+=7;
		str++;
	}	
}

main()
{

	unsigned char ss[6];	
	unsigned int lx,ly;
	spistar();  //模拟spi初始化
	Lcd_Init();   //tft初始化	 
	Pant(0xffff);
	showzifustr(68,5,"VERY GOOD",0,0xffff);	//显示字符串 
	while(1)
	{ 
	 	if (Penirq==0)
		{  	
			AD7843();
			inttostr(TP_X,ss);
			showzifustr(10,305,"X:",0xf800,0xffff);
			showzifustr(25,305,ss,0xf800,0xffff);	//显示字符串 
			inttostr(TP_Y,ss);
			showzifustr(80,305,"Y:",0xf800,0xffff);
			showzifustr(95,305,ss,0xf800,0xffff);	//显示字符串 
			lx=((TP_X-220)/16);
			ly=320-((TP_Y-400)/11);
			Address_set(lx,ly,lx+2,ly+2);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);
			Lcd_Write_Data(255);

		//	delayms(100);

		}
    }



}













