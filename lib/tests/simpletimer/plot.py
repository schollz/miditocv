# warnings off
import warnings

warnings.filterwarnings("ignore")
import matplotlib.pyplot as plt
import pandas as pd

# Read the CSV file
data = pd.read_csv(
    "simple_timer_output.csv", header=None, names=["Time", "Timer", "State"]
)

# Separate data for Timer 1 and Timer 2
timer1_data = data[data["Timer"] == 1]["State"].reset_index(drop=True)
timer2_data = data[data["Timer"] == 2]["State"].reset_index(drop=True)
timer3_data = data[data["Timer"] == 3]["State"].reset_index(drop=True)
# Create the plot
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 6), sharex=True)

# Plot Timer 1 as a square wave
ax1.step(
    data[data["Timer"] == 1]["Time"].reset_index(drop=True), timer1_data, where="post"
)
ax1.set_title("Timer 1 (237 -> 90 -> 190 BPM)")
ax1.set_ylabel("State")
ax1.set_ylim(0, 1)
# fill underneath timer 1
ax1.fill_between(
    data[data["Timer"] == 1]["Time"].reset_index(drop=True),
    timer1_data,
    step="post",
    alpha=0.3,
)

# Plot Timer 2 as a square wave
ax2.step(
    data[data["Timer"] == 2]["Time"].reset_index(drop=True), timer2_data, where="post"
)
ax2.set_title("Timer 2 (90 -> 190 BPM)")
ax2.set_ylabel("State")
ax2.set_xlabel("Time Step")
ax2.set_ylim(0, 1.0)
# fill underneath timer 2
ax2.fill_between(
    data[data["Timer"] == 2]["Time"].reset_index(drop=True),
    timer2_data,
    step="post",
    alpha=0.3,
)

# Plot Timer 3 as a square wave
ax3.step(
    data[data["Timer"] == 3]["Time"].reset_index(drop=True), timer3_data, where="post"
)
ax3.set_title("Timer 3 (190 BPM)")
ax3.set_ylabel("State")
ax3.set_xlabel("Time Step")
ax3.set_ylim(0, 1.0)
# fill underneath timer 3
ax3.fill_between(
    data[data["Timer"] == 3]["Time"].reset_index(drop=True),
    timer3_data,
    step="post",
    alpha=0.3,
)


# Display the plot
plt.tight_layout()
plt.show()
