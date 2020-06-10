import matplotlib.pyplot as plt
import numpy as np
import time
import os
import subprocess
pitch1=[]
roll1=[]
pitch2=[]
roll2=[]
def getangle():

    try:
        f=open('data','r')
    except:
        print("please start angledet process!")
        exit()

    data=f.readlines()
    #print(data)
    # pitch1=[]
    # roll1=[]
    # pitch2=[]
    # roll2=[]
    for d in data:
        pitch,roll=d[2:-1].split(',')
        if d.startswith('1:'):
            pitch1.append(float(pitch))
            roll1.append(float(roll))
        elif d.startswith('2:'):
            pitch2.append(float(pitch))
            roll2.append(float(roll))
#os.system("./angledet")
if(os.path.exists('/home/pi/angledet/data')):
    os.remove('/home/pi/angledet/data')
subprocess.Popen("/home/pi/angledet/angledet")
time.sleep(3)
plt.figure(figsize=(16,16))
plt.ylim(0,720)
while True:
    pitch1=[]
    roll1=[]
    pitch2=[]
    roll2=[]
    getangle()
    print(pitch1,roll1,pitch2,roll2)
    x1=[x for x in range(0,len(pitch1))]
    x2=[x for x in range(0,len(pitch2))]
#    plt.figure(figsize=(16,16))
    plt.clf()
    #plt.figure(2)
    plt.suptitle('sensors')
    plt.subplot(221)
    plt.plot(x1,pitch1, color='green', label='sensor1 pitch')
    plt.ylim(0,720)
    plt.legend()
    plt.subplot(222)
    plt.plot(x1,roll1,color='red', label='sensor1 roll')
    plt.ylim(0,720)
    plt.legend()
    plt.subplot(223)
    plt.plot(x2,pitch2, color='green', label='sensor2 pitch')
    plt.ylim(0,720)
    plt.legend()
    plt.subplot(224)
    plt.plot(x2,roll2,color='red', label='sensor2 roll')
    plt.ylim(0,720)
    plt.legend()

    #plt.show()
    #plt.draw()
    #time.sleep(1)
    plt.pause(1)


