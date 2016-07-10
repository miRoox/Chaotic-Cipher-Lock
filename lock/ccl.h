#ifndef __CCL_
#define __CCL_
/* оƬ�ͺţ� STC90C516RD+ */

#include<reg51.h>
#include"io.h"

// - model -
#define MD_NORM 0x00 //��ͨ����������
#define MD_ORGN 0x01 //����ԭ����
#define MD_NEWP 0x02 //����������
#define MD_CONF 0x03 //ȷ��������

#define PWLEN 16

unsigned char data inputset=0;

// - ����ԭ�� -
void initialize(char *password);
void operatConfirm(char *pmodel,char *ibuf,char *password);

#endif