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

#include"ccl.h"

main()
{
	InitMsg();
	while(0<GetMsg());
	Reset();
	return(0);
}

char MsgProc(unsigned char message,unsigned char param)
{
	static char xdata password[PWLEN]="";
	static char xdata ibuf[PWLEN+1]="";
	static char xdata cnt=0;
	static char xdata model=MD_NORM;
	switch(message){
		case(MG_CREAT):
		initialize(password);
		break;
		case(MG_ERROR):
		inputset &= ~IS_KEY;
		error(param);
		Delay1ms(1000);
		switch(param){
			case(EPSW):case(EPSW2):
			{
				int xdata i=5000;
				while(--i) {
					Beep();
				}
			}
			default:
			Reset();
		}
		break;
		case(MG_COMMAND):
		switch(param){
			case(OPR_OPEN):
			RELAY=0; //开继电器
			myPrintf("\fSuccess!");
			Delay1ms(1000);
			PostMsg(MG_QUIT,0);
			break;
			case(OPR_CONF):
			operatConfirm(&model,ibuf,password);
			break;
			case(OPR_BACK):
			model &= MD_NORM;
			read(password,00,PWLEN);
			PostMsg(MG_COMMAND,OPR_CLS);
			break;
			case(OPR_CHAN):
			if(MD_NORM==(model&MD_CONF)){
				model |= MD_ORGN;
			}
			PostMsg(MG_COMMAND,OPR_CLS);                     
			break;
			case(OPR_CLS):
			memset(ibuf,'\0',PWLEN);
			cnt=0;
			LcdCls();
			if(inputset&IS_KEY)switch(model){
				case(MD_NORM):myPrintf("Enter PW:\n");break;
				case(MD_ORGN):myPrintf("Enter origin PW:\n");break;
				case(MD_NEWP):myPrintf("Enter new PW:\n");break;
				case(MD_CONF):myPrintf("Re-enter PW:\n");break;
			}
			if(inputset&IS_COM){
				myPrintf("Wait for connect");
			}
			break;
			case(OPR_DEL):
			LcdDelete();
			if(cnt>0) ibuf[--cnt]=0;
			break;
			case(OPR_COMON):
			inputset &= ~IS_KEY;
			UsartConfig();
			//PostMsg(MG_SDDAT,0xff);//提示钥匙可以开始发信了
			inputset |= IS_COM;
			break;
		}
		break;
		case(MG_CHAR):
		if(cnt<PWLEN){
			echo(ibuf[cnt++]=param);
		}
		break;
		case(MG_CHNECHO):
		set_echo(param,cnt,ibuf);
		break;
		case(MG_SDDAT):
		/*SendByte(param);*/
		/*{
			char xdata tm;
			float xdata pref=0.3014568;
			for(tm=0;tm<7;tm++){
				myPrintf("\fL: %.6f\nK: %.6f",pref,pref);
				pref=logistic(pref);
				Delay1ms(500);
			}
			PostMsg(MG_COMMAND,OPR_OPEN);
		}*/
		break;
		case(MG_RCDAT):
		ibuf[cnt++]=param;
		Beep();
		if(cnt>=sizeof(float)){
			static float xdata pref=0;
			if(0==inputset&7){
				pref=*(float *)ibuf;
			}
			else {
				pref=logistic(pref);
			}
			cnt=0;
			inputset++;//低3位记通信的次数
			myPrintf("\fL: %.6f\nK: %.6f",pref,*(float *)ibuf);
			if(fabs(*(float *)ibuf-pref)<EPS){
				memset(ibuf,0,sizeof(float));
				if(7==inputset&7){
					PostMsg(MG_COMMAND,OPR_OPEN);
				}
				else {
					REN=1;
					//PostMsg(MG_SDDAT,0xff);//提示钥匙可以开始发信了
				}
			}
			else {
				PushMsg(MG_ERROR,EPSW2);
			}
		}
		else {
			REN=1;
			//PostMsg(MG_SDDAT,0xff);//提示钥匙可以开始发信了
		}
		break;
		default:
		;
	}
	return(model);
}

void initialize(char *password)
{
	LcdInit();
	read(password,00,PWLEN);
	inputset |= IS_KEY;
	PostMsg(MG_COMMAND,OPR_CLS);
}

void operatConfirm(char *pmodel,char *ibuf,char *password)
{
	switch((*pmodel)&MD_CONF){
		case(MD_NORM):
			if(strncmp(ibuf,password,PWLEN)){
				PushMsg(MG_ERROR,EPSW);
			}
			else {
				PostMsg(MG_COMMAND,OPR_COMON);
				//PostMsg(MG_COMMAND,OPR_OPEN);
			}
		break;
		case(MD_ORGN):
			if(strncmp(ibuf,password,PWLEN)){
				PushMsg(MG_ERROR,EPSW);
			}
			else {
				++(*pmodel);
			}
		break;
		case(MD_NEWP):
			strncpy(password,ibuf,PWLEN);
			++(*pmodel);
		break;
		case(MD_CONF):
			if(strncmp(ibuf,password,PWLEN)){
				read(password,00,PWLEN);
				PushMsg(MG_ERROR,EPSW);
			}
			else {
				write(password,00,PWLEN);
				*pmodel &= MD_NORM;
			}
		break;
	}
	PostMsg(MG_COMMAND,OPR_CLS);
}


const float code k=3.983271;
float logistic(float x)
{
	return k*x*(1-x);
}

