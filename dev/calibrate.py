import time
import os
import sys
from datetime import datetime
import random

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
    data = task.read(number_of_samples_per_channel=1)
    return data


def read_voltages():
    channels_to_read = list(range(8))
    with nidaqmx.Task() as task:
        for ch in channels_to_read:
            task.ai_channels.add_ai_voltage_chan(
                f"Dev1/ai{ch}",
                terminal_config=TerminalConfiguration.RSE,
            )
        task.timing.cfg_samp_clk_timing(10)
        voltages = read_all_voltages(task, len(channels_to_read))
        return [v[0] for v in voltages]


def send_sysex(output, sysex_string):
    sysex_data = sysex_string.encode("utf-8")
    sysex_data = b"\xF0" + sysex_data + b"\xF7"
    output.write_sys_ex(pygame.midi.time(), sysex_data)


def send_voltage(output, channel, voltage):
    send_sysex(output, f"setvolt_{channel:d}_{voltage:.4f}")


def send_raw_voltage(output, channel, voltage):
    send_sysex(output, f"setraw_{channel:d}_{voltage:.4f}")


def set_voltage(i, voltage, use_raw=False):
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
            if use_raw:
                send_raw_voltage(output, i + 1, voltage)
            else:
                send_voltage(output, i + 1, voltage)
            time.sleep(0.005)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


def send_calibration(channel, slope, intercept):
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
            sysex_string = f"cali_{channel-1:d}_{slope:.4f}_{intercept:.4f}"
            print(sysex_string)
            send_sysex(output, sysex_string)
            time.sleep(0.005)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


NUM_TRIALS = 1
NUM_POINTS = 30


def run_calibration(output_num, use_raw):
    voltages = np.linspace(-4.5, 9.5, NUM_POINTS)
    measured = np.zeros((len(voltages), NUM_TRIALS))
    for trial in tqdm(range(NUM_TRIALS)):
        for i, voltage in enumerate(voltages):
            set_voltage(output_num, voltage, use_raw)
            for j in range(8):
                if j != output_num and random.random() < 0.01:
                    set_voltage(j, np.random.uniform(-5, 10), use_raw)
            measured_voltages = read_voltages()
            measured[i, trial] = measured_voltages[0]

    mode = "raw" if use_raw else "volt"
    np.save(f"voltages_{output_num}_{mode}.npy", voltages)
    np.save(f"measured_{output_num}_{mode}.npy", measured)
    if use_raw:
        print(f"Raw calibration for channel {output_num+1} complete")
        # calculate the slope and intercept
        x = np.repeat(voltages, NUM_TRIALS)
        y = measured.flatten()
        slope, intercept, r_value, p_value, std_err = linregress(x, y)
        print(f"Slope: {slope}, Intercept: {intercept}")
        # send the calibration values to the device
        send_calibration(output_num + 1, slope, intercept)


def create_printout():
    num_channels = 8
    for mode in ["raw", "volt"]:
        fig, axs = plt.subplots(4, 2, figsize=(8.5, 11))
        fig.subplots_adjust(
            left=0.1, right=0.9, top=0.9, bottom=0.1, hspace=0.5, wspace=0.4
        )

        axs = axs.flatten()
        for i in range(num_channels):
            try:
                voltages = np.load(f"voltages_{i}_{mode}.npy")
                measured = np.load(f"measured_{i}_{mode}.npy")
                y = measured.flatten()
                x = np.repeat(voltages, measured.shape[1])
                axs[i].scatter(x, y, label=f"Measured Data ({mode})", color="black")
                slope, intercept, r_value, p_value, std_err = linregress(x, y)

                regression_line = slope * x + intercept
                total_error = np.sum(np.abs(y - x))
                axs[i].plot(
                    x,
                    regression_line,
                    label=f"Regression ({mode})",
                    color="red",
                    linewidth=2,
                    alpha=0.5,
                    linestyle="--",
                )

                intercept_string = f"{intercept:.4f}"
                if intercept < 0:
                    intercept_string = f"-{-intercept:.4f}"
                else:
                    intercept_string = f"+{intercept:.4f}"
                axs[i].set_title(
                    f"Channel {i+1}: {slope:.4f}x{intercept_string}, error={total_error:.4f}"
                )
                axs[i].set_xlim(-5, 10)
                axs[i].set_ylim(-5, 10)
                axs[i].set_xlabel("Set Voltage")
                axs[i].set_ylabel("Measured Voltage")
                # axs[i].legend(loc="best")
            except FileNotFoundError:
                pass

        if mode == "raw":
            plt.suptitle(
                f"Calibration on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        else:
            plt.suptitle(f"Testing on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
        with PdfPages(f"channel_plots_{mode}.pdf") as pdf:
            fig.tight_layout(rect=[0.025, 0.025, 0.975, 0.975])
            pdf.savefig(fig)
            plt.close(fig)


def run_one_by_one(start,test_only=False):
    first = True
    for i in range(8):
        if i + 1 < start:
            continue
        if first:
            first = False
        else:
            print(f"Running calibration for channel {i+1}, press enter to continue")
            input()
        if not test_only:
            run_calibration(i, True)
        run_calibration(i, False)
        create_printout()
        # reset all of them
        for j in range(8):
            set_voltage(j, -10, False)


if __name__ == "__main__":
    if len(sys.argv) >= 2:
        start_channel = int(sys.argv[1])

        run_one_by_one(start_channel,sys.argv[2]=="test")
    else:
        create_printout()
