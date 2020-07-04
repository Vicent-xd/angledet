#include<STC12C5A60S2.h>  
#include <intrins.h>
#define S2RI 0x01   //串口2接收中断请求标志位  
#define S2TI 0x02   //串口2发送中断请求标志位  
#define uchar unsigned char
#define uint unsigned int
unsigned char flag1,flag2,temp1,temp2;  
unsigned char BUF[8]={0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}; //8 bits per pkt
unsigned char RES[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
int reading=-1;
int cnt=-1;
int uploading=-1;
unsigned char addr1=0x01;
bit getAddr=0;
sbit KEY1=P0^0;
sbit KEY2=P0^1;
#define S2RB8 0x04          //S2CON.2
#define S2TB8 0x08          //S2CON.3

//********************************************DS18b20***************************
sbit DQ=P3^2; 				//数据传输线接单片机的相应的引脚 
unsigned char data tempL=0; 		//设全局变量
unsigned char data tempH=0; 

bit fg=1;        			//温度正负标志
void UART_2SendOneByte(unsigned char c) ;
/*******************************************************************************
* 函数名         : delay(uint u)
* 函数功能                 : 延时ums
* 输入           : uint u
* 输出                  : 无
*******************************************************************************/
void delay_ums(unsigned int u)                       //延时ums 12M晶振误差 0us
{
   unsigned char a,b;
   for( ; u > 0 ; u --)         
                for(b=199;b>0;b--)
                        for(a=1;a>0;a--);
}
//newnew
//适用于STC12c和stc5系列单片机
//编写：刘海龙
//温度传感器定义
//在主程序里调用ReadTemperature();//读取温度程序
//sbit DQ = P5 ^ 5;//ds18B20连接的IO口
unsigned int tem;//
/*********************************/
void delay_us(unsigned char n)// STC15系列单片机延时1微秒@12MHZ
{ 
    while (n--) 
    { 
        _nop_(); 
        _nop_();
        _nop_(); 
    } 
}        
/******************************************************************************/
void Init_DS18B20(void){//初始化ds1820
        unsigned char x=0;
        
DQ = 1;          //DQ复位
delay_us(80); //稍做延时
DQ = 0;          //单片机将DQ拉低
delay_us(240); //精确延时 大于 480us
delay_us(240);
DQ = 1;          //拉高总线
delay_us(60);
x=DQ;        
delay_us(240);              //等待设备释放数据线 
delay_us(180);
}
/******************************************************************************/
unsigned char ReadOneChar(void){//读一个字节
        unsigned char i=0;
        unsigned char dat = 0;
        for (i=8;i>0;i--){
                DQ = 0; // 给脉冲信号
                dat>>=1;
                DQ = 1; // 给脉冲信号
                if(DQ==1){
                        dat|=0x80;
                }
                delay_us(60);
        }
        return(dat);
}
/******************************************************************************/
void WriteOneChar(unsigned char dat){//写一个字节
        unsigned char i=0;
        for (i=8; i>0; i--){
                DQ = 0;
                DQ = dat&0x01;
                delay_us(60);
                DQ = 1;
                dat>>=1;
        }
}
/******************************************************************************/

/************************************/
void ReadTemperature(void){//读取温度
        //unsigned char a=0;
        //unsigned char b=0;
        //unsigned int t=0;
        //float tt=0;
        Init_DS18B20();
        WriteOneChar(0xCC); // 跳过读序号列号的操作
        WriteOneChar(0x44); // 启动温度转换
        Init_DS18B20();
        WriteOneChar(0xCC); //跳过读序号列号的操作
        WriteOneChar(0xBE); //读取温度寄存器
        tempL=ReadOneChar();  //读低8位
        tempH=ReadOneChar(); //读高8位
//				UART_2SendOneByte(tempL);
//				UART_2SendOneByte(tempH);
       /* t=b;
        t<<=8;
        t=t|a;
        tt=t*0.0625;
        t= tt*10+0.5; //放大10倍输出并四舍五入
        tem=t;
        return(t);*/
}

//ReadTemperature();//读取温度程序
//***

/****************串行口初始化函数****************/  
void InitUART(void)  
{  
  /*  TMOD = 0x20;    //定时器1工作在方式2  8位自动重装  
    //SCON = 0x50;    //串口1工作在方式1  10位异步收发 REN=1允许接收  
	  SCON=0Xf0;  //设置为工作方式3,打开接收允许位
    //TH1 = 0xFA;    //定时器1初值  
    TH1 = 0xFD;
    TL1 = TH1;  
    TR1 = 1;       //定时器1开始计数  
	*/
	TMOD=0x20; //定时器1工作于方式2
	TH1=0xfd;  
	TL1=0xfd; //波特率为9600
	PCON=0;
	SCON=0xd0;  //串口工作于方式3
	SM2=0;//receive all
	TR1=1;  //开启定时器
	TI=0;
	RI=0;
    /*
		PCON &= 0x7F;		//波特率不倍速
		SCON = 0xD0;		//9位数据,可变波特率
		AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
		AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
		TMOD &= 0x0F;		//清除定时器1模式位
		TMOD |= 0x20;		//设定定时器1为8位自动重装方式
		TL1 = 0xDC;		//设定定时初值
		TH1 = 0xDC;		//设定定时器重装值
		ET1 = 0;		//禁止定时器1中断
		TR1 = 1;		//启动定时器1	
	  SM2=0;
	*/
    EA =1;  //开总中断  
    ES =1;  //开串口1中断  
  /*
    S2CON = 0x50;   //串口2工作在方式1  10位异步收发 S2REN=1允许接收  
    //BRT = 0xFA;    //独立波特率发生器初值  
	  BRT = 0xFD;
    //AUXR = 0x10;    //BRTR=1 独立波特率发生器开始计数  
	  AUXR &= 0xf7;
	  AUXR |= 0x04;
	  AUXR |= 0x10;
	*/
	  AUXR &= 0xF7;		//波特率不倍速
	  S2CON = 0x50;		//8位数据,可变波特率
	  AUXR |= 0x04;		//独立波特率发生器时钟为Fosc,即1T
	  BRT = 0xDC;		//设定独立波特率发生器重装值
	  AUXR |= 0x10;		//启动独立波特率发生器
		
    IE2 =0x01;  //开串口2中断  ES2=1  
}  
/****************串行口1发送****************/  
void UART_1SendOneByte(unsigned char c)  
{  
	  TB8=0;
    SBUF = c;  
    while(!TI);    //若TI=0，在此等待  
    TI = 0;    
}  
/****************串行口2发送****************/  
void UART_2SendOneByte(unsigned char c)  
{  
    S2BUF = c;  
    while(!(S2CON&S2TI));  //若S2TI=0，在此等待  
    S2CON&=~S2TI;     //S2TI=0  
}
//void UART_2SendOnePkt(unsigned )  
void copy_arr(unsigned char *from,unsigned char *to)
{
	int i=0;
	for (i=0;i<8;i++)
  {	  
		*to=*from;
	   to++;
	   from++; 
  }
}
void UART_1SendAddr(unsigned addr)
{
	 //while(temp1!=addr) //主机等待从机返回其地址作为应答信号
	 //{
		TI=0;    //发送从机地址
		TB8=1;    //发送地址帧
		SBUF=addr;
		while(!TI);
		TI=0; 
		/*
		RI=0;
		while(!RI);
		temp1=SBUF;
		RI=0;*/
	 //}

}
void Uart_1SendPkt(unsigned char pkt[8])
{
	int i=0;
	for (i=0;i<8;i++){
	    UART_1SendOneByte(pkt[i]);
	}
}	
void main(void)  
{  
    InitUART(); //串行口初始化  
		
    while(1) {
/*
			if(KEY1==0)
			{
			 delay(5);
			 if(KEY1==0)
			 {
				while(!KEY1);
				UART_1SendAddr(0x01);
			 }
			}
			if(KEY2==0)
			{
			 delay(5);
			 if(KEY2==0)
			 {
				while(!KEY2);
				UART_1SendAddr(0x02);
			 }
			}*/
			//float tempreture=gettemp();
			//ReadTemperature();
				//UART_2SendOneByte(tempL);
				//UART_2SendOneByte(tempH);
			//UART_2SendOneByte(tempH);
			//UART_2SendOneByte(tempL);
			//delay_ums(1000);
			/*
			if(flag2==1)//收到上位机发来的地址，存在temp2里
			{
				flag2=0;
				UART_1SendAddr(temp2);//下发从机查询
			}
      if(flag1==1)//从机返回值
			{
				flag1=0;
				UART_2SendOneByte(temp1);//通过串口2回传上位机
			}*/
		}
}
/************串行口1中断处理函数*************/ 
/*
void UART_1Interrupt(void) interrupt 4  
{  
    if(RI==1)  
    {  
        RI=0;  
        flag1=1;  
        temp1=SBUF;
    }  
}  */
/************串行口1中断处理函数*************/  
void UART_1Interrupt(void) interrupt 4
{
  ES=0;
	if(RI==1)
	{
		RI=0;
		temp1=SBUF;
		if(RB8)
				{				
					//UART_1SendOneByte(0x00);//got rb8
					if (temp1==addr1)
					{
						//UART_1SendOneByte(addr1);
						SM2=1;//中断地址
						getAddr=1;
						/*if (uploaded==0)
							Uart_1SendPkt(BUF);
						  uploaded=1;*/
						ReadTemperature();
						UART_1SendOneByte(tempL);
						UART_1SendOneByte(tempH);
					}
					else
					{
						SM2=1;//只中断地址帧
						getAddr=0;
					}
				}

	}
	ES=1;
}
/************串行口2中断处理函数*************/  

void UART_2Interrupt(void) interrupt 8  
{  
    if(S2CON&S2RI)  
    {  
        S2CON&=~S2RI;  
        flag2=1;  
        temp2=S2BUF;
			  //UART_1SendAddr(temp2);
			  //UART_1SendOneByte(temp2); 
    }   
}