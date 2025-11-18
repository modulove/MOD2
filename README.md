# MOD2

Welcome to the MOD2 Workshop repository – a collection of Firmwares for Hagiwo's MOD2 hardware module, alternative frontpanel designs and new different firmware versions contributed by fellow enthusiasts. These were made for a Workshop series by modulove in Hamburg in 2025.

## Available Firmware

This repository includes 7 different firmware options for the MOD2 hardware:

1. **[BRAIDS](Firmware/braids/README.md)** - Macro oscillator with 47 synthesis engines (Mutable Instruments port)
2. **[CLAP](Firmware/clap/README.md)** - TR-808 style hand clap synthesizer
3. **[CLAVES](Firmware/claves/README.md)** - Simple percussion voice with morphable waveform
4. **[FM_DRUM](Firmware/fm_drum/README.md)** - Two-operator FM percussion with dual mode
5. **[HIHAT](Firmware/hihat/README.md)** - Blue/white noise hi-hat synthesizer
6. **[KICK](Firmware/kick/README.md)** - Six-parameter kick drum synthesizer
7. **[VCO](Firmware/vco/README.md)** - Six-waveform voltage controlled oscillator

### Sample Player Firmware

The **[SAMPLE](Firmware/sample/README.md)** player firmware requires a `sample.h` file containing audio samples. This file is not included in the repository but is available from Hagiwo's Patreon:

**[Download Sample Firmware from Hagiwo's Patreon](https://www.patreon.com/posts/code-for-mod2-131363551)**

Please support the original creator!

---

Each firmware includes detailed documentation with control mappings, features, and patch recommendations.

## How to Flash Firmware

### Method 1: Download Pre-Built Firmware (Easiest)

**Option A: Direct Downloads (Recommended)**
- Download pre-built firmware:
  - [MOD2_braids.uf2](https://modulove.github.io/releases/MOD2_braids.uf2)
  - [MOD2_clap.uf2](https://modulove.github.io/releases/MOD2_clap.uf2)
  - [MOD2_claves.uf2](https://modulove.github.io/releases/MOD2_claves.uf2)
  - [MOD2_fm_drum.uf2](https://modulove.github.io/releases/MOD2_fm_drum.uf2)
  - [MOD2_hihat.uf2](https://modulove.github.io/releases/MOD2_hihat.uf2)
  - [MOD2_kick.uf2](https://modulove.github.io/releases/MOD2_kick.uf2)
  - [MOD2_vco.uf2](https://modulove.github.io/releases/MOD2_vco.uf2)

**Option B: GitHub Artifacts**
1. Go to the [Actions](https://github.com/modulove/MOD2/actions) tab
2. Click on the latest successful workflow run
3. Scroll down to "Artifacts"
4. Download individual firmware (e.g., `braids-firmware`, `kick-firmware`, etc.)
5. Extract the .uf2 file

### Method 2: Download from Releases

1. Go to [Releases](https://github.com/modulove/MOD2/releases)
2. Download the desired .uf2 file from the latest release

### Flashing to MOD2 Hardware

1. Hold down the **BOOTSEL** button on your Raspberry Pi Pico/Pico 2
2. While holding BOOTSEL, connect the USB cable to your computer
3. Release BOOTSEL - the Pico will appear as a USB drive
4. Drag and drop the .uf2 file onto the USB drive
5. The Pico will automatically reboot with the new firmware

## Building from Source

If you want to compile the firmware yourself:

### Prerequisites

- Arduino IDE or arduino-cli
- RP2040/RP2350 board support (Earle Philhower's core)
- Required libraries (see each firmware's README)

### For BRAIDS firmware specifically:

```bash
arduino-cli lib install Bounce2
arduino-cli lib install "RPI_PICO_TimerInterrupt"

# Clone BRAIDS and STMLIB libraries
git clone https://github.com/poetaster/arduinoMI.git
# Copy to Arduino libraries folder
```

### Compile and Upload

```bash
arduino-cli compile -b rp2040:rp2040:rpipico2 Firmware/<firmware-name>/<firmware-name>.ino
arduino-cli upload -b rp2040:rp2040:rpipico2 -p <port> Firmware/<firmware-name>/<firmware-name>.ino
```

## Hardware

This firmware is designed for Hagiwo's MOD2 hardware module:
- Based on Raspberry Pi RP2040 or RP2350 (Pico 2)
- Three potentiometers (CV inputs)
- Audio output via PWM
- Trigger/gate inputs
- Button input

For hardware details and schematics, visit [Hagiwo's page](https://note.com/solder_state/n/n3e5db2fbaafc).

## Credits

- Original MOD2 hardware design by [Hagiwo](https://note.com/solder_state)
- BRAIDS firmware port from [Mutable Instruments](https://mutable-instruments.net/) by [poetaster](https://github.com/poetaster/arduinoMI)
- Workshop series organized by [modulove Hamburg](https://modulove.io)
- Firmware collection and documentation by the MOD2 community

## License

Each firmware may have its own license. Please check individual firmware directories for details.

- BRAIDS: MIT License (Mutable Instruments)
- Other firmware: Check source files for license information
