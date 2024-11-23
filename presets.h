#pragma once
#include "utils.h"
#include "microtonal.h"
#include "layout.h"
#include "scales.h"
/*
  This section of the code defines
  a "preset" as a collection of
  parameters that control how the
  hexboard is operating and playing.

  In the long run this will serve as
  a foundation for saving and loading
  preferences / settings through the
  file system.
*/
struct presetDef { 
  str presetName; 
  uint tuningIndex;     // instead of using pointers, i chose to store index value of each option, to be saved to a .pref or .ini or something
  uint layoutIndex;
  uint scaleIndex;
  int keyStepsFromA; // what key the scale is in, where zero equals A.
  int transpose;
  // define simple recall functions
  tuningDef tuning() {
    return tuningOptions[tuningIndex];
  }
  layoutDef layout() {
    return layoutOptions[layoutIndex];
  }
  scaleDef scale() {
    return scaleOptions[scaleIndex];
  }
  uint layoutsBegin() {
    if (tuningIndex == TUNING_12EDO) {
      return 0;
    } else {
      uint temp = 0;
      while (layoutOptions[temp].tuning < tuningIndex) {
        temp++;
      }
      return temp;
    }
  }
  int keyStepsFromC() {
    return tuning().spanCtoA() - keyStepsFromA;
  }
  int pitchRelToA4(int givenStepsFromC) {
    return givenStepsFromC + tuning().spanCtoA() + transpose;
  }
  uint keyDegree(int givenStepsFromC) {
    return positiveMod(givenStepsFromC + keyStepsFromC(), tuning().cycleLength);
  }
};

presetDef current = {
  "Default",      // name
  TUNING_12EDO,   // tuning
  0,              // default to the first layout, wicki hayden
  0,              // default to using no scale (chromatic)
  -9,             // default to the key of C, which in 12EDO is -9 steps from A.
  0               // default to no transposition
};
/*
  This section of the code handles the
  file system. There isn't much being
  done with it yet, per se.
  If so, this section might be relocated
*/
#include "LittleFS.h"       // code to use flash drive space as a file system -- not implemented yet, as of May 2024
void presets_littleFS_setup() {
  LittleFSConfig cfg;       // Configure file system defaults
  cfg.setAutoFormat(true);  // Formats file system if it cannot be mounted.
  LittleFS.setConfig(cfg);
  LittleFS.begin();         // Mounts file system.
  if (!LittleFS.begin()) {
    sendToLog("An Error has occurred while mounting LittleFS");
  } else {
    sendToLog("LittleFS mounted OK");
  }
}