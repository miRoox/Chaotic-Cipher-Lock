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

#ifndef __IO_H_
#define __IO_H_

#include<reg51.h>
#include<absacc.h>
#include<stdio.h>
#include<stdarg.h>
#include<stdlib.h>
#include<string.h>
//#include"lcd.h"
//#include"i2c.h"
#include"errno.h"

#define SOL 12 /*晶振频率12MHz*/

#define GPIO_KEY P1 //矩阵键盘 P1^0-JP4^8
sbit bee=P2^2;  //蜂鸣器
sbit RELAY=P2^3; //继电器
sbit RS485E=P3^7;   //RS485的使能脚

sfr ISP_CONTR=0xe7;


#define ON  1
#define OFF 0
#define DEF (-1)

/* TMOD */
#define GATE0 0x08
#define GATE1 0x80
#define CONT0 0x04 /* 相反则为定时器 */
#define CONT1 0x40
#define M13BT0 0x00 /* 13位定时/计数器 */
#define M13BT1 0x00
#define M16BT0 0x01 /* 16位定时/计数器 */
#define M16BT1 0x10
#define M8BTA0 0x02 /* 8位自动重装定时/计数器 */
#define M8BTA1 0x20
#define M8BTSP 0x03 /* T0分成两个独立的8位定时/计数器 */

#define LOBYTE(x) ((char)(x))
#define HIBYTE(x) ((char)((x)>>8))

#define T8BH(x)  HIBYTE(0x100-(x)/(12/SOL))
#define T8BL(x)  LOBYTE(0x100-(x)/(12/SOL))
#define T13BH(x) HIBYTE(0x2000-(x)/(12/SOL))
#define T13BL(x) LOBYTE(0x2000-(x)/(12/SOL))
#define T16BH(x) HIBYTE(0x10000-(x)/(12/SOL))
#define T16BL(x) LOBYTE(0x10000-(x)/(12/SOL))

/*  */

extern unsigned char data inputset;
#define IS_KEY 0x40  //键盘
#define IS_COM 0x10  //串口
#define IS_CMI 0x20  //串口数据输入

// - keyval -
#define K_CONF 16 //确认键
#define K_BACK 12 //返回键
#define K_SHOW 8  //显示密码
#define K_CHAN 4  //修改密码
#define K_CLS 15  //清空本次输入
#define K_DEL 14  //删除，退格


typedef struct _MSG {
	unsigned char message;
	unsigned char param;
	struct _MSG *next;
} MSG, *MSGQ, *lpMSG;
extern volatile MSG xdata * msgHead;
#define MTAG_TRANS 0x80
#define MG_QUIT    0
#define MG_CREAT   1
#define MG_ERROR   2
#define MG_KEYDOWN 3
#define MG_COMMAND 4
#define MG_CHAR    5
#define MG_CHNECHO 6
#define MG_RCDAT   7
#define MG_SDDAT   8

// - command -
#define OPR_OPEN 0
#define OPR_CONF 1
#define OPR_BACK 2
#define OPR_CHAN 3
#define OPR_CLS 4
#define OPR_DEL 5
#define OPR_COMON 6
#define OPR_COMOF 7


#define BUFSIZE 80

void Reset(void);
void Beep();
void Delay1ms(unsigned int c);

extern void At24c02Write(unsigned char addr,unsigned char dat);//in i2c.c
extern unsigned char At24c02Read(unsigned char addr);
void write(const unsigned char *pt,const int addr,const int size);
void read(unsigned char *pt,const int addr,const int size);

extern void LcdInit();
extern void LcdCursorShow(bit set);
extern void LcdCls(void);
extern void LcdLineFeed(void);
extern void LcdBackspace(void);
extern void LcdDelete(void);
extern void LcdPutchar(char c);
extern void LcdCursorMov(bit drc);//in lcd.c
void NoBufferPrint(const char *str);
void error(unsigned int errno);
int myPrintf(const char *fmt, ...);

static char KeyScan();
static void InputScanner();

void InitMsg();
void PostMsg(unsigned char message,unsigned char param);
void PushMsg(unsigned char message,unsigned char param);
void RemoveMsg(lpMSG pmsg);
void ClearMsg();
char GetMsg();
char TranslateMsg(const MSG *pmsg);
extern char MsgProc(unsigned char message,unsigned char param);

char set_echo(char c,char cnt,char *buf);
char echo(char c);

void UsartConfig();
//void SendByte(unsigned char dat);

#endif