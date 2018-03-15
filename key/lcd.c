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

#include"lcd.h"

static void LcdWrite(bit op,unsigned char dat)	  //写入命令/数据
{
	LCD1602_E = 0;     //使能清零
	LCD1602_RS = op;	   //选择发送命令|数据
	LCD1602_RW = 0;	   //选择写入
	
	#ifndef LCD1602_4PINS
		LCD1602_DATAPINS = dat;     //放入命令/数据
		Delay1ms(1);		//等待数据稳定
		
		LCD1602_E = 1;	          //写入时序
		Delay1ms(5);	  //保持时间
		LCD1602_E = 0;
		
	#else
		LCD1602_DATAPINS &= 0x0f;
		LCD1602_DATAPINS = HI4BITS(dat);	//发送高四位
		Delay1ms(1);
		
		LCD1602_E = 1;	          //写入时序
		Delay1ms(5);	  //保持时间
		LCD1602_E = 0;
		
		LCD1602_DATAPINS = LO4BITS(dat); //发送低四位
		Delay1ms(1);
		
		LCD1602_E = 1;	          //写入时序
		Delay1ms(5);	  //保持时间
		LCD1602_E = 0;
		
	#endif
}

static char LcdRead(bit op)     //读入状态/数据（好像并没什么用）
{
	char dat;
	#ifndef  LCD1602_4PINS
		LCD1602_DATAPINS=0xff;  //其实我也不知道为什么要全部置一
		LCD1602_E = 0;
		Delay1ms(3);
		
		LCD1602_RS = op;	   //选择发送命令|数据
		LCD1602_RW = 0;	   //选择读出
		LCD1602_E = 1;
		
		dat = LCD1602_DATAPINS;
		Delay1ms(1);		
		LCD1602_E = 0;
	#else 
		char buf;
		LCD1602_DATAPINS |= 0xf0; //高四位置一
		LCD1602_E = 0;
		Delay1ms(3);
		
		LCD1602_RS = op;	   //选择发送命令|数据
		LCD1602_RW = 0;	   //选择读出
		LCD1602_E = 1;
		
		dat = LCD1602_DATAPINS;
		Delay1ms(1);		
		LCD1602_E = 0;  //EN脉冲宽度大于150ns
		Delay1ms(1);
		dat &= 0xf0;  //取高位
		
		LCD1602_DATAPINS |= 0xf0; //高四位置一
		Delay1ms(1);  
		LCD1602_E = 1;  //之前至少延时大于30ns
		buf = LCD1602_DATAPINS;
		Delay1ms(1);		
		LCD1602_E = 0;   //EN脉冲宽度大于150ns
		Delay1ms(1);
		dat |= (buf>>4); //得到低位
	#endif
	return(dat);
}

void LcdInit()						  //LCD初始化
{
	#ifndef  LCD1602_4PINS
		#ifndef  LCD1602_10ROWS
			LcdWrite(OP_COMD,CMD_8PINS|CMD_2LINE);
		#else
			LcdWrite(OP_COMD,CMD_8PINS|CMD_10ROW);
		#endif
		LcdWrite(OP_COMD,CMD_NOCUR);  //开显示不显示光标
		//LcdWrite(OP_COMD,CMD_CURRT);  //写一个指针加1，这个好像是自动的
		LcdWrite(OP_COMD,CMD_CLS);
		LcdWrite(OP_COMD,DDRAMADD(0x00)); //设置数据指针起点
	#else
		LcdWrite(OP_COMD,0x32); //据说是将8位总线转为4位总线，咱也不懂
		#ifndef  LCD1602_10ROWS
			LcdWrite(OP_COMD,CMD_2LINE);
		#else
			LcdWrite(OP_COMD,CMD_10ROW);
		#endif
		LcdWrite(OP_COMD,CMD_NOCUR);  //开显示不显示光标
		//LcdWrite(OP_COMD,CMD_CURRT);  //写一个指针加1，这个好像是自动的
		LcdWrite(OP_COMD,CMD_CLS);
		LcdWrite(OP_COMD,DDRAMADD(0x00)); //设置数据指针起点
	#endif
	lcd.ac=0;
	lcd.base=buffer;
}

#ifndef  LCD1602_10ROWS
#define is_lcdprint(x) (((x)>=32&&(x)<=127)||((x)>=160&&(x)<=223))
#else
#define is_lcdprint(x) (((x)>=32&&(x)<=127)||((x)>=160&&(x)<=255))
#endif

void LcdCls(void)
{
	LcdWrite(OP_COMD,CMD_CLS);
	lcd.ac=0;
	memset(lcd.base,0,CHCOL*CHROW);
}

void LcdCursorShow(bit set)
{
	if(set){
		LcdWrite(OP_COMD,CMD_CURFLS);
	}
	else {
		LcdWrite(OP_COMD,CMD_NOCUR);
	}
}

void LcdLineFeed(void)
{
	#ifndef LCD1602_10ROWS
		if(lcd.ac<SCDLINE){
			LcdWrite(OP_COMD,CMD_CURES);
			LcdWrite(OP_COMD,DDRAMADD(SCDLINE));
			lcd.ac=SCDLINE;
		}
	#else
		LcdCls();
	#endif
}

void LcdBackspace(void)
{
	if((lcd.ac&~SCDLINE)>0){
		LcdWrite(OP_COMD,CMD_CRLMV);
		if(lcd.ac&~SCDLINE>SCCOL){
			LcdWrite(OP_COMD,CMD_SCLMV);
		}
		LcdWrite(OP_DATA,' ');
		LcdWrite(OP_COMD,CMD_CRLMV);
		lcd.base[lcd.ac--]=0;
	}
}

void LcdDelete(void)
{
	if(lcd.base[lcd.ac]){
		LcdWrite(OP_DATA,0);
		LcdWrite(OP_COMD,CMD_CRLMV);
		lcd.base[lcd.ac]=0;
	}
	else {
		LcdBackspace();
	}
}

void LcdPutchar(char c)
{
	if(is_lcdprint(c) && (lcd.ac<(SCDLINE+40))){
		if((lcd.ac&~SCDLINE)<CHCOL-1){
			LcdWrite(OP_DATA,c);
			lcd.base[lcd.ac&~SCDLINE]=c;
			if((lcd.ac&~SCDLINE)>=SCCOL){
				LcdWrite(OP_COMD,CMD_SCLMV);
			}
			lcd.ac++;
		}
		else {
			LcdWrite(OP_DATA,c);
			lcd.base[lcd.ac]=c;
			LcdLineFeed();
		}
	}
}

void LcdCursorMov(bit drc)
{
	if(drc==DRC_LEFT){
		if((lcd.ac&~SCDLINE)>0 && lcd.base[lcd.ac-1] ){
			LcdWrite(OP_COMD,CMD_CRLMV);
			if(lcd.ac&~SCDLINE>SCCOL){
				LcdWrite(OP_COMD,CMD_SCLMV);
			}
			lcd.ac--;
		}
	}
	else {
		if((lcd.ac&~SCDLINE)!=(CHCOL-1) && lcd.base[lcd.ac+2] ){
			LcdWrite(OP_COMD,CMD_CRRMV);
			if(lcd.ac&~SCDLINE>=SCCOL){
				LcdWrite(OP_COMD,CMD_SCRMV);
			}
			lcd.ac++;
		}
	}
}