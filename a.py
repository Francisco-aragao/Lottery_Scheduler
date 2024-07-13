import pandas as pd
import matplotlib.pyplot as plt
import re
import numpy as np

df = pd.read_csv("./testtickets.csv")
#y = []
for col in df:
    tickets = int(re.search(r'\d+', col).group())
    length = len(df[col])
    plt.plot(range(0, length), df[col].values, label=f"{tickets} {"tickets" if tickets != 1 else "ticket"}")
    plt.text(length - 10, df[col].values[-1], f"{df[col].values[-1]}")
    plt.scatter([length - 1], [df[col].values[-1]])
    #y.append(df[col].values[-1])
    #plt.hlines([df[col].values[-1]], 0, [length - 1], linestyles="dashed")

plt.title("Lottery scheduler results")
plt.legend()
plt.xlim(xmin=0)
plt.ylim(ymin=0)
plt.xlabel("Reading")
plt.ylabel("Ticks")
#locs, labels = plt.yticks()
#plt.yticks(np.append(locs, y))
plt.show()