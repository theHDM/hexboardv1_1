// Hexboard firmware v1.1
// build date 2024 Nov 15

#include <Arduino.h>
#define HARDWARE_UNKNOWN 0
#define HARDWARE_V1_1 1
#define HARDWARE_V1_2 2 
byte Hardware_Version = HARDWARE_UNKNOWN; // Software-detected hardware revision

#include "src/V1_diagnostics.h"   // optional log messages thru the Serial port
#include "src/utils.h"            // helpful utility functions
#include "src/config.h"           // hardware configuration -- pin locations, mapping, etc.
#include "src/timing.h"           // measuring time and setting software alarms

//  This section does two things:
//  1. sets up the hardware keyboard & rotary knob to run in the background and return
//     details on key presses and timing for use in any arbitary program
//  2. lets you write 8-bit [0..255] audio samples to a buffer and exports audio via PWM to digital pins
#include "src/hardware.h"

#include "src/V1_defaults.h"   // default values for user-editable options
#include "src/V1_microtonal.h" // preloaded tuning systems (equal sized steps only for now)
#include "src/V1_scales.h"     // library of scales (patterns of musical steps)
#include "src/V1_layout.h"     // library of isomorphic keyboard layouts
#include "src/V1_palettes.h"   // library of keyboard colors and patterns
#include "src/V1_presets.h"    // data structure for current user settings
#include "src/V1_1_gridSystem.h" // data structure for keys and buttons
#include "src/V1_LED.h"        // interface to set the LED colors
#include "src/V1_MIDImsg.h"    // convert keyboard actions to MIDI messages
// #include "src/readMIDI.h" in the future to read MIDI settings in/send out
#include "src/V1_1_synth.h"    // converts keyboard actions to synthesized PWM audio
#include "src/V1_1_animate.h"  // reactive key coloring
#include "src/V1_assignment.h" // sets/resets the keyboard notes or colors
#include "src/V1_OLED.h"       // object that drives the B&W OLED screen
#include "src/V1_1_menu.h"     // OLED menu, using the GEM library by Spirik
#include "src/V1_1_interface.h"// translates inputs from hardware.h into actions

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
  //Serial.begin(115200);     //  Set serial to make uploads work without bootsel button
  for (byte i = 0; i < 5 && !TinyUSBDevice.mounted(); i++) {
    delay(1);                //  wait until device mounted, maybe 
  }

  delay(5000);               //  if debugging, give time for the serial monitor to start before sending messages

  OLED_setup();               //  Start the OLED screen, in case you want a splash screen?
  presets_littleFS_setup();   //  Set up the littleFS file system first (to pull stored user settings in v2) 
  hardware_setup();           //  set up the keyboard, rotary, and audio functions based on config constants.
    setup_phase = 1;        //  change the setup flag to let the other core know to start the background processes
  synth_reset();              //  make sure the synth is reset so no notes are running
  MIDI_setup();               //  Set up the USB (Serial, pin 0) and MIDI-out (Serial1, pin 1) as MIDI objects
  gridSystem_setup();         //  Set up the hex grid object, and set the pins that will read the button states
  applyLayout(); // see V1.assignment.h. Based on the default layout, populate grid with notes and colors
  LED_setup();                //  Once the grid is defined, start the LEDs
  menu_setup();               //  Set up the menu last.

}
void loop() { // need to add soft loop timing.
  timing_measure_lap();         //  get time in uS at the start of the loop, measure loop duration
  OLED_screenSaver();           //  every 1 second. reduces wear-and-tear on OLED panel  
  interface_interpret_hexes();  //  every loop. interpret button press actions, play MIDI / synth notes
  interface_update_wheels();    //  v1.0 firmware only. deal with the pitch/mod wheel
  synth_arpeggiate();           //  every X millis based on user input. arpeggiate if synth mode allows it
  animate_calculate_pixels();   //  every 17 or 33 millis, calculate the next frame of responsive animations
  LED_update_pixels();          //  every 17 or 33 millis, send updated pixel data to LEDs
  interface_interpret_rotary(); //  every loop. interpret rotary knob presses, send to menu object, refresh OLED
}

void setup1() {
  while (setup_phase < 1) {}    //  wait until the hardware objects have been set up in the other core
  hardware_start_background_process();   //  begin the background loops on this core
}
void loop1() {
  //  dedicate this core to computing the synth audio and running the background processes.
  if (audioOut.roomToWrite()) {
    audioOut.write(synth.next_sample());   //  write synth samples tnto the audio buffer.
  }
  //  and respond to all hardware task manager interrupts when they are called.
}
