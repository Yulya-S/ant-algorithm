import matplotlib.pyplot as plt

x = []
y = []
with open("result4.txt", "r") as file:
    for i in file:
        data = i.split(" ")
        x.append(int(data[1]))
        y.append(int(data[0]))

plt.plot(x, y)
plt.show()
