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

#ifndef __LCD_H_
#define __LCD_H_

//#define LCD1602_4PINS //开了这个后下载完必须要重启（复位不行）
//#define LCD1602_10ROWS

#include<reg51.h>
#include<string.h>

#define LCD1602_DATAPINS P0
sbit LCD1602_E=P2^7;
sbit LCD1602_RW=P2^5;
sbit LCD1602_RS=P2^6;

// - 基本操作 (读/写~) -
#define OP_COMD 0 /*命令*/
#define OP_DATA 1 /*数据*/

// - 命令集 -
#define CMD_CLS 0x01	/* 清屏 - DB0 */
#define CMD_CURES 0x02	/* 光标归位 - DB1 */
/* 显示开关控制指令 - DB3 */
#define CMD_SCOFF 0x08	/*关显示*/
#define CMD_NOCUR 0x0c	/*无光标*/
#define CMD_CURFLS 0x0e	/*有光标闪烁*/
#define CMD_CURSTC 0x0f	/*有光标不闪烁*/
/* 进入模式设置指令 - DB2 */
#define CMD_CURLF 0x04	/*写入新数据后光标左移模式*/
#define CMD_CURRT 0x06	/*写入新数据后光标右移模式*/
#define CMD_SCMOV 0x05	/*写入新数据后显示屏整体移动1个字(应与上面这两个(通过位或)结合使用)*/
/* 功能设定指令(通过位与组合使用) - DB5 */ 
#define CMD_8PINS 0x30	/*数据总线为8位,def:数据总线为4位*/
#define CMD_2LINE 0x28	/*显示2行,def:显示1行*/
#define CMD_10ROW 0x24	/*每字符5×10点阵,def:每字符5×8点阵*/
/* 设定显示屏或光标移动方向指令 -DB4 */
#define CMD_SCRMV 0x1c	/*整屏右移*/
#define CMD_SCLMV 0x18	/*整屏左移*/
#define CMD_CRRMV 0x14	/*光标右移1格*/
#define CMD_CRLMV 0x10	/*光标左移1格*/
/* 设定CGRAM地址指令 - DB6 */
#define CGRAMADD(x) (((x)&0x3f)|0x40)
/* 设置DDRAM地址指令 - DB7 */
#define DDRAMADD(x) (((x)&0x7f)|0x80)

#ifndef LCD1602_10ROWS
#define CHROW 2
#define SCDLINE 0x40 //第二行开始的地址
char xdata buffer[0x80];
#else 
#define CHROW 1
char xdata buffer[0x40];
#endif
#define CHCOL 40
#define SCCOL 16

#define DRC_LEFT 0
#define DRC_RIGH 1

struct {
	char ac;   /*当前ac计数器的值*/
	char * base;  /*当前页的基址*/
	//char page;  /*所处的页*/
	//...
} xdata lcd;//目前不支持换页 :(


// - 函数原型 -
extern void Delay1ms(unsigned int c);//in io.c
static void LcdWrite(bit op,unsigned char dat);
static char LcdRead(bit op);
void LcdInit();
void LcdCursorShow(bit set);
void LcdCls(void);
void LcdLineFeed(void);
void LcdBackspace(void);
void LcdDelete(void);
void LcdPutchar(char c);
void LcdCursorMov(bit drc);

#ifdef LCD1602_4PINS
	#define HI4BITS(x) ((x)&0xf0)
	#define LO4BITS(x) ((x)<<4)
#endif

#endif