/** FROM 普中科技 **/
#include"i2c.h"

/*******************************************************************************
* 函 数 名         : Delay10us()
* 函数功能		   : 延时
* 输    入         : 无
* 输    出         : 无
*******************************************************************************/

static void I2C_Delay10us()
{
	uchar a, b;
	for(b=1; b>0; b--)
	{
		for(a=2; a>0; a--);
	}
}
/*******************************************************************************
* 函 数 名         : I2C_Start()
* 函数功能		   : 起始信号：在I2C_SCL时钟信号在高电平期间I2C_SDA信号产生一个下降沿
* 输    入         : 无
* 输    出         : 无
* 备    注         : 起始之后I2C_SDA和I2C_SCL都为0
*******************************************************************************/

static void I2C_Start()
{
	I2C_SDA = 1;
	I2C_Delay10us();
	I2C_SCL = 1;
	I2C_Delay10us();//建立时间是I2C_SDA保持时间>4.7us
	I2C_SDA = 0;
	I2C_Delay10us();//保持时间是>4us
	I2C_SCL = 0;			
	I2C_Delay10us();		
}
/*******************************************************************************
* 函 数 名           : I2C_Stop()
* 函数功能	         : 终止信号：在I2C_SCL时钟信号高电平期间I2C_SDA信号产生一个上升沿
* 输    入           : 无
* 输    出         	 : 无
* 备    注           : 结束之后保持I2C_SDA和I2C_SCL都为1；表示总线空闲
*******************************************************************************/

static void I2C_Stop()
{
	I2C_SDA = 0;
	I2C_Delay10us();
	I2C_SCL = 1;
	I2C_Delay10us();//建立时间大于4.7us
	I2C_SDA = 1;
	I2C_Delay10us();		
}
/*******************************************************************************
* 函 数 名           : I2cSendByte(uchar num)
* 函数功能 	         : 通过I2C发送一个字节。在I2C_SCL时钟信号高电平期间，
*                    * 保持发送信号I2C_SDA保持稳定
* 输    入           : num ,ack
* 输    出         	 : 0或1。发送成功返回1，发送失败返回0
* 备    注           : 发送完一个字节I2C_SCL=0, 需要应答则应答设置为1，否则为0
*******************************************************************************/

static uchar I2C_SendByte(uchar dat, uchar ack)
{
	uchar a = 0,b = 0;//最大255，一个机器周期为1us，最大延时255us。
			
	for(a=0; a<8; a++)//要发送8位，从最高位开始
	{
		I2C_SDA = dat >> 7;	 //起始信号之后I2C_SCL=0，所以可以直接改变I2C_SDA信号
		dat = dat << 1;
		I2C_Delay10us();
		I2C_SCL = 1;
		I2C_Delay10us();//建立时间>4.7us
		I2C_SCL = 0;
		I2C_Delay10us();//时间大于4us		
	}

	I2C_SDA = 1;
	I2C_Delay10us();
	I2C_SCL = 1;
	while(I2C_SDA && (ack == 1))//等待应答，也就是等待从设备把I2C_SDA拉低
	{
		b++;
		if(b > 200)	 //如果超过200us没有应答发送失败，或者为非应答，表示接收结束
		{
			I2C_SCL = 0;
			I2C_Delay10us();
			return 0;
		}
	}

	I2C_SCL = 0;
	I2C_Delay10us();
 	return 1;		
}
/*******************************************************************************
* 函 数 名           : I2cReadByte()
* 函数功能	    	 : 使用I2c读取一个字节
* 输    入           : 无
* 输    出         	 : dat
* 备    注           : 接收完一个字节I2C_SCL=0
*******************************************************************************/

static uchar I2C_ReadByte()
{
	uchar a = 0,dat = 0;
	I2C_SDA = 1;			//起始和发送一个字节之后I2C_SCL都是0
	I2C_Delay10us();
	for(a=0; a<8; a++)//接收8个字节
	{
		I2C_SCL = 1;
		I2C_Delay10us();
		dat <<= 1;
		dat |= I2C_SDA;
		I2C_Delay10us();
		I2C_SCL = 0;
		I2C_Delay10us();
	}
	return dat;		
}


/******************
 * 24C02 读写函数 *
 ******************/
void At24c02Write(unsigned char addr,unsigned char dat)
{
	I2C_Start();
	I2C_SendByte(0xa0, 1);//发送写器件地址
	I2C_SendByte(addr, 1);//发送要写入内存地址
	I2C_SendByte(dat, 0);	//发送数据
	I2C_Stop();
}
unsigned char At24c02Read(unsigned char addr)
{
	unsigned char num;
	I2C_Start();
	I2C_SendByte(0xa0, 1); //发送写器件地址
	I2C_SendByte(addr, 1); //发送要读取的地址
	I2C_Start();
	I2C_SendByte(0xa1, 1); //发送读器件地址
	num=I2C_ReadByte(); //读取数据
	I2C_Stop();
	return num;	
}
