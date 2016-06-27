#ifndef __CCL_
#define __CCL_

#include<reg51.h>
#include<intrins.h>

#define GPIO_KEY P1 //������� P1^0-JP4^8

sbit bee=P2^2;  //������
sbit RELAY=P2^3; //�̵���
sbit GLED=P2^4; //�̵�

#if defined __LCD_1602_
	#define LCD1602_DATAPINS P0
	sbit LCD1602_E=P2^7;
	sbit LCD1602_RW=P2^5;
	sbit LCD1602_RS=P2^6;
#else
	#define GPIO_DIG P0 //����ܶ�ѡ P0^0-J12^8
	sbit LSA=P2^5; //�����λѡ
	sbit LSB=P2^6;
	sbit LSC=P2^7;
	//--����ȫ�ֱ���--//
	unsigned char code DIG_CODE[16]={
		0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,
		0x7f,0x6f,0x77,0x7c,0x39,0x5e,0x79,0x71};
	//0��1��2��3��4��5��6��7��8��9��A��b��C��d��E��F����ʾ��
	unsigned char xdata DisplayData[8];
	//�������Ҫ��ʾ��8λ����ֵ
#endif

// - model -
#define MD_NORM 0x00 //��ͨ����������
#define MD_ORGN 0x01 //����ԭ����
#define MD_NEWP 0x02 //����������
#define MD_CONF 0x03 //ȷ��������
#define MD_COMI 0x04 //��������

#define MD_SHOW 0x40 //��ʾ����
#define MD_ERRO 0x80 //����

// - keyval -
#define K_CONF 16 //ȷ�ϼ�
#define K_BACK 12 //���ؼ�
#define K_SHOW 8  //��ʾ����
#define K_CHAN 4  //�޸�����
#define K_CLS 15  //��ձ�������
#define K_DEL 14  //ɾ�����˸�

typedef struct {
	char mdata;
	char param; /*������Ϣ��λ,�ɵ͵��ߣ�*/
}MSG;
#define MG_REPAINT 0x01 /*�ػ�*/
#define MG_NUMBER  0x02 /*�����ַ�*/
#define MG_CONTROL 0x04 /*���Ʒ�*/


// - ����ԭ�� -
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