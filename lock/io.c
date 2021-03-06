/**
 ** Copyright 2017-2018 Lu <miroox@outlook.com>
 **
 ** Licensed under the Apache License, Version 2.0 (the "License");
 ** you may not use this file except in compliance with the License.
 ** You may obtain a copy of the License at
 **
 **     http://www.apache.org/licenses/LICENSE-2.0
 **
 ** Unless required by applicable law or agreed to in writing, software
 ** distributed under the License is distributed on an "AS IS" BASIS,
 ** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 ** See the License for the specific language governing permissions and
 ** limitations under the License.
 **/

#include"io.h"

void Reset(void)
{
	LcdCls();
	NoBufferPrint("Rebooting...");
	Delay1ms(1000);
	ISP_CONTR |= 0x20;//软复位
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
	for(j=0;j<50;j++){
		for(k=0;k<20;k++){
			bee=0;
		}
		for(k=0;k<20;k++){
			bee=1;
		}
	}
}

void write(const unsigned char *pt,const int addr,const int size) 
{
	unsigned char i;
	for(i=addr;i<size;i++){
		At24c02Write(i,*(pt++));
		Delay1ms(10);
	}
}
void read(unsigned char *pt,const int addr,const int size)
{
	unsigned char i;
	for(i=addr;i<size;pt++){
		*pt=At24c02Read(i++);
		Delay1ms(10);
	}
}

void NoBufferPrint(const char *str)
{
	for(;*str && *str!=EOF;str++){
		switch(*str){
			case'\0':break;
			case'\a':Beep();break;
			case'\f':LcdCls();break;
			case'\b':LcdCursorMov(0);break; //#define DRC_LEFT 0
			case'\n':LcdLineFeed();break;
			default:LcdPutchar(*str);
		}
	}
}

void error(unsigned int errno)
{
	LcdCursorShow(OFF);
	NoBufferPrint("\fError:\n");
	switch(errno){
		case(EPERM):NoBufferPrint("Operation not permitted");break;
		case(EINTR):NoBufferPrint("Interrupted system call");break;
		case(EIO):NoBufferPrint("I/O error");break;
		case(E2BIG):NoBufferPrint("Argument list too long");break;
		case(ENOMEM):NoBufferPrint("Out of memory");break;
		case(EFAULT):NoBufferPrint("Bad address");break;
		case(EINVAL):NoBufferPrint("Invalid argument");break;
		case(ENODATA):NoBufferPrint("No data available");break;
		case(ENOBUFS):NoBufferPrint("No buffer space available");break;
		case(EREMOTEIO):NoBufferPrint("Remote I/O error");break;
		case(EPSW):NoBufferPrint("Wrong password");break;
		case(EPSW2):NoBufferPrint("Wrong key to open");break;
		default:NoBufferPrint("Unknown error");break;
	}
}

int myPrintf(const char *fmt, ...)
{
	va_list ap;
	char xdata pbuf[BUFSIZE];
	int len;
	va_start(ap,fmt);
	len=vsprintf(pbuf,fmt,ap);
	va_end(ap);
	if(len>BUFSIZE){
		memset(pbuf,0,len);
		len=EOF;
		PostMsg(MG_ERROR,ENOBUFS);
	}
	else {
		LcdCursorShow(OFF);
		NoBufferPrint(pbuf);
		LcdCursorShow(ON);
	}
	return(len);
}


/*************************
矩阵键盘按键分布连接示意图
	1-	S1	S2	S3	S4
	2-	S5	S6	S7	S8
	3-	S9	S10	S11	S12
	4-	S13	S14	S15	S16
		|	|	|	|
	P1	5	6	7	8
**************************/
static char KeyScan()
{
	char data KeyValue=0;
	GPIO_KEY=0xf0;
	if(GPIO_KEY!=0xf0)
  	{
		Delay1ms(10);
		if(GPIO_KEY!=0xf0)
		{
			unsigned int data a=0;
			//测试行
			GPIO_KEY=0xf0;
			switch(GPIO_KEY)
			{
				case(0x70):	KeyValue=1;break;
				case(0xb0):	KeyValue=5;break;
				case(0xd0): KeyValue=9;break;
				case(0xe0):	KeyValue=13;break;
				default:	return(0);	 //跳过错误操作
			}
			//测试列
			GPIO_KEY=0x0f;
			switch(GPIO_KEY)
			{
				case(0x0e):	KeyValue=KeyValue+3;break;
				case(0x0d):	KeyValue=KeyValue+2;break;
				case(0x0b): KeyValue=KeyValue+1;break;
				case(0x07):	KeyValue=KeyValue;break;
				default:	return(0);  //跳过错误操作
			}
			if(KeyValue!=K_SHOW){
				while((a<10)&&(GPIO_KEY!=0x0f))	 //按键松手检测
				{
					Delay1ms(45);
					++a;
				}
			}
		}
	}
	return(KeyValue);
}

static void InputScanner()
{
	if(inputset&IS_KEY){
		char data keyval=KeyScan();
		if(keyval){
			PostMsg(MG_KEYDOWN,keyval);
		}
	}
	if(inputset&IS_COM){
		if(inputset&IS_CMI){
			char data RDat=SBUF;
			REN=0;
			PostMsg(MG_RCDAT,RDat);
		}
		else {
			RS485E=0; // RS5485E=0为接收状态  RS5485E=1为发送状态
			Delay1ms(1);
		}
	}
}

volatile MSG xdata * msgHead=NULL;
void InitMsg()
{
	if(NULL==msgHead){
		init_mempool(&XBYTE[0x200],40*sizeof(MSG));
		msgHead=(MSGQ)malloc(sizeof(MSG));
		if(NULL==msgHead){
			error(ENOMEM);
			Delay1ms(1000);
			Reset();
		}
		else {
			msgHead->message=MG_CREAT;
			msgHead->param=0;
			msgHead->next=NULL;
		}
	}
}

//向消息队列尾部添加一个消息
void PostMsg(unsigned char message,unsigned char param)
{
	lpMSG p=NULL, pr;
	p=(lpMSG)malloc(sizeof(MSG));
	if(NULL==p){
		error(ENOMEM);
		ClearMsg();
		Delay1ms(1000);
		Reset();
	}
	else {
		if(NULL==msgHead){
			msgHead=p;
		}
		else {
			for(pr=msgHead;pr->next!=NULL;pr=pr->next);
			pr->next=p;
		}
		p->message=message;
		p->param=param;
		p->next=NULL;
	}
}

//直接将消息推到队列的最前
void PushMsg(unsigned char message,unsigned char param)
{
	lpMSG p=NULL;
	p=(lpMSG)malloc(sizeof(MSG));
	if(NULL==p){
		error(ENOMEM);
		ClearMsg();
		Delay1ms(1000);
		Reset();
	}
	else {
		p->message=message;
		p->param=param;
		p->next=msgHead;
		msgHead=p;
	}
}

//删除队列里的一个消息
void RemoveMsg(lpMSG pmsg)
{
	if(NULL==msgHead || NULL==pmsg){
		;
	}
	else {
		lpMSG p=msgHead ,pr=msgHead;
		while( p!=pmsg && p->next!=NULL ){
			pr=p;
			p=p->next;
		}
		if(p==pmsg){
			if(p==msgHead){
				msgHead=p->next;
			}
			else {
				pr->next=p->next;
			}
			free(p);
		}
	}
}

//清空消息队列
void ClearMsg()
{
	lpMSG p=msgHead, pr=NULL;
	while(p!=NULL){
		pr=p;
		p=p->next;
		free(pr);
	}
}

//获取消息，并处理
char GetMsg()
{
	lpMSG pr=NULL;
	char xdata n;
	do{
		InputScanner();
	}
	while(NULL==msgHead);//无消息时阻塞
	pr=msgHead;
	n=TranslateMsg(pr);
	RemoveMsg(pr);
	return(n);
}

char TranslateMsg(const MSG *pmsg)
{
	char xdata n=1;
	if(NULL!=pmsg){
		if(pmsg->message&MTAG_TRANS){
			MsgProc(pmsg->message&~MTAG_TRANS,pmsg->param);
		}
		else {
			switch(pmsg->message){
				case(MG_QUIT):
				ClearMsg();
				n=0;
				break;
				case(MG_ERROR):
				PushMsg(MG_ERROR|MTAG_TRANS,pmsg->param);
				break;
				case(MG_KEYDOWN):
				switch(pmsg->param){
					case(K_CONF):
					PostMsg(MG_COMMAND|MTAG_TRANS,OPR_CONF);
					break;
					case(K_BACK):
					PostMsg(MG_COMMAND|MTAG_TRANS,OPR_BACK);
					break;
					case(K_SHOW):
					{
						static char data flag=0;
						flag|=0x10;
						if(pmsg->next->message!=MG_KEYDOWN || pmsg->next->param!=K_SHOW){
							PushMsg(MG_CHNECHO|MTAG_TRANS,'*');
							flag=0;
						}
						if(flag==0x10){
							PushMsg(MG_CHNECHO|MTAG_TRANS,ON);
						}
						flag=flag>>4;
					}
					break;
					case(K_CHAN):
					PostMsg(MG_COMMAND|MTAG_TRANS,OPR_CHAN);
					break;
					case(K_CLS):
					PostMsg(MG_COMMAND|MTAG_TRANS,OPR_CLS);
					break;
					case(K_DEL):
					PostMsg(MG_COMMAND|MTAG_TRANS,OPR_DEL);
					break;
					default:
					PostMsg(MG_CHAR|MTAG_TRANS,(pmsg->param-(pmsg->param>>2))%10+'0');
				}
				break;
				
				default:
				PostMsg(pmsg->message|MTAG_TRANS,pmsg->param);
			}
		}
	}
	return(n);
}

char set_echo(char c,char cnt,char *buf)
{
	static char xdata echoset='*';
	if(DEF==c){
		return(echoset);
	}
	else {
		for(;cnt;cnt--){
			LcdBackspace();
		}
		if(ON==c){
			for(;buf[cnt];cnt++){
				LcdPutchar(buf[cnt]);
			}
			return(echoset=ON);
		}
		else if(OFF==c){
			return(echoset=OFF);
		}
		else {
			for(;buf[cnt];cnt++){
				LcdPutchar(c);
			}
			return(echoset=c);
		}
	}
}

char echo(char c)
{
	char xdata shc=set_echo(DEF,0,NULL);
	if(ON==shc){
		LcdPutchar(c);
	}
	else if(OFF==shc){
		;
	}
	else {
		LcdPutchar(shc);
	}
	return(shc);
}


void UsartConfig()
{
	SCON = 0X50;			//REN=1 SM1=1 ,设置为工作方式1
	TMOD |= M8BTA1;			//设置计数器工作方式2(8位自动重装定时器)
	PCON |= 0X80;			// SCOM=1 ,波特率加倍
	TH1  = 0xFD;
	TR1  = 1;         //打开计数器                                            
	ES   = 1;        //开串口中断                  
	EA   = 1;        // 开总中断 
}

void RecieveData(void) interrupt 4 using 1
{
	if(1==RI){
		inputset |= IS_COM;
		RI=0; //清除RI接受中断标志
	}
}

/*void SendByte(unsigned char dat)
{
	RS485E=1; 	  // RS5485E=0为接收状态  RS5485E=1为发送状态
	SBUF=dat;	   //发送数据
	while(!TI);		  //等待发送数据完成
	TI=0;			  //清除发送完成标志位
	Delay1ms(1);
	RS485E=0;
}*/