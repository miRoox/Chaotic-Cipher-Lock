#ifndef __I2C_H_
#define __I2C_H_

#include<reg51.h>

//---�ض���ؼ���---//
#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

//--����ʹ�õ�IO��--//
sbit I2C_SCL = P2^1;
sbit I2C_SDA = P2^0;

//--����ԭ��--//
void I2C_Delay10us();
void I2C_Start();
void I2C_Stop();
uchar I2C_SendByte(uchar dat, uchar ack);
uchar I2C_ReadByte();

void At24c02Write(unsigned char addr,unsigned char dat);
unsigned char At24c02Read(unsigned char addr);
void saveAt24c02(const unsigned char *pt,const int offset,const int size);
void loadAt24c02(unsigned char *pt,const int offset,const int size);

#endif