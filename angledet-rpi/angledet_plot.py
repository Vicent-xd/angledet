from mpl_toolkits.mplot3d import axes3d
import matplotlib.pyplot as plt
 
# # 打开画图窗口1，在三维空间中绘图
fig = plt.figure(1)
ax = fig.gca(projection='3d')
 
# # 给出点（0，0，0）和（100，200，300）
# x = [0, 0]
# y = [0, 0]
# z = [-1, 1]
 
# # 将数组中的前两个点进行连线
# ax.plot(x, y, z, c='r')
# plt.show()
import numpy as np
import scipy.linalg as linalg
import math
#参数分别是旋转轴和旋转弧度值
def rotate_mat(axis, radian):
    return linalg.expm(np.cross(np.eye(3), axis / linalg.norm(axis) * radian))

axis_x, axis_y, axis_z = [1,0,0], [0,1,0], [0, 0, 1]#分别是x,y和z轴,也可以自定义旋转轴
yaw = 2 #pi/4
rot_matrix = rotate_mat(axis_x, yaw)#绕Z轴旋转pi/4
print(rot_matrix)
old_coor=np.array([[0,0,-1],[0,0,1]])
new_coor=np.dot(old_coor, rot_matrix)
print(new_coor)
x = [new_coor[0][0],new_coor[1][0]]
y = [new_coor[0][1], new_coor[1][1]]
z = [new_coor[0][2], new_coor[1][2]]
ax.plot(x, y, z, color='red',linewidth=4)
plt.show()
