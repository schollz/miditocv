import pandas as pd
import matplotlib.pyplot as plt

# Load the CSV file
data = pd.read_csv("adsr_output.csv")

# Create the plot
plt.figure(figsize=(10, 6))
plt.plot(data["time_ms"], data["level"], label="ADSR Level", linewidth=2)
plt.xlabel("Time (ms)")
plt.ylabel("Level")
plt.title("ADSR Envelope")
plt.grid(True)
plt.legend()

# Mark the states on the plot
states = ["Idle", "Attack", "Decay", "Sustain", "Release"]
for state, name in enumerate(states):
    state_data = data[data["state"] == state]
    if not state_data.empty:
        plt.fill_between(
            state_data["time_ms"], state_data["level"], alpha=0.1, label=name
        )

plt.legend()
plt.show()
