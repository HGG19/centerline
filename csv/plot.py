import matplotlib.pyplot as plt
import csv

x_c = []
y_c = []
with open('cones.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x_c.append(float(row[0]))
        y_c.append(float(row[1]))

x_v = []
y_v = []
with open('voronoi.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x_v.append(float(row[0]))
        y_v.append(float(row[1]))

x_s = []
y_s = []
with open('shape.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x_s.append(float(row[0]))
        y_s.append(float(row[1]))

x_center = []
y_center = []
with open('centerline.csv','r') as csvfile:
    plots = csv.reader(csvfile, delimiter=',')
    for row in plots:
        x_center.append(float(row[0]))
        y_center.append(float(row[1]))


plt.plot(x_c, y_c, 'ro', color='r')
#plt.plot(x_v, y_v, 'ro', color='g')
#plt.plot(x_s, y_s, 'ro', color='y')
plt.plot(x_center, y_center, '-ro', color='y')

plt.show()
