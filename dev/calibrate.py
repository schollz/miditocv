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
import click


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
    print(sysex_string)
    sysex_data = sysex_string.encode("utf-8")
    sysex_data = b"\xF0" + sysex_data + b"\xF7"
    output.write_sys_ex(pygame.midi.time(), sysex_data)


def send_voltage(output, channel, voltage):
    send_sysex(output, f"setvolt_{channel:d}_{voltage:.5f}")


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
            send_voltage(output, i, voltage)
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
            sysex_string = f"cali_{channel:d}_{slope:.5f}_{intercept:.5f}"
            print(sysex_string)
            send_sysex(output, sysex_string)
            time.sleep(0.005)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


def send_use_raw(use_raw=False):
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
            sysex_string = f"useraw 1"
            if not use_raw:
                sysex_string = f"useraw 0"
            # print(sysex_string)
            send_sysex(output, sysex_string)
            time.sleep(0.005)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


NUM_TRIALS = 1
NUM_POINTS = 30


def run_calibration(id, output_num, use_raw):
    send_use_raw(use_raw)
    voltages = np.linspace(-4.5, 9.5, NUM_POINTS)
    measured = np.zeros((len(voltages), NUM_TRIALS))
    for trial in tqdm(range(NUM_TRIALS)):
        for i, voltage in enumerate(voltages):
            set_voltage(output_num, voltage)
            for j in range(8):
                if j != output_num and random.random() < 0.01:
                    set_voltage(j, np.random.uniform(-5, 10))
            measured_voltages = read_voltages()
            measured[i, trial] = measured_voltages[0]

    mode = "raw" if use_raw else "volt"
    # create a folder with the id if it doesn't already exist
    if not os.path.exists(id):
        os.mkdir(id)
    np.save(f"{id}/voltages_{output_num}_{mode}.npy", voltages)
    np.save(f"{id}/measured_{output_num}_{mode}.npy", measured)
    print(f"Calibration for channel {output_num} complete")
    # calculate the slope and intercept
    x = np.repeat(voltages, NUM_TRIALS)
    y = measured.flatten()
    slope, intercept, r_value, p_value, std_err = linregress(x, y)
    print(f"Slope: {slope}, Intercept: {intercept}")
    if use_raw:
        # send the calibration values to the device
        send_calibration(output_num, slope, intercept)


def create_printout(id):
    num_channels = 12
    for mode in ["raw", "volt"]:
        fig, axs = plt.subplots(6, 2, figsize=(8.5, 11))
        fig.subplots_adjust(
            left=0.1, right=0.9, top=0.9, bottom=0.1, hspace=0.5, wspace=0.4
        )

        axs = axs.flatten()
        for i in range(num_channels):
            channel_num = i + 1
            try:
                voltages = np.load(f"{id}/voltages_{channel_num}_{mode}.npy")
                measured = np.load(f"{id}/measured_{channel_num}_{mode}.npy")
                y = measured.flatten()
                x = np.repeat(voltages, measured.shape[1])
                # axs[i].scatter(x, y, label=f"Measured Data ({mode})", color="black")
                slope, intercept, r_value, p_value, std_err = linregress(x, y)

                regression_line = slope * x + intercept
                # total_error is calculated as the average relavtive error
                total_error = (
                    np.sum(np.divide(np.abs(x - y), np.abs(x))) * 100.0 / len(x)
                )
                y2 = np.divide((x - y), np.abs(x))
                # plot vertical line from each point to 0 on the y-axis
                for j in range(len(x)):
                    if x[j] - y[j] > 0:
                        axs[i].plot(
                            [x[j], x[j]], [x[j] - y[j], 0], color="red", alpha=0.8
                        )
                    else:
                        axs[i].plot(
                            [x[j], x[j]], [x[j] - y[j], 0], color="blue", alpha=0.8
                        )

                axs[i].plot(
                    x,
                    x - y,
                    label=f"Regression ({mode})",
                    color="black",
                    linewidth=2,
                    alpha=0.9,
                    linestyle="none",
                    marker="o",
                    markersize=3,
                    markerfacecolor="black",
                    markeredgecolor="black",
                )

                # plot 0 line
                axs[i].plot(
                    x, np.zeros_like(x), label="Zero Error", color="black", alpha=0.5
                )

                intercept_string = f"{intercept:.3f}"
                if intercept < 0:
                    intercept_string = f"-{-intercept:.3f}"
                else:
                    intercept_string = f"+{intercept:.3f}"
                if mode == "raw":
                    axs[i].set_title(
                        f"Channel {channel_num}: {slope:.3f}x{intercept_string}, error={total_error:.1f}%"
                    )
                else:
                    axs[i].set_title(f"Channel {channel_num}: error={total_error:.1f}%")
                axs[i].set_xlim(-5, 10)
                axs[i].set_ylim(-0.25, 0.25)
                axs[i].set_xlabel("Set Voltage")
                axs[i].set_ylabel("Error")

                # axs[i].legend(loc="best")
            except FileNotFoundError:
                pass

        if mode == "raw":
            plt.suptitle(
                f"Calibration correction on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        else:
            plt.suptitle(
                f"Testing correction on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        with PdfPages(f"{id}/channel_plots_{mode}.pdf") as pdf:
            fig.tight_layout(rect=[0.025, 0.025, 0.975, 0.975])
            pdf.savefig(fig)
            plt.close(fig)


def run_one_by_one(id, start, num, test_only=False):
    first = True
    # check start to start + num inclusively
    for i in range(start, num + start):
        if first:
            first = False
        else:
            print(f"Running calibration for channel {i}, press enter to continue")
            input()
        if not test_only:
            run_calibration(id, i, True)
        run_calibration(id, i, False)
        # reset all of them
        for j in range(8):
            set_voltage(j, -10)
    create_printout(id)


@click.command()
@click.argument("id", required=True)
@click.argument("start", required=False, type=int, default=1)
@click.argument("num", required=False, type=int, default=8)
@click.option("--test", is_flag=True, help="Run in test mode")
def main(id, start, num, test):
    if test:
        create_printout(id)
    else:
        run_one_by_one(id, start, num, test)


if __name__ == "__main__":
    main()
