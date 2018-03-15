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