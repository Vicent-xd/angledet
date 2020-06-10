//serial-receive.cpp
#include<string>
#include<time.h>
#include<stdlib.h>
#include<iostream>
#include<wiringPi.h>
#include<wiringSerial.h>
#include<unistd.h>

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
int main()
{
        int MySerial;
	int YPR[3]={0};
	int BUF[10]={0};
	int STOP_BIT=0;
        // pinMode(LED, OUTPUT);
        if (wiringPiSetup()<0)
        {
                cout << "Setup Failed!\n";
                exit(0);
        }

        if ((MySerial=serialOpen("/dev/ttyS0", 115200))<0)
        {
                cout << "Serial Failed!\n";
                exit(0);
        }
//	serialPrintf(MySerial,0xa5);
//	serialPrintf(MySerial,0x51);
	while (1)
        {
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
					string   time = getTime();
                                        cout << time << endl;
					//cout<<"SUCCESS!"<<endl;
					//YPR[0]=BUF[0]<<8|BUF[1];
			       		YPR[1]=BUF[2]<<8|BUF[3];
					YPR[2]=BUF[4]<<8|BUF[5];
					//system("clear");
					//cout<<"yaw:"<<YPR[0]<<endl;
					cout<<"    pitch:"<<YPR[1]/100.0<<endl;
					cout<<"    roll:"<<YPR[2]/100.0<<endl;
					sleep(5);
					serialFlush(MySerial);
				}
				else
					cout<<"broken pack!"<<endl;
			}
                }
        }

        serialClose(MySerial);

        return 0;
}
