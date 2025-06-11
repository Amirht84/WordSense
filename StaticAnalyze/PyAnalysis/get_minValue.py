import numpy as np

size = 371345 
data = list(range(size))
data = [0] * size
i = 0
with open("../words_alpha.txt", 'r') as file:
    for line in file:
        data[i] = len(line.strip())
        i += 1

print(f"median: {np.median(data)}")
print(f"Q1: {np.percentile(data, 25)}")
print(f"Q2: {np.percentile(data, 75)}")
