minlen = 100
minwords = list()
maxlen = 0
maxwords = list()
maxCount = 0
minCount = 0
with open("../words_alpha.txt", 'r') as file:
    for line in file:
        temp = len(line.strip())
        if(temp < minlen):
            minlen = temp
            minwords.clear()
            minwords = [line.strip()]
            minCount = 1
        elif(temp == minlen):
            minwords.append(line.strip())
            minCount += 1
        if(temp > maxlen):
            maxlen = temp
            maxwords.clear()
            maxwords = [line.strip()]
            maxCount = 1
        elif(temp == maxlen):
            maxwords.append(line.strip())
            maxCount += 1
print(f"max length: {maxlen}")
print(f"max word: {maxwords}")
print(f"min length: {minlen}")
print(f"min word: {minwords}")
