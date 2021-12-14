# import matplotlib and csv
import matplotlib.pyplot as plt
import csv
import sys

# arrays for data
time = []
buffer = []

# get file name
file_name = str(sys.argv[1])

# read data
with open(file_name) as data_file:
    csv_reader = csv.reader(data_file)

    # get headers
    header = next(csv_reader)

    for row in csv_reader:
        time.append(float(row[0]))
        buffer.append(float(row[1]))

# display data
plt.plot(time, buffer)
plt.xlabel(header[0])
plt.ylabel(header[1])
plt.show()
