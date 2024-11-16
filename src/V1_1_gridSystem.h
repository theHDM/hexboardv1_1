#pragma once
#include "Arduino.h"
#include "Wire.h"
#include <algorithm>

// Effective Nov 15, 2024, the portion of the code related to setting the key pins
// is moved to "hwKeys.h". This section now focuses on the grid object in a theoretical sense.

/*
  This section of the code handles the hex grid
      Hexagonal coordinates
        https://www.redblobgames.com/grids/hexagons/
        http://ondras.github.io/rot.js/manual/#hex/indexing

  There are 140 LED pixels on the Hexboard.
  LED instructions all go through the LED_PIN.
  It so happens that each LED pixel corresponds
  to one and only one hex button, so both a LED
  and its button can have the same index from 0-139.
  Since these parameters are pre-defined by the
  hardware build, the dimensions of the grid
  are therefore constants.
*/
#define LED_COUNT 140
#define COLCOUNT 10
#define ROWCOUNT 16
#define BTN_COUNT COLCOUNT*ROWCOUNT
/*
  Of the 140 buttons, 7 are offset to the bottom left
  quadrant of the Hexboard and are reserved as command
  buttons. Their LED reference is pre-defined here.
  If you want those seven buttons remapped to play
  notes, you may wish to change or remove these
  variables and alter the value of CMDCOUNT to agree
  with how many buttons you reserve for non-note use.
*/
#define CMDBTN_0 0
#define CMDBTN_1 20
#define CMDBTN_2 40
#define CMDBTN_3 60
#define CMDBTN_4 80
#define CMDBTN_5 100
#define CMDBTN_6 120
#define CMDCOUNT 7
/*
  This class defines the hexagon button
  as an object. It stores all real-time
  properties of the button -- its coordinates,
  its current pressed state, the color
  codes to display based on what action is
  taken, what note and frequency is assigned,
  whether the button is a command or not,
  whether the note is in the selected scale,
  whether the button is flagged to be animated,
  and whether the note is currently 
  sounding on MIDI / the synth.
  
  Needless to say, this is an important class.
*/
struct buttonDef {
  uint     index;              // i can't bother to look up how to self-reference the index in the array lol
  hex_t    coord;    // invalid
  int      pixel = -1;           // what pixel does this hex light up (or -1 if n/a)
  uint8_t  hwKey;            // where in the hardware pingrid the state is foud

  uint64_t timePressed = 0;     // timecode of last press
  uint32_t LEDcodeAnim = 0;     // calculate it once and store value, to make LED playback snappier 
  uint32_t LEDcodePlay = 0;     // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeRest = 0;     // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeOff = 0;      // calculate it once and store value, to make LED playback snappier
  uint32_t LEDcodeDim = 0;      // calculate it once and store value, to make LED playback snappier
  bool     animate = 0;         // hex is flagged as part of the animation in this frame, helps make animations smoother
  int16_t  stepsFromC = 0;      // number of steps from C4 (semitones in 12EDO; microtones if >12EDO)
  bool     isCmd = 0;           // 0 if it's a MIDI note; 1 if it's a MIDI control cmd
  bool     inScale = 0;         // 0 if it's not in the selected scale; 1 if it is
  uint8_t  note = UNUSED_NOTE;  // MIDI note or control parameter corresponding to this hex
  int16_t  bend = 0;            // in microtonal mode, the pitch bend for this note needed to be tuned correctly
  uint8_t  MIDIch = 0;          // what MIDI channel this note is playing on
  uint8_t  synthCh = 0;         // what synth polyphony ch this is playing on
  float    frequency = 0.0;     // what frequency to ring on the synther
  #define BTN_STATE_OFF 0
  #define BTN_STATE_NEWPRESS 1
  #define BTN_STATE_RELEASED 2
  #define BTN_STATE_HELD 3
  uint8_t  btnState = 0;       
  uint8_t  prevBtnState = 0;
  buttonDef(uint n) {
    index = n;
  }
};

struct button_collection {
  std::vector<buttonDef> buttons;
  //  maps to navigate the list of buttons
  //    index: location in button vector
  //    coord: physical hex location
  //    pixel: corresponding pixel number
  //    hwKey: C...M..., C = column index, M = mux state
  //  these are derived in the grid setup based on config.h constants.
  std::map<hex_t, int>   coord_to_index;
  std::map<int,   int>   pixel_to_index;
  std::map<int,   int>   hwKey_to_index;
  buttonDef& button_at_coord(const hex_t coord) {
    return buttons[coord_to_index[coord]];
  }
  buttonDef& button_on_pixel(const int pixel) {
    return buttons[pixel_to_index[pixel]];
  }
  buttonDef& button_at_hwKey(const int hwKey) {
    return buttons[hwKey_to_index[hwKey]];
  }
  bool coord_in_bounds(const hex_t coord) {
    return (coord_to_index.find(coord) != coord_to_index.end());
  }
};

/*
  When sending smoothly-varying pitch bend
  or modulation messages over MIDI, the
  code uses a cool-down period of about
  1/30 of a second in between messages, enough
  for changes to sound continuous without
  overloading the MIDI message queue.
*/
#define CC_MSG_COOLDOWN_MICROSECONDS 32768
/*
  This class is like a virtual wheel.
  It takes references / pointers to 
  the state of three command buttons,
  translates presses of those buttons
  into wheel turns, and converts
  these movements into corresponding
  values within a range.
  
  This lets us generalize the
  behavior of a virtual pitch bend
  wheel or mod wheel using the same
  code, only needing to modify the
  range of output and the connected
  buttons to operate it.
*/
struct wheelDef {
  byte* alternateMode; // two ways to control
  byte* isSticky;      // TRUE if you leave value unchanged when no buttons pressed
  byte* topBtn;        // pointer to the key Status of the button you use as this button
  byte* midBtn;
  byte* botBtn;
  int16_t minValue;
  int16_t maxValue;
  int* stepValue;      // this can be changed via GEM menu
  int16_t defValue;    // snapback value
  int16_t curValue;
  int16_t targetValue;
  uint64_t timeLastChanged;
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
  bool updateValue(uint64_t givenTime) {
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
const byte assignCmd[] = { 
  CMDBTN_0, CMDBTN_1, CMDBTN_2, CMDBTN_3, 
  CMDBTN_4, CMDBTN_5, CMDBTN_6
};

button_collection hexBoard;

void gridSystem_setup() {
  int asgn;
  int pxl;
  int ky;
  // index = order within vector
  for (asgn = 0; asgn < pinGrid.buttonCount(); ++asgn) {
    hexBoard.buttons.emplace_back(buttonDef(asgn));
  }
  // sort the button locations physically from left to right, top to bottom
  hex_t sorted_coord[pixelCount];
  std::copy(std::begin(ortho_coord_of_pixel),std::end(ortho_coord_of_pixel),std::begin(sorted_coord));
  std::sort(std::begin(sorted_coord), std::end(sorted_coord));
  
  // assign buttons in order of coordinate location
  for (asgn = 0; asgn < pixelCount; ++asgn) {
    hexBoard.buttons[asgn].coord = sorted_coord[asgn];
    hexBoard.coord_to_index[sorted_coord[asgn]] = asgn;
  }
  // load pixel / coordinate lookup map
  for (pxl = 0; pxl < pixelCount; ++pxl) {
    asgn = hexBoard.coord_to_index[ortho_coord_of_pixel[pxl]];
    hexBoard.pixel_to_index[pxl] = asgn;
    hexBoard.buttons[asgn].pixel = pxl;
  }
  // load hardware key # / pixel lookup map
  int nextNonPixelIndex = pixelCount;
  for (ky = 0; ky < pinGrid.buttonCount(); ++ky) {
    pxl = hwKey_to_pixel[ky];
    if (pxl >= 0) {
      asgn = hexBoard.pixel_to_index[pxl];
    } else {
      asgn = nextNonPixelIndex;
      ++nextNonPixelIndex;
    }
    hexBoard.hwKey_to_index[ky] = asgn;
    hexBoard.buttons[asgn].hwKey = ky;
  }
    
  for (auto& h : hexBoard.buttons) {
    h.btnState = 0;
    h.isCmd = (h.pixel == -1);
  }
  for (uint8_t c = 0; c < CMDCOUNT; c++) {
    hexBoard.button_on_pixel(assignCmd[c]).isCmd = 1;
    hexBoard.button_on_pixel(assignCmd[c]).note = CMDB + c;
  }
  // On version 1.2, "button" 140 is shorted (always connected)
  hexBoard.button_at_hwKey(140).note = HARDWARE_V1_2;

}

bool toggleWheel = 0; // 0 for mod, 1 for pb

wheelDef modWheel = { &wheelMode, &modSticky,
  &hexBoard.button_on_pixel(assignCmd[4]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[5]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[6]).btnState,
  0, 127, &modWheelSpeed, 0, 0, 0, 0
};
wheelDef pbWheel =  { &wheelMode, &pbSticky, 
  &hexBoard.button_on_pixel(assignCmd[4]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[5]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[6]).btnState,
  -8192, 8191, &pbWheelSpeed, 0, 0, 0, 0
};
wheelDef velWheel = { &wheelMode, &velSticky, 
  &hexBoard.button_on_pixel(assignCmd[0]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[1]).btnState, 
  &hexBoard.button_on_pixel(assignCmd[2]).btnState,
  0, 127, &velWheelSpeed, 96, 96, 96, 0
};