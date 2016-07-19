#include<reg51.h>
//#include"lcd.h"

#define IN 0x10

sbit bee=P1^5;
sbit RS485E=P3^7;   //RS485的使能脚
char data iflag;

void UsartConfig();
void Delay1ms(unsigned int c);
void DatProc(unsigned char dat);
void SendByte(unsigned char dat);
float logistic(float x);
void Beep();

void Printn(char num);//for test
extern void LcdInit();
extern void LcdPutchar(char c);

char code DIG[17]="0123456789ABCDEF";

main()
{
	LcdInit();
	UsartConfig();
	while(1){
		if(iflag&IN){
			unsigned char xdata idat=SBUF;
			REN=0;
			Beep();
			Printn(idat);
			DatProc(idat);
			REN=1;
			iflag &= ~IN;
		}
		else {
			RS485E=0; // RS5485E=0为接收状态  RS5485E=1为发送状态
			Delay1ms(32);
		}
	}
	return(0);
}

void UsartConfig()
{
	SCON = 0x50;			//REN=1 SM1=1 ,设置为工作方式1
	TMOD |= 0x20;			//设置计数器工作方式2(8位自动重装定时器)
	PCON |= 0x80;			// SCOM=1 ,波特率加倍
	TH1  = 0xFD;
	TR1  = 1;         //打开计数器                                            
	ES   = 1;        //开串口中断                  
	EA   = 1;        // 开总中断 
}

void Delay1ms(unsigned int c)   
{
    unsigned char a,b;
	for (; c>0; c--)
	{
		for (b=199;b>0;b--)
		{
			for(a=1;a>0;a--);
		}      
	}
}

void Beep()
{
	unsigned char j, k;
	//EA=0;//防止重入
	for(j=0;j<255;j++){
		for(k=0;k<20;k++){
			bee=0;
		}
		for(k=0;k<20;k++){
			bee=1;
		}
	}
	//EA=1;
}

void RecieveData(void) interrupt 4 using 1
{
	if(1==RI){
		iflag |= IN;
		RI=0; //清除RI接受中断标志
	}
}

void DatProc(unsigned char idat)
{
	static char dig=0;
	static float xdata f=0.4117583;
	if(0xff==idat){
		if(0==dig){
			f=logistic(f);
		}
		Printn(*((char *)(&f)+dig));
		SendByte(*((char *)(&f)+dig++));
		Printn(dig);
		if(sizeof(float)==dig){
			dig=0;
			iflag++;
		}
	}
	else {
		Printn(*((char *)(&f)+dig));
		SendByte(*((char *)(&f)+dig));
	}
}

void SendByte(unsigned char dat)
{
	RS485E=1; 	  // RS5485E=0为接收状态  RS5485E=1为发送状态
	SBUF=dat;	   //发送数据
	while(!TI);		  //等待发送数据完成
	TI=0;			  //清除发送完成标志位
	Delay1ms(1);
	RS485E=0;
}

const float code k=3.983271;
float logistic(float x)
{
	return k*x*(1-x);
}

void Printn(char num)
{
	LcdPutchar(DIG[(num>>4)&0x0f]);
	LcdPutchar(DIG[num&0x0f]);
}