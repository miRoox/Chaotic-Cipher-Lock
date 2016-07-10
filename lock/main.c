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
			RELAY=0; //¿ª¼ÌµçÆ÷
			Delay1ms(5000);
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
			switch(model){
				case(MD_NORM):myPrintf("Enter PW:\n");break;
				case(MD_ORGN):myPrintf("Enter origin PW:\n");break;
				case(MD_NEWP):myPrintf("Enter new PW:\n");break;
				case(MD_CONF):myPrintf("Re-enter PW:\n");break;
			}
			break;
			case(OPR_DEL):
			LcdDelete();
			if(cnt>0) ibuf[--cnt]=0;
			break;
			case(OPR_COMON):
			inputset &= ~IS_KEY;
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
	myPrintf("Enter PW:\n");
}

void operatConfirm(char *pmodel,char *ibuf,char *password)
{
	switch((*pmodel)&MD_CONF){
		case(MD_NORM):
			if(strncmp(ibuf,password,PWLEN)){
				PushMsg(MG_ERROR,EPSW);
			}
			else {
				//PostMsg(MG_COMMAND,OPR_COMON);
				PostMsg(MG_COMMAND,OPR_OPEN);
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