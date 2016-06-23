#include<reg51.h>
#include<intrins.h>
#include"ccl.h"
#include"i2c.h"  //I2C串行总线相关

//--定义全局变量--//
unsigned char code DIG_CODE[17]={
0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F的显示码
unsigned char DisplayData[8];
//用来存放要显示的8位数的值


main()
{
	long password;
	char model=MD_NORM; //见ccl.h
	initialize(&password);
	while(1){
		MSG msg={0,0};
		inputModu(&model,&msg);
		operatModu(&model,msg,&password);
	}
	return 0;
}

void initialize(long *Ppassword)
{
	RELAY=1;
	DisplayData[0]=0x00;
	DisplayData[1]=0x00;
	DisplayData[2]=0x00;
	DisplayData[3]=0x00;
	DisplayData[4]=0x00;
	DisplayData[5]=0x00;
	DisplayData[6]=0x00;
	DisplayData[7]=0x00;
	DigDisplay(8);
	readPassword(Ppassword);
}

void inputModu(char *pmodel,MSG *pmsg)
{
	static char premodel=1;
	if((*pmodel)&MD_ERRO){
		if(K_BACK==KeyScan()){
			*pmodel &= ~MD_ERRO;
		}
	}
	else if((*pmodel)&MD_COMI){
		
	}
	else {
		char KeyVal=KeyScan();
		if(KeyVal){
			if((KeyVal&0x03)==0){
				pmsg->param |= MG_CONTROL;
				pmsg->mdata=KeyVal>>2;
				/*if(KeyVal==K_SHOW){
					pmsg->param &= ~MG_REPAINT;
				}*/
			}
			else if(KeyVal==K_CLS || KeyVal==K_DEL){
				pmsg->param |= MG_CONTROL;
				pmsg->mdata=KeyVal-9;
			}
			else {
				pmsg->param |= MG_NUMBER;
				pmsg->mdata=(KeyVal-(KeyVal>>2))%10;
			}
		}
	}
	if(premodel!=*pmodel){
		pmsg->param |= MG_REPAINT;
	}
	premodel=*pmodel;
}

void operatModu(char *pmodel,MSG msg,long *Ppassword)
{
	static char digit=0;
	static long buf=0;
	if((*pmodel)&MD_ERRO){
		operatError(msg.param);
	}
	else if((*pmodel)&MD_COMI){
		
	}
	else {
		if(msg.param&MG_CONTROL){
			switch(msg.mdata){
				case(K_CHAN/4): 
					if(((*pmodel)&MD_CONF)==0){
						*pmodel |= MD_ORGN;
					}
				break;
				case(K_SHOW/4): *pmodel |= MD_SHOW; break;
				case(K_BACK/4): *pmodel &= MD_NORM; break;
				case(K_CONF/4): operatConfirm(pmodel,&digit,&buf,Ppassword);	break;
				case(K_DEL-9): 	
					if(digit>0){
						--digit; buf/=10;
					}
				break;
				case(K_CLS-9):  digit=0; buf=0; break;
			}
		}
		else if(msg.param&MG_NUMBER){
			if(digit<8){
				++digit;
				buf=buf*10+msg.mdata;
			}
		}
		if(!(msg.param&MG_CONTROL)||(msg.mdata!=K_SHOW/4)){
			(*pmodel) &= ~MD_SHOW;
		}
		if(msg.param&MG_REPAINT){
			if((*pmodel)&MD_SHOW){
				DisplayData[0]=DIG_CODE[buf%10];
				DisplayData[1]=DIG_CODE[buf%100/10];
				DisplayData[2]=DIG_CODE[buf%1000/100];
				DisplayData[3]=DIG_CODE[buf%10000/1000];
				DisplayData[4]=DIG_CODE[buf%100000/10000];
				DisplayData[5]=DIG_CODE[buf%1000000/100000];
				DisplayData[6]=DIG_CODE[buf%10000000/1000000];
				DisplayData[7]=DIG_CODE[buf%100000000/10000000];
			}
			else {
				DisplayData[0]=0x40; //"-"
				DisplayData[1]=0x40;
				DisplayData[2]=0x40;
				DisplayData[3]=0x40;
				DisplayData[4]=0x40;
				DisplayData[5]=0x40;
				DisplayData[6]=0x40;
				DisplayData[7]=0x40;
			}
		}
		DigDisplay(digit);
	}
}

void operatError(char msgprm){
	if(msgprm & MG_REPAINT){
		DisplayData[0]=0x00;
		DisplayData[1]=0x50;
		DisplayData[2]=0x5c;
		DisplayData[3]=0x50;
		DisplayData[4]=0x50;
		DisplayData[5]=0x79;
		DisplayData[6]=0x00;
		DisplayData[7]=0x00;  /*"  Error "*/
	}
	Beep();
	DigDisplay(8);
}

void operatConfirm(char *pmodel,char *pdigit,long *pbuf,long *Ppassword){
	static long tmppw=-1;
	switch((*pmodel)&MD_CONF){
		case(MD_NORM):
			if((*pdigit<8)||(*pbuf!=*Ppassword)){
				*pmodel |= MD_ERRO;
			}
			else {
				GLED=0; //绿指示灯亮
				RELAY=0;
			//	*pmodel |= MD_COMI;
			}
		break;
		case(MD_ORGN):
			if((*pdigit<8)||(*pbuf!=*Ppassword)){
				*pmodel |= MD_ERRO;
			}
			else {
				++(*pmodel);
			}
		break;
		case(MD_NEWP):
			if(*pdigit<8){
				*pmodel |= MD_ERRO;
			}
			else {
				tmppw=*pbuf;
				++(*pmodel);
			}
		break;
		case(MD_CONF):
			if((*pdigit<8)||(*pbuf!=tmppw)){
				*pmodel |= MD_ERRO;
			}
			else {
				*Ppassword=tmppw;
				savePassword(Ppassword);
				tmppw=-1;
				*pmodel &= MD_NORM;
			}
		break;
	}
	*pdigit=0;
	*pbuf=0;
}

/*************************
矩阵键盘按键分布连接示意图
	1-	S1	S2	S3	S4
	2-	S5	S6	S7	S8
	3-	S9	S10	S11	S12
	4-	S13	S14	S15	S16
		|	|	|	|
	P2	5	6	7	8
**************************/
char KeyScan()
{
	char KeyValue=0;
	GPIO_KEY=0xf0;
	if(GPIO_KEY!=0xf0)
  	{
		Delay1ms(10);
		if(GPIO_KEY!=0xf0)
		{
			unsigned int a=0;
			//测试行
			GPIO_KEY=0Xf0;
			switch(GPIO_KEY)
			{
				case(0X70):	KeyValue=1;break;
				case(0Xb0):	KeyValue=5;break;
				case(0Xd0): KeyValue=9;break;
				case(0Xe0):	KeyValue=13;break;
				default:	return(0);	 //跳过错误操作
			}
			//测试列
			GPIO_KEY=0X0f;
			switch(GPIO_KEY)
			{
				case(0X0e):	KeyValue=KeyValue+3;break;
				case(0X0d):	KeyValue=KeyValue+2;break;
				case(0X0b): KeyValue=KeyValue+1;break;
				case(0X07):	KeyValue=KeyValue;break;
				default:	return(0);  //跳过错误操作
			}
			if(KeyValue!=K_SHOW){
				while((a<10)&&(GPIO_KEY!=0x0f))	 //按键松手检测
				{
					Delay1ms(50);
					++a;
				}
			}
		}
	}
	return KeyValue;
}

void DigDisplay(unsigned char t)
{
	unsigned char i ,j;
	for(i=0;i<t;++i){
		switch(i)	 //位选，选择点亮的数码管，
		{
			case(0):
				LSA=1;LSB=1;LSC=1; break;//显示第0位
			case(1):
				LSA=0;LSB=1;LSC=1; break;//显示第1位
			case(2):
				LSA=1;LSB=0;LSC=1; break;//显示第2位
			case(3):
				LSA=0;LSB=0;LSC=1; break;//显示第3位
			case(4):
				LSA=1;LSB=1;LSC=0; break;//显示第4位
			case(5):
				LSA=0;LSB=1;LSC=0; break;//显示第5位
			case(6):
				LSA=1;LSB=0;LSC=0; break;//显示第6位
			case(7):
				LSA=0;LSB=0;LSC=0; break;//显示第7位	
		}
		GPIO_DIG=DisplayData[i];//发送段码
		j=50;					 //扫描间隔时间设定
		while(--j);	
		GPIO_DIG=0x00;//消隐
	}
}

void Delay1ms(unsigned int c)
{
    unsigned char a, b;
    for (;c>0;c--)
	{
		for (b=38;b>0;b--)
		{
			for (a=13;a>0;a--);
		}          
	}       
}

void Beep()
{
	unsigned char j, k;
	for(j=0;j<10;j++){
		for(k=0;k<20;k++){
			bee=0;
		}
		for(k=0;k<20;k++){
			bee=1;
		}
	}
}


void savePassword(long *Ppassword)
{
	uchar num0,num1,num2,num3;
	num0=*((uchar *)(Ppassword));
	num1=*((uchar *)(Ppassword)+1);
	num2=*((uchar *)(Ppassword)+2);
	num3=*((uchar *)(Ppassword)+3);
	At24c02Write(0,num0);
	Delay1ms(50);  //间隔也许可以更小一点，但不能没有
	At24c02Write(1,num1);
	Delay1ms(50);
	At24c02Write(2,num2);
	Delay1ms(50);
	At24c02Write(3,num3);
	Delay1ms(50);
}

void readPassword(long *Ppassword)
{
	uchar *p=(uchar *)(Ppassword);//p现在指向的是password的第一个字节
	*p=At24c02Read(0);
	p++;
	*p=At24c02Read(1);
	p++;
	*p=At24c02Read(2);
	p++;
	*p=At24c02Read(3);
}