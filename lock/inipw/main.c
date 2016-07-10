/* 设置初始密码 */
#include"..\io.h"

#define PWLEN 16
char code password[PWLEN]="0123456789";

main(){
	char xdata buf[PWLEN];
	write(password,00,PWLEN);
	LcdInit();
	read(buf,00,PWLEN);
	NoBufferPrint(buf);
	while(1);
	return(0);
}