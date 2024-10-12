import matplotlib.pyplot as plt
import csv

time = []
temps = []

with open('tempVtime.csv', 'r') as file:
    reader = csv.reader(file)
    next(reader)
    for row in reader:
        time.append(float(row[0]))
        temps.append(float(row[1]))


plt.plot(time, temps, marker='o', linestyle='-', color='b')

plt.xlabel('Time (s)')
plt.ylabel('Temperature (°F)')
plt.title('Temperature vs Time')

plt.grid(True)
plt.show()