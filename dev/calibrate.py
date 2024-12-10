import time
import os
from datetime import datetime

os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "hide"
import pygame.midi
from icecream import ic
import nidaqmx
from nidaqmx.constants import TerminalConfiguration
import matplotlib.pyplot as plt
from matplotlib.backends.backend_pdf import PdfPages
from scipy.stats import linregress
import numpy as np
from tqdm import tqdm


def read_all_voltages(task, num_channels):
    # Read all channels in one go
    data = task.read(number_of_samples_per_channel=1)
    return data


def read_voltages():
    # Specify channels to read
    channels_to_read = list(range(8))  # Reading ai0 to ai7

    # Create and configure the task outside the loop
    with nidaqmx.Task() as task:
        # Add multiple channels to the task with single-ended configuration
        for ch in channels_to_read:
            task.ai_channels.add_ai_voltage_chan(
                f"Dev1/ai{ch}",
                terminal_config=TerminalConfiguration.RSE,  # Single-ended configuration
            )
        task.timing.cfg_samp_clk_timing(10)

        # Read voltages
        voltages = read_all_voltages(task, len(channels_to_read))
        return [v[0] for v in voltages]


def float_to_14bit(float_value):
    if not (0 <= float_value <= 1):
        raise ValueError("Input float must be in the range 0 to 1.")

    # Scale float to 14-bit range (0 to 16383)
    scaled_value = round(float_value * 16383)

    # Extract the high 7 bits (most significant bits)
    high7_bits = (scaled_value >> 7) & 0x7F

    # Extract the low 7 bits (least significant bits)
    low7_bits = scaled_value & 0x7F

    # Return the two 7-bit numbers
    return high7_bits, low7_bits


def send_voltage(output, channel, voltage):
    """
    Send the scaled voltage as a MIDI SysEx message.

    :param channel: MIDI channel (1-indexed)
    :param scaled_voltage: A float between 0 and 1
    """
    # find midi output with "yoctocore" in name

    scaled_voltage = (voltage + 5.0) / 15.0
    # Convert the scaled voltage to 14-bit
    high7_bits, low7_bits = float_to_14bit(scaled_voltage)

    # Create the Control Change message and send it with pygame
    message = [0xB0 + channel - 1 + 8, high7_bits, low7_bits]

    # ic(output, message)
    output.write_short(message[0], message[1], message[2])


def set_all_voltage(voltage):
    not_done = True
    while not_done:
        output = None
        try:
            pygame.midi.init()
            for device_id in range(pygame.midi.get_count()):
                interface, name, is_input, is_output, opened = (
                    pygame.midi.get_device_info(device_id)
                )
                if not is_output:
                    continue
                if "yoctocore" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            for i in range(8):
                send_voltage(output, i + 1, voltage)
            time.sleep(0.05)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


def set_voltage(i, voltage):
    not_done = True
    while not_done:
        output = None
        try:
            pygame.midi.init()
            for device_id in range(pygame.midi.get_count()):
                interface, name, is_input, is_output, opened = (
                    pygame.midi.get_device_info(device_id)
                )
                if not is_output:
                    continue
                if "yoctocore" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            send_voltage(output, i + 1, voltage)
            time.sleep(0.05)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


def run_calibration(output_num):
    voltages = [
        -4,
        -3,
        -2,
        -1,
        0,
        0.5,
        1,
        1.5,
        2,
        2.5,
        3,
        3.5,
        4,
        4.5,
        5,
        6,
        7,
        8,
        9,
        10,
    ]
    voltages = [-4, 0, 5]
    num_trials = 5
    measured = np.zeros((len(voltages), num_trials))
    for trial in range(num_trials):
        for i, voltage in enumerate(voltages):
            set_voltage(output_num, voltage)
            measured_voltages = read_voltages()
            measured[i, trial] = measured_voltages[i]

    # save voltages and measured voltages to file
    np.save(f"voltages_{output_num}.npy", voltages)
    np.save(f"measured_{output_num}.npy", measured)


def run_one_by_one():
    for i in range(8):
        print(f"Running calibration for channel {i}")
        # wait for user to press enter
        input()
        run_calibration(i)


# def run():
#     num_channels = 8  # Assuming 8 channels
#     num_trials = 5
#     # make numpy matrix of measured voltages
#     measured = np.zeros((len(voltages), num_channels, num_trials))
#     for trial in tqdm(range(num_trials)):
#         for i, voltage in enumerate(voltages):
#             set_all_voltage(voltage)
#             measured_voltages = read_voltages()
#             measured[i, :, trial] = measured_voltages

#     # Create 8 plots: one for each channel
#     fig, axs = plt.subplots(
#         4, 2, figsize=(8.5, 11)
#     )  # Adjust figure size for better layout
#     fig.subplots_adjust(
#         left=0.1, right=0.9, top=0.9, bottom=0.1, hspace=0.5, wspace=0.4
#     )

#     axs = axs.flatten()  # Flatten to simplify indexing

#     for i in range(num_channels):
#         # Gather all trial data
#         x = np.repeat(voltages, num_trials)  # Repeat voltages for all trials
#         y = measured[:, i, :].flatten()  # Flatten measured data for channel i

#         # Plot scatter points
#         axs[i].scatter(x, y, color="black", label="Measured Data")

#         # Perform linear regression
#         slope, intercept, r_value, p_value, std_err = linregress(x, y)
#         regression_line = slope * x + intercept

#         # Plot regression line and confidence intervals
#         axs[i].plot(x, regression_line, color="red", label="Regression Line")
#         axs[i].fill_between(
#             x,
#             regression_line - 2.54 * std_err,
#             regression_line + 2.54 * std_err,
#             color="red",
#             alpha=0.2,
#             label="95% Confidence Interval",
#         )

#         # Set plot titles and labels
#         axs[i].set_title(f"Channel {i+1}: {slope:.3f}x + {intercept:.3f}")
#         axs[i].set_xlabel("Voltage")
#         axs[i].set_ylabel("Measured Voltage")
#         # axs[i].legend(loc="best")

#     plt.suptitle(f"Calibration on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
#     with PdfPages("channel_plots.pdf") as pdf:
#         fig.tight_layout(rect=[0.025, 0.025, 0.975, 0.975])
#         pdf.savefig(fig)  # Save the figure to the PDF
#         plt.close(fig)  # Close the figure after saving

run_one_by_one()
