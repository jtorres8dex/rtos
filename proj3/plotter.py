import matplotlib.pyplot as plt
import csv

filename = 'dataF.csv'

pot_values = []
rpm_values = []

MAX_VOLTAGE = 11.1
KV_RATING = 1000.0

# Read the data from the CSV file
with open(filename, 'r') as csvfile:
    csvreader = csv.reader(csvfile)
    for row in csvreader:
        if row[0] == 'potValue':
            pot_values.append(int(row[1]))
        elif row[0] == "RPM":
            rpm_values.append(int(row[1]))


# Plot Potentiometer Value
plt.figure(figsize=(10, 5))
plt.plot(pot_values, label='Potentiometer Value', marker='o')
plt.title('Potentiometer Value Over Time')
plt.xlabel('Data Point Index')
plt.ylabel('Potentiometer Value')
plt.legend()

# Plot RPM
plt.figure(figsize=(10, 5))
plt.plot(rpm_values, label='RPM', marker='x')
plt.title('RPM Over Time')
plt.xlabel('Data Point Index')
plt.ylabel('RPM')
plt.legend()

# Display the plots
plt.show()