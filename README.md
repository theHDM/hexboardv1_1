```
    HexBoard
    Copyright 2022-2023 Jared DeCook and Zach DeCook
    with help from Nicholas Fox
    Licensed under the GNU GPL Version 3.

    Hardware information:
      Generic RP2040 running at 133MHz with 16MB of flash
        https://github.com/earlephilhower/arduino-pico
      Additional board manager URL:
        https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
      Tools > USB Stack > (Adafruit TinyUSB)
      Sketch > Export Compiled Binary

    Compilation instructions:
      Using arduino-cli...
        # Download the board index
        arduino-cli --additional-urls=https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json core update-index
        # Install the core for rp2040
        arduino-cli --additional-urls=https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json core download rp2040:rp2040
        arduino-cli --additional-urls=https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json core install rp2040:rp2040
        # Install libraries
        arduino-cli lib install "MIDI library"
        arduino-cli lib install "Adafruit NeoPixel"
        arduino-cli lib install "U8g2" # dependency for GEM
        arduino-cli lib install "Adafruit GFX Library" # dependency for GEM
        arduino-cli lib install "GEM"
        sed -i 's@#include "config/enable-glcd.h"@//\0@g' ~/Arduino/libraries/GEM/src/config.h # remove dependency from GEM
        # Run Make to build the firmware
        make
    ---------------------------
    New to programming Arduino?
    ---------------------------
    Coding the Hexboard is, basically, done in C++.
    
    When the HexBoard is plugged in, it runs
    void setup() and void setup1(), then
    runs void loop() and void loop1() on an
    infinite loop until the HexBoard powers down.
    There are two cores running independently.
    You can pretend that the compiler tosses
    these two routines inside an int main() for
    each processor.
  
    To #include libraries, the Arduino
    compiler expects them to be installed from
    a centralized repository. You can also bring
    your own .h / .cpp code but it must be saved
    in "/src/____/___.h" to be valid.

    We found this really annoying so to the
    extent possible we have consolidated
    this code into one single .ino sketch file.
    However, the code is sectioned into something
    like a library format for each feature
    of the HexBoard, so that if the code becomes
    too long to manage in a single file in the
    future, it is easier to air-lift parts of
    the code into a library at that point.
```