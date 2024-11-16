// Hexboard firmware v1.1
// build date 2024 Nov 15

#include <Arduino.h>
#define HARDWARE_UNKNOWN 0
#define HARDWARE_V1_1 1
#define HARDWARE_V1_2 2 
byte Hardware_Version = HARDWARE_UNKNOWN; // Software-detected hardware revision

//  This section of the code handles optional log messages thru the Serial port
#include "src/V1_diagnostics.h"

//  This section of the code contains helpful utility functions
#include "src/utils.h"

//  This section of the code contains hardware configuration -- pin locations, mapping, etc.
#include "src/config.h"

//  This section defines functions for measuring time and setting software alarms
#include "src/timing.h"

//  This section does two things:
//  1. sets up the hardware keyboard & rotary knob to run in the background and return
//     details on key presses and timing for use in any arbitary program
//  2. lets you write 8-bit [0..255] audio samples to a buffer and exports audio via PWM to digital pins
#include "src/hardware.h"

//  This section sets default values for user-editable options
#include "src/V1_defaults.h"

//  This section sets preloaded tuning systems (equal sized steps only for now)
#include "src/V1_microtonal.h"

//  This section sets a library of scales (patterns of musical steps)
#include "src/V1_scales.h"

//  This section sets a library of isomorphic keyboard layouts
#include "src/V1_layout.h"

//  This section sets a library of keyboard colors and patterns
#include "src/V1_palettes.h"

//  This section makes a data structure for current user settings
#include "src/V1_presets.h"



//  This section creates the main data object, a single keyboard button
#include "src/V1_1_gridSystem.h"

//  This section creates the interface to set the LED colors
#include "src/V1_LED.h"

//  This section has functions to convert keyboard actions to MIDI messages
#include "src/V1_MIDImsg.h"

//  This section converts keyboard actions to synthesized PWM audio signals
#include "src/V1_1_synth.h"

//  This section modifies key colors in response to animation routines
#include "src/V1_1_animate.h"

//  This section sets or resets the keyboard notes or colors
#include "src/V1_assignment.h"

//  This section creates the object that drives the B&W OLED screen
#include "src/V1_OLED.h"

//  This section creates the OLED menu, using the GEM library by Spirik
#include "src/V1_1_menu.h"

//  This section translates inputs from the keyboard or knob into actions
#include "src/V1_1_interface.h"

//  An Arduino program runs the setup() function once, then runs the loop() 
//  function on repeat until the machine is powered off. In C++ terms:
//    int main() {setup(); while (1) {loop();} return 0;}

//  The RP2040 has two identical cores. 
//  Anything called from setup()  and loop()  runs on the first core.
//  Anything called from setup1() and loop1() runs on the second core.

//  On the HexBoard, the second core is dedicated to two timing-critical tasks:
//  running the synth emulator, and tracking the rotary knob inputs.
//  Everything else runs on the first core.

//  a global variable used to control the timing of setup functions between cores
int setup_phase = 0;


void setup() {
  #if (defined(ARDUINO_ARCH_MBED) && defined(ARDUINO_ARCH_RP2040))
  //  Manual begin() is required on core without built-in support for TinyUSB 
  //  such as mbed rp2040
  TinyUSB_Device_Init(0);  
  #endif
  //Serial.begin(115200);     // Set serial to make uploads work without bootsel button
  //  Start the OLED screen, in case you want a splash screen?
  OLED_setup();
  //  Set up the littleFS file system first (to pull stored user settings in v2)
  presets_littleFS_setup(); 
  //  set up the keyboard, rotary, and audio functions based on config constants.
  hardware_setup();
  //  change the setup flag to let the other core know to start the background processes
  setup_phase = 1;
  //  make sure the synth is reset so no notes are running
  synth_reset();
  //  Set up the USB (Serial, pin 0) and MIDI-out (Serial1, pin 1) as MIDI objects
  MIDI_setup();
  //  Set up the hex grid object, and set the pins that will read the button states
  gridSystem_setup();
  //  Based on the default layout, populate grid with notes and colors
  applyLayout(); // see V1.assignment.h
  //  Once the grid is defined, start the LEDs
  LED_setup();
  //  Set up the menu last.
  menu_setup();
  //  wait until device mounted, maybe
  for (byte i = 0; i < 5 && !TinyUSBDevice.mounted(); i++) {
    delay(1);  
  }
}
void loop() {
  //  get time in uS at the start of the loop, measure loop duration
  timing_measure_lap();
  //  reduce wear-and-tear on OLED panel
  OLED_screenSaver();
  //  interpret button press actions, play MIDI / synth notes
  interface_interpret_hexes(); 
  //  deal with the pitch/mod wheel
  interface_update_wheels();
  //  arpeggiate if synth mode allows it
  synth_arpeggiate();
  //  calculate the next frame of responsive animations
  animate_calculate_pixels();
  //  send updated pixel data to LEDs
  LED_update_pixels();
  //  interpret rotary knob presses, send to menu object, refresh OLED
  interface_interpret_rotary();
}

void setup1() {
  //  wait until the hardware objects have been set up in the other core
  while (setup_phase < 1) {}
  //  begin the background loops on this core
  hardware_start_background_process();
}
void loop1() {
  //  dedicate this core to computing the synth audio and running the background processes.
  if (audioOut.roomToWrite()) {
    //  write as many synth samples as are available into the audio buffer,
    audioOut.write(synth.next_sample());
  }
  //  and respond to all hardware task manager interrupts when they are called.
}
