import matplotlib.pyplot as plt
import pandas as pd

# Load the CSV data
data = pd.read_csv("wave.csv", header=None, names=["Time", "Value"])

# Plot the triangle wave
plt.plot(data["Time"], data["Value"])
plt.title("Triangle Wave")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.grid()
plt.show()
