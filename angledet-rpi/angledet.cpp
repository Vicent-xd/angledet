//serial-receive.cpp
#include<string>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<wiringPi.h>
#include<wiringSerial.h>
#include<unistd.h>
#include<fstream> 
#define LED 1

using namespace std;

string getTime()
{
    time_t timep;
    time (&timep); //获取time_t类型的当前时间
    char tmp[64];
    strftime(tmp, sizeof(tmp), "%Y-%m-%d %H:%M:%S",localtime(&timep) );//对日期和时间进行格式化
    return tmp;
}
int Fake2Real(int fake){
	int real=0;
	//cout<<"fake:"<<fake<<endl;
	if (fake>40000 &&fake<65536){
		real=fake-65536;
		//cout<<"Im in"<<endl;
	}
	else{
		//cout<<"Direct"<<endl;
		real=fake;
	}
	//cout<<"real:"<<real<<endl;
	return real;
}
int getAngle(int Serial,unsigned char Addr)
{
	int YPR[3]={0};
	int BUF[10]={0};
	int STOP_BIT=0;
    //cout<<"正在与从机 "<<(int)Addr<<"通信"<<endl;
    serialPutchar(Serial,Addr);//send address1
    sleep(1);
    if (serialDataAvail(Serial)>0)
    {
            int ch=serialGetchar(Serial);
            //cout << ch << endl;
            //printf("%x\n",ch);
			//int val=ch-48;
            //digitalWrite(LED, val);
			if (ch==0xaa){
				BUF[0]=serialGetchar(Serial);
				BUF[1]=serialGetchar(Serial);
				BUF[2]=serialGetchar(Serial);
				BUF[3]=serialGetchar(Serial);
				BUF[4]=serialGetchar(Serial);
				BUF[5]=serialGetchar(Serial);
				STOP_BIT=serialGetchar(Serial);
		        if (STOP_BIT==0x55){
					string time = getTime();
                    cout << time << endl;
					//cout<<"SUCCESS!"<<endl;
					//YPR[0]=BUF[0]<<8|BUF[1];
			       	YPR[1]=Fake2Real(BUF[2]<<8|BUF[3]);
					YPR[2]=Fake2Real(BUF[4]<<8|BUF[5]);
					//system("clear");
					cout<<"从机"<<(int)Addr<<":"<<endl;
					//cout<<"yaw:"<<YPR[0]<<endl;
					cout<<"    pitch:"<<YPR[1]/100.0<<endl;
					cout<<"    roll:"<<YPR[2]/100.0<<endl;
					//cout<<"    pitch:"<<YPR[1]<<endl;
					//sleep(5);
					ofstream out("data",ios::app);
					out<<(int)Addr<<':'<<YPR[1]/100.0<<','<<YPR[2]/100.0<<endl;
					out.close();
					serialFlush(Serial);
					return 0;
				}
				else{
					cout<<"从机"<<(int)Addr<<":"<<endl;
					cout<<"broken pack!"<<endl;
					return -1;
				}

			}
        }
    return -1;           
}
int main()
{
    int MySerial;
	/*int YPR[3]={0};
	int BUF[10]={0};
	int STOP_BIT=0;*/
	unsigned char Addr1=(unsigned char)0x01;
	unsigned char Addr2=(unsigned char)0x02;
        // pinMode(LED, OUTPUT);
        if (wiringPiSetup()<0)
        {
                cout << "Setup Failed!\n";
                exit(0);
        }

        if ((MySerial=serialOpen("/dev/ttyS0", 9600))<0)
        {
                cout << "Serial Failed!\n";
                exit(0);
        }
//	serialPrintf(MySerial,0xa5);
//	serialPrintf(MySerial,0x51);
	while (1)
        {
        	getAngle(MySerial,Addr1);
        	getAngle(MySerial,Addr2);
        /*
        	cout<<"正在与从机 "<<(int)Addr1<<"通信"<<endl;
        	serialPutchar(MySerial,Addr1);//send address1
        	sleep(2);
            if (serialDataAvail(MySerial)>0)
            {
                    int ch=serialGetchar(MySerial);
                    //cout << ch << endl;
                    //printf("%x\n",ch);
					//int val=ch-48;
                    //digitalWrite(LED, val);
					if (ch==0xaa){
						BUF[0]=serialGetchar(MySerial);
						BUF[1]=serialGetchar(MySerial);
						BUF[2]=serialGetchar(MySerial);
						BUF[3]=serialGetchar(MySerial);
						BUF[4]=serialGetchar(MySerial);
						BUF[5]=serialGetchar(MySerial);
						STOP_BIT=serialGetchar(MySerial);
				        if (STOP_BIT==0x55){
							string time = getTime();
		                    cout << time << endl;
							//cout<<"SUCCESS!"<<endl;
							//YPR[0]=BUF[0]<<8|BUF[1];
					       	YPR[1]=BUF[2]<<8|BUF[3];
							YPR[2]=BUF[4]<<8|BUF[5];
							//system("clear");
							cout<<"从机 "<<(int)Addr1<<":"<<endl;
							//cout<<"yaw:"<<YPR[0]<<endl;
							cout<<"    pitch:"<<YPR[1]/100.0<<endl;
							cout<<"    roll:"<<YPR[2]/100.0<<endl;
							//sleep(5);
							serialFlush(MySerial);
						}
						else{
							cout<<"从机 "<<(int)Addr1<<":"<<endl;
							cout<<"broken pack!"<<endl;
						}
					}
                }*/
        }

        serialClose(MySerial);

        return 0;
}
