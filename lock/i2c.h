#ifndef __I2C_H_
#define __I2C_H_

#include<reg51.h>

//---重定义关键词---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--定义使用的IO口--//
sbit I2C_SCL = P2^1;
sbit I2C_SDA = P2^0;

//--函数原型--//
static void I2C_Delay10us();
static void I2C_Start();
static void I2C_Stop();
static uchar I2C_SendByte(uchar dat, uchar ack);
static uchar I2C_ReadByte();

void At24c02Write(unsigned char addr,unsigned char dat);
unsigned char At24c02Read(unsigned char addr);

#endif