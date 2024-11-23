#pragma once
#include "utils.h"
#include "config.h"
#include "hwKeys.h" // use the same "linear_index" function as the hardware read, to ensure compatible

// Effective Nov 15, 2024, the portion of the code related to setting the key pins
// is moved to "hwKeys.h". This section now focuses on the grid object in a theoretical sense.

// grandparent structure for a mux/pin input
// a shorted (hardwired) pin is the minimal example
struct switch_t {
  uint hwKey; // column x 2^muxPins + muxValue
  uint type;  // see enum in config.h
  switch_t(uint h, uint t) : hwKey(h), type(t) {}
};

// parent structure for a physical button
// in the rare case a button on a mux/pin grid
// does NOT have an associated LED pixel,
// set pixel negative and the LED routine will
// skip this object. pixel values should still be
// unique among physical button switches.
// there is not a error check for this so be careful
struct button_t : switch_t {
  hex_t coord; // physical location
  uint pixel; // associated pixel
  uint index; // location within its array
  std::map<time_uS, uint> state_history;
  time_uS timePressed;
  int btnState;
  int prevState;
  uint zero = 0;
  pixel_code LEDcodeAnim;     // calculate it once and store value, to make LED playback snappier 
  pixel_code LEDcodePlay;     // calculate it once and store value, to make LED playback snappier
  pixel_code LEDcodeRest;     // calculate it once and store value, to make LED playback snappier
  pixel_code LEDcodeOff;      // calculate it once and store value, to make LED playback snappier
  pixel_code LEDcodeDim;      // calculate it once and store value, to make LED playback snappier
  int      animate; // store value to track animations
  button_t(switch_t sw, hex_t c, uint p) : switch_t(sw.hwKey, sw.type), coord(c), pixel(p) {}
  button_t(uint h, uint t, hex_t c, uint p) : switch_t(h, t), coord(c), pixel(p) {}
};

// child structure for buttons that 
// play a musical note.
struct music_key_t : button_t {
  int      stepsFromC;      // number of steps from C4 (semitones in 12EDO; microtones if >12EDO)
  float    centsFromAnchor;
  
  bool     inScale;         // 0 if it's not in the selected scale; 1 if it is

  uint     note;  // MIDI note or control parameter corresponding to this hex
  int16_t  bend;            // in microtonal mode, the pitch bend for this note needed to be tuned correctly
  float    pitch;

  uint     MIDIch;          // what MIDI channel this note is playing on
  uint     synthCh;         // what synth polyphony ch this is playing on
  float    frequency;     // what frequency to ring on the synther
  music_key_t(button_t btn) : button_t(btn.hwKey, btn.type, btn.coord, btn.pixel) {}
};

// child structure for buttons that
// do any other command.
struct other_cmd_t : button_t {
  uint  cmd;  // control parameter corresponding to this hex
  // possibly more structure, pending
  other_cmd_t(button_t btn) : button_t(btn.hwKey, btn.type, btn.coord, btn.pixel) {}
};

// structure to collect all inputs from 
// the grid, and groups the switches by type.
struct switchboard_t {
  std::vector<music_key_t> keys;
  std::vector<other_cmd_t> commands;
  std::vector<switch_t> hardwired_switches;
//  maps to navigate the list of buttons
  //    index: location in button vector
  //    coord: physical hex location
  //    pixel: corresponding pixel number
  //    hwKey: C...M..., C = column index, M = mux state
  //  these are derived in the grid setup based on config.h constants.
  std::map<hex_t, int>   coord_to_pixel; // e.g. hex(0,-6) -> pixel 5
  std::map<int, bool>    pixel_is_cmd;  // e.g. pixel 5 -> false, pixel 80 -> true
  std::map<int, uint>    pixel_to_index; // e.g. pixel 5 -> 4 (key), pixel 80 -> 4 (cmd)
  music_key_t& key_at_pixel(const int pxl) {
    return keys[pixel_to_index.at(pxl)];
  }
  button_t& button_at_pixel(int pxl) {
    int ind = pixel_to_index.at(pxl);
    if (pixel_is_cmd.at(pxl)) {
      return commands[ind];
    } else {
      return keys[ind];
    }
  }
  button_t& button_at_coord(hex_t coord) {
    return button_at_pixel(coord_to_pixel.at(coord));
  }
  bool in_bounds(hex_t coord) {
    return (coord_to_pixel.find(coord) != coord_to_pixel.end());
  }
};

// THIS IS THE OBJECT WHERE ALL THE MAGIC HAPPENS
switchboard_t hexBoard;
const uint_vec assignCmd = {0,20,40,60,80,100,120};

// when you run the setup routine, you should then have
// arrays of key definitions that are ready to be loaded
// with instructions and states.

void gridSystem_setup() {
  for (auto& eachRow : config_hexboard_layout) {
    switch_t tempSwitch(
      pinGrid.linear_index(
        eachRow.column_pin_index,
        eachRow.multiplexer_value
      ), eachRow.switch_type
    );
    switch (tempSwitch.type) {
      case unused_pin:
        break;
      case hardwired:
        hexBoard.hardwired_switches.emplace_back(tempSwitch);
        break;
      default:
        button_t tempButton(
          tempSwitch,
          {eachRow.hex_coordinate_x,eachRow.hex_coordinate_y},
          eachRow.associated_pixel
        );
        hexBoard.coord_to_pixel[tempButton.coord] = tempButton.pixel; // add to the lookup map
        // in v2 this will be part of the predefined layout definition
        // for now using a very dirty method:
        auto findCmd = std::find(assignCmd.begin(),assignCmd.end(),tempButton.pixel);
        bool isCmd = (findCmd != assignCmd.end());
        hexBoard.pixel_is_cmd[tempButton.pixel] = isCmd; // add to the lookup map
        if (isCmd) {
          other_cmd_t tempCmd(tempButton);
          tempCmd.cmd = findCmd - assignCmd.begin();
          hexBoard.pixel_to_index[tempCmd.pixel] = hexBoard.commands.size();
          hexBoard.commands.emplace_back(tempCmd);
        } else {
          music_key_t tempKey(tempButton);
          hexBoard.pixel_to_index[tempKey.pixel] = hexBoard.keys.size();
          hexBoard.keys.emplace_back(tempKey);
        }
        break;
    }
  }
}
/*

// the command wheel interface won't work in v2
// because members of vector objects are not
// static in memory. the simple answer
// is for the command keys to pass its key state
// when it's pressed or released, and then this
// should otherwise work as normal.

// consider setting this as a setup parameter for
// the wheel constructor. OR.
// the wheel constructor is updated only
// after this many microseconds on a global
// loop (like LEDs).
//  When sending smoothly-varying pitch bend
//  or modulation messages over MIDI, the
//  code uses a cool-down period of about
//  1/30 of a second in between messages, enough
//  for changes to sound continuous without
//  overloading the MIDI message queue.
#define CC_MSG_COOLDOWN_MICROSECONDS 32768
//
//  This class is like a virtual wheel.
//  It takes references / pointers to 
//  the state of three command buttons,
//  translates presses of those buttons
//  into wheel turns, and converts
//  these movements into corresponding
//  values within a range.
//  
//  This lets us generalize the
//  behavior of a virtual pitch bend
//  wheel or mod wheel using the same
//  code, only needing to modify the
// range of output and the connected
// buttons to operate it.

struct wheelDef {
  uint* alternateMode; // two ways to control
  uint* isSticky;      // TRUE if you leave value unchanged when no buttons pressed
  uint* topBtn;        // pointer to the key Status of the button you use as this button
  uint* midBtn;
  uint* botBtn;
  int16_t minValue;
  int16_t maxValue;
  int* stepValue;      // this can be changed via GEM menu
  int16_t defValue;    // snapback value
  int16_t curValue;
  int16_t targetValue;
  time_uS timeLastChanged;
  void setTargetValue() {
    if (*alternateMode) {
      if (*midBtn >> 1) { // middle button toggles target (0) vs. step (1) mode
        int16_t temp = curValue;
            if (*topBtn == 1)     {temp += *stepValue;} // tap button
            if (*botBtn == 1)     {temp -= *stepValue;} // tap button
            if (temp > maxValue)  {temp  = maxValue;} 
        else if (temp <= minValue) {temp  = minValue;}
        targetValue = temp;
      } else {
        switch (((*topBtn >> 1) << 1) + (*botBtn >> 1)) {
          case 0b10:   targetValue = maxValue;     break;
          case 0b11:   targetValue = defValue;     break;
          case 0b01:   targetValue = minValue;     break;
          default:     targetValue = curValue;     break;
        }
      }
    } else {
      switch (((*topBtn >> 1) << 2) + ((*midBtn >> 1) << 1) + (*botBtn >> 1)) {
        case 0b100:  targetValue = maxValue;                         break;
        case 0b110:  targetValue = (3 * maxValue + minValue) / 4;    break;
        case 0b010:
        case 0b111:
        case 0b101:  targetValue = (maxValue + minValue) / 2;        break;
        case 0b011:  targetValue = (maxValue + 3 * minValue) / 4;    break;
        case 0b001:  targetValue = minValue;                         break;
        case 0b000:  targetValue = (*isSticky ? curValue : defValue); break;
        default: break;
      }
    }
  }
  bool updateValue(time_uS givenTime) {
    int16_t temp = targetValue - curValue;
    if (temp != 0) {
      if ((givenTime - timeLastChanged) >= CC_MSG_COOLDOWN_MICROSECONDS ) {
        timeLastChanged = givenTime;
        if (abs(temp) < *stepValue) {
          curValue = targetValue;
        } else {
          curValue = curValue + (*stepValue * (temp / abs(temp)));
        }
        return 1;
      } else {
        return 0;
      }
    } else {
      return 0;
    }
  }   
};
bool toggleWheel = 0; // 0 for mod, 1 for pb

wheelDef modWheel = { &wheelMode, &modSticky,
  &hexBoard.button_at_pixel(assignCmd[4]).zero, 
  &hexBoard.button_at_pixel(assignCmd[5]).zero, 
  &hexBoard.button_at_pixel(assignCmd[6]).zero,
  0, 127, &modWheelSpeed, 0, 0, 0, 0
};
wheelDef pbWheel =  { &wheelMode, &pbSticky, 
  &hexBoard.button_at_pixel(assignCmd[4]).zero, 
  &hexBoard.button_at_pixel(assignCmd[5]).zero, 
  &hexBoard.button_at_pixel(assignCmd[6]).zero,
  -8192, 8191, &pbWheelSpeed, 0, 0, 0, 0
};
wheelDef velWheel = { &wheelMode, &velSticky, 
  &hexBoard.button_at_pixel(assignCmd[0]).zero, 
  &hexBoard.button_at_pixel(assignCmd[1]).zero, 
  &hexBoard.button_at_pixel(assignCmd[2]).zero,
  0, 127, &velWheelSpeed, 96, 96, 96, 0
};
*/