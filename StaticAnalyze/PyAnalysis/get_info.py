import numpy as np
SIZE = 370105
FILENAME = "../words_alpha.txt"
def show_menu():
    chopOut = [
            "----------------------",
            "menu:",
            "average",
            "variance",
            "median",
            "q1",
            "q3",
            "min",
            "max",
            "minwords",
            "maxwords",
            "exit",
            "----------------------"

            ]
    for Line in chopOut:
        print(Line)
    return

def get_data(fileName):
    i = 0
    data = [0] * SIZE
    with open(fileName, 'r') as file:
        for line in file:
            data[i] = len(line.strip())
            i += 1
    return data
def find_max(fileName):
    maxWords = list()
    maxie = max(get_data(fileName))
    with open(fileName, 'r') as file:
        for line in file:
            temp = line.strip()
            if(len(temp) == maxie):
                maxWords.append(temp)
    return maxWords
def find_min(fileName):
    minWords = list()
    minie = min(get_data(fileName))
    with open(fileName, 'r') as file:
        for line in file:
            temp = line.strip()
            if(len(temp) == minie):
                minWords.append(temp)

    return minWords

data = get_data(FILENAME)
UsIn = ""
while(UsIn != "exit"):
    show_menu()
    UsIn = input()
    if(UsIn == "average"):
        print(f"average: {np.mean(data)}")
    elif(UsIn == "variance"):
        print(f"variance: {np.var(data)}")
    elif(UsIn == "median"):
        print(f"mendian: {np.median(data)}")
    elif(UsIn == "q1"):
        print(f"Q1: {np.percentile(data, 25)}")
    elif(UsIn == "q3"):
        print(f"Q3: {np.percentile(data, 75)}")
    elif(UsIn == "min"):
        print(f"min: {min(data)}")
    elif(UsIn == "max"):
        print(f"max: {max(data)}")
    elif(UsIn == "maxwords"):
        print(f"maxwords: {find_max(FILENAME)}")
    elif(UsIn == "minwords"):
        print(f"minword: {find_min(FILENAME)}")
