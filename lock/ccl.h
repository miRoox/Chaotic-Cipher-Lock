#ifndef __CCL_
#define __CCL_
/** оƬ�ͺţ� STC90C516RD+ **/
/******** ���� ***************
 * P0 �ڽ�LCD1602�� DB0~DB7
 * P1 �ڽӾ�����̣����io.c
 * �������� P2.2
 * �̵����� P2.3
 * LCD�� RW �� P2.5
 * LCD�� RS �� P2.6
 * LCD�� E �� P2.7
 *****************************/

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