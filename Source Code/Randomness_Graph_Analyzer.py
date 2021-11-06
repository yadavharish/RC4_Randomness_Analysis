import matplotlib.pyplot as plt


for k in range(3, 13):
    file = open("./randomnessOnUsing" + str(2**k) + "Counters.txt", 'r')
    randomnessValues = file.read()
    file.close()

    randomnessValuesList = randomnessValues.split("\n")
    randomnessValuesMatrix = []
    for i in range(len(randomnessValuesList)):
        randomnessValuesMatrix.append(randomnessValuesList[i].split(" "))
    randomnessValuesMatrix.pop()

    for i in range(len(randomnessValuesMatrix)):
        randomnessValuesMatrix[i].pop()    
        for j in range(len(randomnessValuesMatrix[0])):
            randomnessValuesMatrix[i][j] = float(randomnessValuesMatrix[i][j])

    streamLengths = [2, 4, 8, 32, 128, 1024]

    xpoints = []
    ypoints = []
    noOfBitsFlipped = []

    if(k <= 4):
        irange = range(6)
    elif(k <= 5):
        irange = range(1, 6)
    elif(k <= 7):
        irange = range(2, 6)
    elif(k <= 8):
        irange = range(3, 6)
    elif(k <= 10):
        irange = range(4, 6)
    else:
        irange = range(5, 6)

    for i in range(1, 33):
        xpoints.append(i)

    for i in irange:
        ypoints.clear()
        for j in range(len(randomnessValuesMatrix[i])):
            ypoints.append(randomnessValuesMatrix[i][j])
        plt.plot(xpoints, ypoints, label=str(streamLengths[i]) + "Byte Stream Length")

    plt.xlabel("Number of bits flipped")
    plt.ylabel("Randomness")
    # plt.xticks(range(1, 33))
    plt.title("Randomness using " + str(2**k) + " counters")
    plt.legend(loc='best')
    plt.show()
