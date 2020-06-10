#include<STC12C5A60S2.h>  
#define S2RI 0x01   //串口2接收中断请求标志位  
#define S2TI 0x02   //串口2发送中断请求标志位  
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
/*******************************************************************************
* 函数名         : delay(uint u)
* 函数功能                 : 延时ums
* 输入           : uint u
* 输出                  : 无
*******************************************************************************/
void delay(unsigned int u)                       //延时ums 12M晶振误差 0us
{
   unsigned char a,b;
   for( ; u > 0 ; u --)         
                for(b=199;b>0;b--)
                        for(a=1;a>0;a--);
}
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
			if(flag2==1)//收到上位机发来的地址，存在temp2里
			{
				flag2=0;
				UART_1SendAddr(temp2);//下发从机查询
			}
      if(flag1==1)//从机返回值
			{
				flag1=0;
				UART_2SendOneByte(temp1);//通过串口2回传上位机
			}
		}
}
/************串行口1中断处理函数*************/  
void UART_1Interrupt(void) interrupt 4  
{  
    if(RI==1)  
    {  
        RI=0;  
        flag1=1;  
        temp1=SBUF;
    }  
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