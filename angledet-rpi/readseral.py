#!/usr/bin/python
# -*- coding:utf-8 -*-
import serial
import time

ser = serial.Serial("/dev/ttyS0",115200)

print('serial test start ...')
for i in range(10):
    ser.write(0xa5)
    ser.write(0x51)
    print(i)
try:
    while True:
        count=ser.inWaiting()
        if count != 0:
            recv = ser.read(count)
            print(type(recv))
            exit()
            #print(hex(recv))
        ser.flushInput()
        time.sleep(0.1)
#ser.write("Hello Wrold !!!\n")
except KeyboardInterrupt:
    if ser != None:
        ser.close()
