/*********************************************************************** 
时间：2012.11.24 
芯片：STC12C5A60S2 
晶振：22.1184MHz   波特率：9600bps 
引脚定义：串行口1：  发送————TxD/P3.1； 接收————RxD/P3.0 
          串行口2： 发送————TxD2/P1.3；接收————RxD2/P1.2 
功能描述：STC12双串口通信（中断方式） 
          当串行口1接收数据后，将此数据由串行口2发送出去 
          当串行口2接收数据后，将此数据由串行口1发送出去 
***********************************************************************/  
#include<STC12C5A60S2.h>  
#define S2RI 0x01   //串口2接收中断请求标志位  
#define S2TI 0x02   //串口2发送中断请求标志位  
unsigned char flag1,flag2,temp1,temp2;  
unsigned char BUF[8]={0x11,0x11,0x11,0x11,0x11,0x11,0x11,0x11}; //8 bits per pkt
unsigned char RES[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
int reading=-1;
int cnt=-1;
int uploaded=-1;
unsigned char addr1=0x02;
bit getAddr=0;
/****************串行口初始化函数****************/  
void InitUART(void)  
{  /*
    TMOD = 0x20;    //定时器1工作在方式2  8位自动重装  
    //SCON = 0x50;    //串口1工作在方式1  10位异步收发 REN=1允许接收  
	  SCON=0Xf0;  //设置为工作方式3,打开接收允许位
	  SM2=1;//直接收地址帧
    //TH1 = 0xFA;    //定时器1初值  
    TH1 = 0xFD;
    TL1 = TH1;  
    TR1 = 1;       //定时器1开始计数  
    EA =1;  //开总中断  
    ES =1;  //开串口1中断  
*/
	 TMOD=0x20; //定时器1工作于方式2
	 TH1=0xfd;  
	 TL1=0xfd; //波特率为9600
	 PCON=0;
	 SCON=0xd0;  //串口工作于方式3
	 SM2=1;//直接收地址帧
	 TR1=1;  //开启定时器
	 TI=0;
	 RI=0;
	 EA =1;  //开总中断  
	 ES =1;  //开串口1中断  	 
	/*
		PCON &= 0x7F;		//波特率不倍速
	//SCON = 0x50;		//8位数据,可变波特率
	SCON = 0xD0;		//9位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xDC;		//设定定时初值
	TH1 = 0xDC;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	    EA =1;  //开总中断  
    ES =1;  //开串口1中断  
		*/
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
void Uart_1SendPkt(unsigned char pkt[8])
{
	int i=0;
	for (i=0;i<8;i++){
	    UART_1SendOneByte(pkt[i]);
	}
}	
void UART_1Listen(void)
{
	SM2=1;
	if(RI==1)  
    {  
        RI=0;  
        //flag1=1;  
        //temp1=SBUF;
			  //UART_1SendOneByte(0xff);//got ri
			  if(RB8)
				{
					flag1=1;
					temp1=SBUF;
					//UART_1SendOneByte(0x00);//got rb8
					if (temp1==addr1)
					{
						//UART_1SendOneByte(addr1);
						SM2=0;//中断地址和数据
						getAddr=1;
						while(reading==1);
						Uart_1SendPkt(BUF);
						SM2=1;
					}
					else
					{
						SM2=1;//只中断地址帧
						getAddr=0;
					}
				}
				/*
				else if (getAddr)
				{
					uploading=1;
				}
				else if(!getAddr)
				{
					uploading=0;
				}*/
	
    }  
} 
/*****************主函数******************/  
void main(void)  
{  
    InitUART(); //串行口初始化  
		
    while(1)  
    {
      //UART_1SendOneByte(0xbb);			
		//UART_1Listen();
		//if(reading==0 && uploading==1)
		//	Uart_1SendPkt(RES);
        //如果串口1接收到数据，将此数据由串口2发送  
        /*
        if(flag1==1)  
        {  
            flag1=0;  
            UART_2SendOneByte(temp1);  
        }  */
          
        //如果串口2接收到数据，将此数据由串口1发送
        //从机连续发包，直到主机要求停止
			  //UART_1SendOneByte(temp2); 
       /* if(flag2==1)
        {  
					  //UART_1SendOneByte(temp2);
            flag2=0;
            if(uploading==1){					
                UART_1SendOneByte(temp2); 
						}							
					
					  if (temp2==0xaa){
							//UART_1SendOneByte(0xaa);
							reading=1;
							cnt=0;
							while(reading==1){
								  //UART_1SendOneByte(temp2);
								  Uart_1SendPkt(BUF);
							    BUF[cnt]=temp2;
							    cnt++;
								  if(cnt==8)      //读完8位结束
									{
										  reading=0;
									    if (temp2==0x55)  
									        copy_arr(BUF,RES);
								  }
								  while(flag2==0);//等待串口来数据->temp2

							}
              }
        }  */
				//Uart_1SendPkt(RES);
				//UART_1SendOneByte(0xaa);
				//Uart_1SendPkt(BUF);
}
}

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
						if (uploaded==0)
							Uart_1SendPkt(BUF);
						  uploaded=1;
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
/*
void UART_1Interrupt(void) interrupt 4  
{  
    if(RI==1)  
    {  
        RI=0;  
        flag1=1;  
        temp1=SBUF;
			  UART_1SendOneByte(0x88);//got ri
			  if(RB8)
				{
					UART_1SendOneByte(0x99);//got rb8
					if (temp1==addr1)
					{
						UART_1SendOneByte(addr1);
						SM2=0;//中断地址和数据
						getAddr=1;
					}
					else
					{
						SM2=1;//只中断地址帧
						getAddr=0;
					}
				}
				else if (getAddr)
				{
					uploading=1;
				}
				else if(!getAddr)
				{
					uploading=0;
				}
	
    }  
}  */
/************串行口2中断处理函数*************/  
void UART_2Interrupt(void) interrupt 8  
{  
    if(S2CON&S2RI)  
    {  
        S2CON&=~S2RI;  
        flag2=1;  
        temp2=S2BUF;  
		if (temp2==0xaa){
			reading=1;//读取中
			cnt=0;
			BUF[cnt]=temp2;
			cnt++;
		}
		else if(cnt>0 && cnt<7)
		{
			BUF[cnt]=temp2;
			cnt++;
		}
		else if(cnt==7)
		{
			cnt=0;
			if(temp2==0x55){
				BUF[7]=temp2;
				copy_arr(BUF,RES);
				uploaded=0;
				reading=0;//读结束
			}
			//else
			//	reading=1;
		}
			  //UART_1SendOneByte(temp2); 
    }   
}