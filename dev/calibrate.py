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
    channels_to_read = list(range(0, 8))
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


def set_voltages(voltages):
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
                if "miditocv" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            for channel, voltage in enumerate(voltages):
                send_sysex(output, f"setvolt_{channel+1:d}_{voltage:.5f}")
            time.sleep(0.025)
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
                if "miditocv" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            send_voltage(output, i, voltage)
            time.sleep(0.025)
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
                if "miditocv" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            sysex_string = f"cali_{channel:d}_{slope:.5f}_{intercept:.5f}"
            print(sysex_string)
            send_sysex(output, sysex_string)
            time.sleep(0.025)
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
                if "miditocv" in name.decode("utf-8"):
                    output = pygame.midi.Output(device_id)
                    break
            sysex_string = f"useraw 1"
            if not use_raw:
                sysex_string = f"useraw 0"
            # print(sysex_string)
            send_sysex(output, sysex_string)
            time.sleep(0.025)
            not_done = False
        except:
            pygame.midi.quit()
            time.sleep(0.05)
            continue


NUM_TRIALS = 5
NUM_POINTS = 60


def run_calibration(id, use_raw):
    # output_num is 1-indexed channel number
    send_use_raw(use_raw)
    num_outputs = 8
    voltages = np.linspace(-4.5, 9.5, NUM_POINTS)
    voltage_settings = np.zeros((num_outputs, len(voltages)))
    measured = np.zeros((num_outputs, len(voltages), NUM_TRIALS))
    # shuffle voltages for each output for testing
    for i in range(num_outputs):
        voltages = np.random.permutation(voltages)
        voltage_settings[i, :] = voltages

    for trial in tqdm(range(NUM_TRIALS)):
        for i in range(NUM_POINTS):
            test_voltages = []
            for output in range(num_outputs):
                test_voltages.append(voltage_settings[output, i])
            set_voltages(test_voltages)
            measured_voltages = read_voltages()
            for j in range(num_outputs):
                measured[j, i, trial] = measured_voltages[j]
            print(measured[:, i, trial])

    mode = "raw" if use_raw else "volt"

    # create a folder with the id if it doesn't already exist
    if not os.path.exists("calibrations"):
        os.mkdir("calibrations")
    if not os.path.exists(f"calibrations/{id}"):
        os.mkdir(f"calibrations/{id}")

    for i in range(num_outputs):
        output_num = i + 1
        voltages = voltage_settings[i, :]
        voltages_measured = measured[i, :, :]
        np.savez(
            f"calibrations/{id}/data_{output_num}_{mode}.npz",
            voltages=voltages,
            measured=voltages_measured,
        )
        print(f"Calibration for channel {output_num} complete")
        # calculate the slope and intercept
        x = np.repeat(voltages, NUM_TRIALS)
        y = voltages_measured.flatten()
        slope, intercept, _, _, _ = linregress(x, y)
        print(f"Slope: {slope}, Intercept: {intercept}")
        if use_raw:
            # send the calibration values to the device
            send_calibration(output_num, slope, intercept)


def create_printout(id, num_channels=8):
    recalibration_text = ""

    slopes = np.zeros(num_channels)
    intercepts = np.zeros(num_channels)
    for mode in ["raw", "volt"]:
        fig, axs = plt.subplots(int(num_channels / 2), 2, figsize=(8.5, 11))
        fig.subplots_adjust(
            left=0.1, right=0.9, top=0.9, bottom=0.1, hspace=0.5, wspace=0.4
        )

        axs = axs.flatten()
        for i in range(num_channels):
            channel_num = i + 1
            try:
                data = np.load(f"calibrations/{id}/data_{channel_num}_{mode}.npz")
                voltages = data["voltages"]
                measured = data["measured"]
                y = measured.flatten()
                x = np.repeat(voltages, measured.shape[1])
                # axs[i].scatter(x, y, label=f"Measured Data ({mode})", color="black")
                slope, intercept, r_value, p_value, std_err = linregress(x, y)
                if mode == "raw":
                    slopes[i] = slope
                    intercepts[i] = intercept

                if mode == "raw":
                    sysex_string = f"cali_{channel_num:d}_{slope:.5f}_{intercept:.5f}"
                    random_milliseconds = 1000 + i * 1000
                    recalibration_text += f"setTimeout(() => send_sysex('{sysex_string}'), {random_milliseconds});\n"

                # total_error is calculated as the average relative error
                total_error = np.sum(np.divide(np.abs(x - y), 1)) * 100.0 / len(x)
                # plot vertical line from each point to 0 on the y-axis
                for j in range(len(x)):
                    cents = 1200 * (x[j] - y[j])
                    if cents > 0:
                        axs[i].plot([x[j], x[j]], [cents, 0], color="red", alpha=0.8)
                        # plot dot
                        axs[i].plot(
                            x[j],
                            cents,
                            color="red",
                            linestyle="none",
                            marker="o",
                            markersize=3,
                            markerfacecolor="red",
                            markeredgecolor="red",
                        )
                    else:
                        axs[i].plot([x[j], x[j]], [cents, 0], color="blue", alpha=0.8)
                        # plot dot
                        axs[i].plot(
                            x[j],
                            cents,
                            color="blue",
                            linestyle="none",
                            marker="o",
                            markersize=3,
                            markerfacecolor="blue",
                            markeredgecolor="blue",
                        )

                # plot 0 line
                axs[i].plot(
                    x, np.zeros_like(x), label="Zero Error", color="black", alpha=0.5
                )

                intercept_string = f"{intercepts[i]:.3f}"
                if intercepts[i] < 0:
                    intercept_string = f"{intercepts[i]:.3f}"
                else:
                    intercept_string = f"+{intercepts[i]:.3f}"
                axs[i].set_title(
                    f"Channel {channel_num}: {slopes[i]:.3f}x{intercept_string}, error={total_error:.1f}%"
                )
                axs[i].set_xlim(-5, 10)
                axs[i].set_ylim(-200, 200)
                axs[i].set_xlabel("Set Voltage")
                axs[i].set_ylabel("Pitch error (cents)")

                # axs[i].legend(loc="best")
            except FileNotFoundError:
                pass

        if mode == "raw":
            plt.suptitle(
                f"[#{id}] Calibration on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        else:
            plt.suptitle(
                f"[#{id}] Calibrated on {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        title = "calibrated" if mode == "raw" else "Calibration"
        with PdfPages(f"calibrations/{id}/{id}_{title}.pdf") as pdf:
            fig.tight_layout(rect=[0.025, 0.025, 0.975, 0.975])
            pdf.savefig(fig)
            plt.close(fig)
    with open(f"calibrations/{id}/recalibration.js", "w") as f:
        f.write(recalibration_text)


def run_all_calibration(id, test_only=False):
    if not test_only:
        run_calibration(id, True)
    run_calibration(id, False)
    # reset all of them
    for j in range(1, 8 + 1):
        set_voltage(j, -10)
    if not test_only:
        create_printout(id)


@click.command()
@click.argument("id", required=True)
@click.option("--test", is_flag=True, help="Run in test mode")
@click.option("--print", is_flag=True, help="Print the calibration results")
def main(id, test, print):
    if print:
        create_printout(id)
    else:
        run_all_calibration(id, test)


if __name__ == "__main__":
    main()
