#pragma once
#include "Arduino.h"
#include <string>
#include "V1_microtonal.h"
/*
  This section defines the different
  preset note layout options.
*/  
/*
  This class provides the seed values
  needed to implement a given isomorphic
  note layout. From it, the map of buttons
  to note frequencies can be calculated.
  
  A layout is tied to a specific tuning.
*/
class layoutDef {
public:
  std::string name;    // limit is 17 characters for GEM menu
  bool isPortrait;     // affects orientation of the GEM menu only.
  byte hexMiddleC;     // instead of "what note is button 1", "what button is the middle"
  int8_t acrossSteps;  // defined this way to be compatible with original v1.1 firmare
  int8_t dnLeftSteps;  // defined this way to be compatible with original v1.1 firmare
  byte tuning;         // index of the tuning that this layout is designed for
};
/*
  Isomorphic layouts are defined by
  establishing where the center of the
  layout is, and then the number of tuning
  steps to go up or down for the hex button
  across or down diagonally.
*/
layoutDef layoutOptions[] = {
  { "Wicki-Hayden",      1, 64,   2,  -7, TUNING_12EDO },
  { "Harmonic Table",    0, 75,  -7,   3, TUNING_12EDO },
  { "Janko",             0, 65,  -1,  -1, TUNING_12EDO },
  { "Gerhard",           0, 65,  -1,  -3, TUNING_12EDO },
  { "Accordion C-sys.",  1, 75,   2,  -3, TUNING_12EDO },
  { "Accordion B-sys.",  1, 64,   1,  -3, TUNING_12EDO },

  { "Full Gamut",        1, 65,   1,  -9, TUNING_17EDO },
  { "Bosanquet-Wilson",  0, 65,  -2,  -1, TUNING_17EDO },
  { "Neutral Thirds A",  0, 65,  -1,  -2, TUNING_17EDO },
  { "Neutral Thirds B",  0, 65,   1,  -3, TUNING_17EDO },

  { "Full Gamut",        1, 65,   1,  -9, TUNING_19EDO },
  { "Bosanquet-Wilson",  0, 65,  -1,  -2, TUNING_19EDO },
  { "Kleismic",          0, 65,  -1,  -4, TUNING_19EDO },
  
  { "Full Gamut",        1, 65,   1,  -8, TUNING_22EDO },
  { "Bosanquet-Wilson",  0, 65,  -3,  -1, TUNING_22EDO },
  { "Porcupine",         0, 65,   1,  -4, TUNING_22EDO },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_24EDO },
  { "Bosanquet-Wilson",  0, 65,  -1,  -3, TUNING_24EDO },
  { "Inverted",          0, 65,   1,  -4, TUNING_24EDO },
  
  { "Full Gamut",        1, 65,   1,  -7, TUNING_31EDO },
  { "Bosanquet-Wilson",  0, 65,  -2,  -3, TUNING_31EDO },
  { "Double Bosanquet",  0, 65,  -1,  -4, TUNING_31EDO },
  { "Anti-Double Bos.",  0, 65,   1,  -5, TUNING_31EDO },
  
  { "Full Gamut",        0, 65,   1,  -8, TUNING_41EDO },  // forty-one #3
  { "Bosanquet-Wilson",  0, 65,  -4,  -3, TUNING_41EDO },  // forty-one #1
  { "Gerhard",           0, 65,   3, -10, TUNING_41EDO },  // forty-one #2
  { "Baldy",             0, 65,  -1,  -6, TUNING_41EDO },  
  { "Rodan",             1, 65,  -1,  -7, TUNING_41EDO },  
  
  { "Wicki-Hayden",      1, 64,   9, -31, TUNING_53EDO },
  { "Bosanquet-Wilson",  0, 65,  -5,  -4, TUNING_53EDO },
  { "Kleismic A",        0, 65,  -8,  -3, TUNING_53EDO },
  { "Kleismic B",        0, 65,  -5,  -3, TUNING_53EDO },
  { "Harmonic Table",    0, 75, -31,  14, TUNING_53EDO },
  { "Buzzard",           0, 65,  -9,  -1, TUNING_53EDO },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_72EDO },
  { "Expanded Janko",    0, 65,  -1,  -6, TUNING_72EDO },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_BP },
  { "Standard",          0, 65,  -2,  -1, TUNING_BP },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_ALPHA },
  { "Compressed",        0, 65,  -2,  -1, TUNING_ALPHA },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_BETA },
  { "Compressed",        0, 65,  -2,  -1, TUNING_BETA },
  
  { "Full Gamut",        1, 65,   1,  -9, TUNING_GAMMA },
  { "Compressed",        0, 65,  -2,  -1, TUNING_GAMMA }    
};
const byte layoutCount = sizeof(layoutOptions) / sizeof(layoutDef);