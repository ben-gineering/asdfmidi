# asdfmidi

asdfmidi is a small standalone C++ application for Linux that turns your
computer keyboard into a MIDI keyboard. It creates a virtual ALSA MIDI
output port that you can connect to a DAW or any other MIDI-aware
application.

The current implementation focuses on:

- Linux (ALSA sequencer for MIDI)
- X11 / Xwayland keyboard input
- CLI-only usage (no GUI controls beyond a tiny X window for focus)

## Features

- Virtual MIDI output port (`asdfmidi`) using ALSA
- QWERTY keyboard mapped to musical notes
- Polyphonic Note On/Off handling with per-key state tracking
- Configurable MIDI channel, base octave, and velocity via CLI flags
- Octave up/down from the keyboard while running

## Requirements

- A Linux system with ALSA and X11
- Development tools:
  - C++17 compiler (g++ or clang++)
  - CMake (>= 3.16 recommended)
  - X11 development headers (`libX11` / `libX11-dev`)
  - ALSA development headers (`libasound2-dev` or distro equivalent)

On many Debian/Ubuntu systems, for example:

```sh
sudo apt install build-essential cmake libx11-dev libasound2-dev
```

## Building

From the project root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4
```

This produces the executable:

- `build/asdfmidi`

## Running

Basic usage:

```sh
./build/asdfmidi
```

Command-line options:

- `--channel N`      MIDI channel (0–15, default `0`)
- `--velocity N`     Velocity (1–127, default `100`)
- `--base-octave N`  Base octave for C (default `4`, i.e. C4 = MIDI 60)
- `--help`           Show help text

Examples:

```sh
./build/asdfmidi --channel 1 --velocity 90
./build/asdfmidi --base-octave 3
```

When you run the program it will:

1. Create a virtual ALSA MIDI output port named `asdfmidi`.
2. Open a small X11 window titled `asdfmidi keyboard`.
3. Start reading key presses and sending MIDI events.

You must give the `asdfmidi keyboard` window focus for it to receive
keypresses.

## Connecting To A DAW

In your DAW (Ardour, Reaper, Bitwig, etc.):

1. Start `asdfmidi`.
2. In the DAW, locate MIDI inputs and enable/route the `asdfmidi` port as a
   MIDI input to a track or instrument.
3. With the `asdfmidi keyboard` window focused, press keys on your computer
   keyboard to play notes.

For quick testing without a DAW you can use ALSA tools such as `aseqdump`:

```sh
aseqdump -p asdfmidi
```

Press keys in the `asdfmidi keyboard` window and you should see MIDI note
messages printed.

## Keyboard Layout

The current default layout uses a common "piano on QWERTY" style:

Lower row (starting at base C):

- White keys: `z x c v b n m` → C D E F G A B
- Black keys: `s d g h j`     → C# D# F# G# A#

Upper row (one octave above):

- White keys: `q w e r t y u`
- Black keys: `2 3 5 6 7`

Octave control:

- `[`  – octave down
- `]`  – octave up

Velocity and channel are fixed for a given run and set with CLI flags.

## Wayland Notes

This project currently uses X11 for keyboard input. On a Wayland desktop
environment, it runs under Xwayland:

- It works as long as the `asdfmidi keyboard` X11 window has focus.
- It cannot see global keypresses while a native Wayland application (for
  example, a Wayland-native DAW window) has focus.

For a future "global keyboard" experience under Wayland you would typically
add an evdev-based backend that reads `/dev/input/event*` directly. That is
not implemented yet.

## Minimal RtMidi Stub

The code under `external/rtmidi/` is **not** the full upstream RtMidi
project. It is a minimal, ALSA-only MIDI output wrapper that exposes a small
subset of the RtMidi API (`RtMidiOut`) sufficient for this application.

If you want the real RtMidi (multiple backends, full API surface) you can
replace this directory with the official sources from:

- https://github.com/thestk/rtmidi

and update `CMakeLists.txt` accordingly.

## Roadmap / Ideas

- Add an evdev backend for true global key capture under Wayland.
- Configurable key layouts (via config file or CLI).
- Additional controls (sustain pedal, panic/all-notes-off key, etc.).
- Optional GUI for configuration (Qt/SDL) while keeping the CLI mode.
