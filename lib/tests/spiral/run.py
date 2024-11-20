import numpy as np
import matplotlib.pyplot as plt


# Modify the function to flip the x-axis for the square spiral
def square_spiral_three_turns_flipped_x(t):
    n_turns = 3  # Number of turns
    max_radius = 0.5  # Maximum radius (since it's in the unit square)
    x, y = [], []
    for t_ in t:
        layer = int(t_ * n_turns * 4)  # Determine which "side" of the spiral
        pos_in_layer = (t_ * n_turns * 4) % 1  # Position within the layer (0 to 1)
        radius = max_radius * (1 - t_)
        if layer % 4 == 0:  # Right side, moving up
            x.append(0.5 - radius)  # Flip x-axis
            y.append(0.5 + radius * (2 * pos_in_layer - 1))
        elif layer % 4 == 1:  # Top side, moving left
            x.append(0.5 - radius * (1 - 2 * pos_in_layer))  # Flip x-axis
            y.append(0.5 + radius)
        elif layer % 4 == 2:  # Left side, moving down
            x.append(0.5 + radius)  # Flip x-axis
            y.append(0.5 + radius * (1 - 2 * pos_in_layer))
        else:  # Bottom side, moving right
            x.append(0.5 + radius * (1 - 2 * pos_in_layer))  # Flip x-axis
            y.append(0.5 - radius)
    return x, y


# Define a function to map input (0 to 1023) to the x-y coordinates along the spiral
def spiral_coordinate(input_value):
    """
    Takes an input from 0 to 1023 and returns the corresponding (x, y) coordinate
    along the square spiral line.

    Parameters:
    input_value (int): An integer between 0 and 1023.

    Returns:
    tuple: (x, y) coordinate on the spiral.
    """
    if not (0 <= input_value <= 1023):
        raise ValueError("Input value must be between 0 and 1023.")

    # Normalize the input to a value between 0 and 1
    t_normalized = input_value / 1023

    # Generate the full spiral coordinates
    t_values = np.linspace(0, 1, 1024)
    x_values, y_values = square_spiral_three_turns_flipped_x(t_values)

    # Find the closest point corresponding to the input value
    index = int(t_normalized * 1023)
    return x_values[index], y_values[index]


# Generate points for the flipped x-axis spiral
t_values = np.linspace(0, 1, 1000)
x_values_flipped, y_values_flipped = square_spiral_three_turns_flipped_x(t_values)

# Plot the square spiral with three turns and flipped x-axis
plt.figure(figsize=(6, 6))
plt.plot(
    x_values_flipped,
    y_values_flipped,
    label="Square Spiral (3 Turns, Flipped X)",
    color="purple",
)
plt.gca().set_aspect("equal")
plt.title("Square Spiral with Three Turns (Flipped X-Axis)")
plt.xlabel("x")
plt.ylabel("y")
plt.xlim(0, 1)
plt.ylim(0, 1)
plt.grid(True)
plt.legend()
plt.show()

# plot sampled
sampled_x = []
sampled_y = []
for i in range(1024):
    x, y = spiral_coordinate(i)
    sampled_x.append(x)
    sampled_y.append(y)

plt.gca().set_aspect("equal")
plt.plot(sampled_x, sampled_y, "o", label="Sampled Points")
plt.show()
