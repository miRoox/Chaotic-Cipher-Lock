#ifndef __CCL_
#define __CCL_

#include<reg51.h>
#include<intrins.h>

#define GPIO_DIG P0 //����ܶ�ѡ P0^0-J12^8
#define GPIO_KEY P1 //������� P1^0-JP4^8

sbit bee=P2^5;  //������
sbit RELAY=P2^6; //�̵���
sbit GLED=P2^7; //�̵�
sbit LSA=P2^2; //�����λѡ
sbit LSB=P2^3;
sbit LSC=P2^4;

// - model -
#define MD_NORM 0x00 //��ͨ����������
#define MD_ORGN 0x01 //����ԭ����
#define MD_NEWP 0x02 //����������
#define MD_CONF 0x03 //ȷ��������
#define MD_COMI 0x04 //��������

#define MD_SHOW 0x40 //��ʾ����
#define MD_ERRO 0x80 //����

// - keyval -
#define K_CONF 16
#define K_BACK 12
#define K_SHOW 8
#define K_CHAN 4
#define K_CLS 15
#define K_DEL 14

typedef struct {
	char mdata;
	char param; /*������Ϣ��λ,�ɵ͵��ߣ�*/
}MSG;
#define MG_REPAINT 0x01 /*�ػ�*/
#define MG_NUMBER  0x02 /*�����ַ�*/
#define MG_CONTROL 0x04 /*���Ʒ�*/



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

void savePassword(const unsigned char *oPpassword);
void readPassword(long *Ppassword);

#endif