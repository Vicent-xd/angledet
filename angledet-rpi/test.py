import time 
import serial
ser=serial.Serial("/dev/ttyS0", baudrate=115200, timeout=3.0)
START_BIT = 0xAA
STOP_BIT = 0x55
while True:
    S_START_BIT=ser.read(2)
    if(S_START_BIT==START_BIT):
        ROW1=ser.read(2)
        ROW2=ser.read(2)
        YAW1=ser.read(2)
        YAW2=ser.read(2)
        PIT1=ser.read(2)
        PIT2=ser.read(2)
        S_STOP_BIT=ser.read(2)
        if(S_STOP_BIT==STOP_BIT):
            print('SUCCESS')
        else:
            print('Failed')
    else:
        print(S_START_BIT)
