#ifndef __CCL_
#define __CCL_
/** 芯片型号： STC90C516RD+ **/
/******** 接线 ***************
 * P0 口接LCD1602的 DB0~DB7
 * P1 口接矩阵键盘，详见io.c
 * 蜂鸣器接 P2.2
 * 继电器接 P2.3
 * LCD的 RW 接 P2.5
 * LCD的 RS 接 P2.6
 * LCD的 E 接 P2.7
 *****************************/

#include<reg51.h>
#include"io.h"

// - model -
#define MD_NORM 0x00 //普通，键入密码
#define MD_ORGN 0x01 //输入原密码
#define MD_NEWP 0x02 //输入新密码
#define MD_CONF 0x03 //确认新密码

#define PWLEN 16

unsigned char data inputset=0;

// - 函数原型 -
void initialize(char *password);
void operatConfirm(char *pmodel,char *ibuf,char *password);

#endif