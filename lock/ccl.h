#ifndef __CCL_
#define __CCL_

#include<reg51.h>
#include<intrins.h>

#define GPIO_KEY P1 //矩阵键盘 P1^0-JP4^8

sbit bee=P2^2;  //蜂鸣器
sbit RELAY=P2^3; //继电器
sbit GLED=P2^4; //绿灯

#if defined __LCD_1602_
	#define LCD1602_DATAPINS P0
	sbit LCD1602_E=P2^7;
	sbit LCD1602_RW=P2^5;
	sbit LCD1602_RS=P2^6;
#else
	#define GPIO_DIG P0 //数码管段选 P0^0-J12^8
	sbit LSA=P2^5; //数码管位选
	sbit LSB=P2^6;
	sbit LSC=P2^7;
	//--定义全局变量--//
	unsigned char code DIG_CODE[16]={
		0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
		0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
	//0、1、2、3、4、5、6、7、8、9、A、b、C、d、E、F的显示码
	unsigned char xdata DisplayData[8];
	//用来存放要显示的8位数的值
#endif

// - model -
#define MD_NORM 0x00 //普通，键入密码
#define MD_ORGN 0x01 //输入原密码
#define MD_NEWP 0x02 //输入新密码
#define MD_CONF 0x03 //确认新密码
#define MD_COMI 0x04 //串口输入

#define MD_SHOW 0x40 //显示密码
#define MD_ERRO 0x80 //错误

// - keyval -
#define K_CONF 16 //确认键
#define K_BACK 12 //返回键
#define K_SHOW 8  //显示密码
#define K_CHAN 4  //修改密码
#define K_CLS 15  //清空本次输入
#define K_DEL 14  //删除，退格

typedef struct {
	char mdata;
	char param; /*附加信息（位,由低到高）*/
}MSG;
#define MG_REPAINT 0x01 /*重绘*/
#define MG_NUMBER  0x02 /*数字字符*/
#define MG_CONTROL 0x04 /*控制符*/


// - 函数原型 -
void initialize(long *Ppassword);

void inputModu(char *pmodel,MSG *pmsg);
char KeyScan();

void operatModu(char *pmodel,MSG msg,long *Ppassword);
void operatError(char msgprm);
void operatConfirm(char *pmodel,char *pdigit,long *pbuf,long *Ppassword);
void DigDisplay(unsigned char t);
void NumberDig(long num,const unsigned char ary);

void Delay1ms(unsigned int c);
void Beep();

#endif