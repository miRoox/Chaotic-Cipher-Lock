#include"ccl.h"
#include"i2c.h"  //I2C串行总线相关


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
	return(0);
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
	loadAt24c02((unsigned char *)Ppassword,0,sizeof(long));
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
				NumberDig(buf,10);
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

void operatError(char msgprm)
{
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

void operatConfirm(char *pmodel,char *pdigit,long *pbuf,long *Ppassword)
{
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
				saveAt24c02((unsigned char *)Ppassword,0,sizeof(long));
				tmppw=-1;
				*pmodel &= MD_NORM;
			}
		break;
	}
	*pdigit=0;
	*pbuf=0;
}

void NumberDig(long num,const unsigned char ary)
{
	char i;
	for(i=0;i<8;i++,num/=ary){
		DisplayData[i]=DIG_CODE[num%ary];
	}
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
	return(KeyValue);
}

void DigDisplay(unsigned char t)
{
	unsigned char i ,j;
	for(i=0;i<t;++i){
		LSA=~i&0x01;LSB=(~i&0x02)>>1;LSC=(~i&0x04)>>2;//位选，选择点亮的数码管
		GPIO_DIG=DisplayData[i];//发送段码
		j=80;					 //扫描间隔时间设定
		while(--j);	
		GPIO_DIG=0x00;//消隐
	}
}

void Delay1ms(unsigned int c)
{
    unsigned char a, b;
    for(;c>0;c--)
	{
		for(b=199;b>0;b--)
		{
		  	for(a=1;a>0;a--);
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
