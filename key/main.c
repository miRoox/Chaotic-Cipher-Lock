#include<reg51.h>
#include<stdlib.h>

//#define IN 0x10

#define RAND_MAX 32767
#define INITIAL (srand(atoi(__TIME__)),(float)rand()/RAND_MAX)

sbit bee=P1^5;
sbit RS485E=P3^7;   //RS485的使能脚
unsigned char data iflag;

void UsartConfig();
void Delay1ms(unsigned int c);
void SendByte(unsigned char dat);
float logistic(float x);
void Beep();


main()
{
	char xdata dig=0;
	float xdata f=INITIAL;
	UsartConfig();
	/*while(1){
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
	}*/
	RS485E=1; // RS5485E=0为接收状态  RS5485E=1为发送状态
	while(~(iflag&7)){
		Delay1ms(400);
		if(0==dig){
			f=logistic(f);
		}
		SendByte(*((char *)(&f)+dig++));
		Beep();
		if(sizeof(float)==dig){
			dig=0;
			iflag++;
			Delay1ms(600);
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
	for(j=0;j<255;j++){
		for(k=0;k<20;k++){
			bee=0;
		}
		for(k=0;k<20;k++){
			bee=1;
		}
	}
}

/*void RecieveData(void) interrupt 4 using 1
{
	if(1==RI){
		iflag |= IN;
		RI=0; //清除RI接受中断标志
	}
}*/

void SendByte(unsigned char dat)
{
	//RS485E=1; 	  // RS5485E=0为接收状态  RS5485E=1为发送状态
	SBUF=dat;	   //发送数据
	while(!TI);		  //等待发送数据完成
	TI=0;			  //清除发送完成标志位
	Delay1ms(1);
	//RS485E=0;
}

const float code k=3.983271;
float logistic(float x)
{
	return k*x*(1-x);
}