#!/usr/bin/env python3
import os
import sys
import re
import shutil
import subprocess
import time

os.environ["PYGAME_HIDE_SUPPORT_PROMPT"] = "hide"
try:
    import pygame.midi
except Exception as exc:
    print(f"Failed to import pygame.midi: {exc}")
    print("Try: pip install -r requirements.txt")
    sys.exit(1)


def find_miditocv_output():
    for device_id in range(pygame.midi.get_count()):
        interface, name, is_input, is_output, opened = pygame.midi.get_device_info(
            device_id
        )
        if not is_output:
            continue
        if b"miditocv" in name.lower():
            return pygame.midi.Output(device_id)
    return None


def send_sysex(output, sysex_string):
    sysex_data = sysex_string.encode("utf-8")
    sysex_data = b"\xf0" + sysex_data + b"\xf7"
    output.write_sys_ex(pygame.midi.time(), sysex_data)


def sysex_hex(sysex_string):
    data = b"\xf0" + sysex_string.encode("utf-8") + b"\xf7"
    return " ".join(f"{b:02X}" for b in data)


def send_sysex_amidi(sysex_string):
    if shutil.which("amidi") is None:
        return False
    try:
        result = subprocess.run(
            ["amidi", "-l"],
            check=True,
            capture_output=True,
            text=True,
        )
    except Exception:
        return False

    device = None
    for line in result.stdout.splitlines():
        match = re.match(r"^\s*IO\s+(\S+)\s+(.+)$", line)
        if not match:
            continue
        dev, name = match.group(1), match.group(2)
        if "miditocv" in name.lower():
            device = dev
            break

    if device is None:
        return False

    try:
        subprocess.run(
            ["amidi", "-p", device, "-S", sysex_hex(sysex_string)],
            check=True,
        )
    except Exception:
        return False
    return True


def main(argv):
    if len(argv) != 3:
        print("Usage: python dev/send.py <channel> <voltage>")
        return 1

    try:
        channel = int(argv[1])
        voltage = float(argv[2])
    except ValueError:
        print("Invalid arguments. Channel must be int, voltage must be float.")
        return 1

    if channel < 1 or channel > 8:
        print("Channel must be in the range 1-8.")
        return 1

    try:
        pygame.midi.init()
        output = None
        try:
            output = find_miditocv_output()
            if output is not None:
                send_sysex(output, f"setvolt_{channel:d}_{voltage:.5f}")
                time.sleep(0.025)
                return 0
        finally:
            if output is not None:
                output.close()
            pygame.midi.quit()
    except Exception:
        pass

    if send_sysex_amidi(f"setvolt_{channel:d}_{voltage:.5f}"):
        return 0

    print("No MIDI output named 'miditocv' was found.")
    print("Tried pygame.midi and amidi.")
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv))
