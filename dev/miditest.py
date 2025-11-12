#!/usr/bin/env python3
import time, mido, random

port = next((p for p in mido.get_output_names() if "miditocv" in p.lower()), None)
if not port:
    raise SystemExit("no miditocv port found")

with mido.open_output(port) as out:
    random_note = random.randint(60, 122)
    print(f"Sending note_on {random_note} to {port}")
    out.send(mido.Message("note_on", note=random_note, velocity=100))
    time.sleep(3)
    print(f"Sending note_off {random_note} to {port}")
    out.send(mido.Message("note_off", note=random_note, velocity=0))
