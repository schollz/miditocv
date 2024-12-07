import time
import os

import pygame.midi
from icecream import ic
import nidaqmx
from nidaqmx.constants import TerminalConfiguration
import matplotlib.pyplot as plt
from scipy.stats import linregress
import numpy as np


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


def run():
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
    num_channels = 8  # Assuming 8 channels
    num_trials = 5
    # make numpy matrix of measured voltages
    measured = np.zeros((len(voltages), num_channels, num_trials))
    for trial in range(num_trials):
        for i, voltage in enumerate(voltages):
            set_all_voltage(voltage)
            measured_voltages = read_voltages()
            measured[i, :, trial] = measured_voltages

    # make 8 plots showing a scatter plot for each channel
    (fig, axs) = plt.subplots(2, 4)
    for i in range(num_channels):
        # concatenate all trials
        x = []
        y = []
        for trial in range(num_trials):
            x.extend(voltages)
            y.extend(measured[:, i, trial])

        # plot with confidence intervals
        axs[i // 4, i % 4].scatter(x, y, color="black")

        # create regression with 95% confidence interval
        slope, intercept, r_value, p_value, std_err = linregress(x, y)
        x = np.array(x)
        axs[i // 4, i % 4].plot(x, slope * x + intercept, color="red")
        axs[i // 4, i % 4].fill_between(
            x,
            slope * x + intercept - 1.96 * std_err,
            slope * x + intercept + 1.96 * std_err,
            alpha=0.2,
            color="red",
        )
        axs[i // 4, i % 4].set_title(f"Channel {i+1} {slope:.3f}x + {intercept:.3f}")
        axs[i // 4, i % 4].set_xlabel("Voltage")
        axs[i // 4, i % 4].set_ylabel("Measured Voltage")

    plt.show()


run()
