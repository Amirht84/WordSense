allSum = 0
count = 0
with open("../words_alpha.txt", 'r') as file:
    for line in file:
        allSum += len(line.strip())
        count += 1

average = allSum / count
print(f"average: {average}")

zigmaSq = 0
with open("../words_alpha.txt", 'r') as file:
    for line in file:
        zigmaSq += pow(len(line.strip()) - average, 2)

variance = zigmaSq / count
print(f"variance: {variance}")
