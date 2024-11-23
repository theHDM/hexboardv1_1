#pragma once
#include "gridSystem.h"
#include "presets.h"




























// need to outline dependencies
/*
  This section of the code contains broad
  procedures for assigning musical notes
  and related values to each button
  of the hex grid.

// run this if the layout, key, or transposition changes, but not if color or scale changes
void assignPitches() {     
  sendToLog("assignPitch was called:");
  for (auto& h : hexBoard.keys) {
    // steps is the distance from C (the anchor note)
    // the stepsToMIDI function needs distance from A4
    // it also needs to reflect any transposition, but
    // NOT the key of the scale.

    float N = 60; //stepsToMIDI(current.pitchRelToA4(h.stepsFromC));
    
    if (N < 0 || N >= 128) {
      h.note = 255;
      h.bend = 0;
      h.pitch = 255.0;
      h.frequency = 0.0;
    } else {
      // h.note and h.bend may soon be collapsed into
      // a floating h.note value, with h.bend calculated
      // situationally.
      // also might have to reflect global tuning (i.e.
      // what if the KBM file defines A = 432 or A = 441
      h.pitch = N;
      h.note = ((N >= 127) ? 127 : round(N));
      // skip for now // h.bend = (ldexp(N - h.note, 13) / MPEpitchBendSemis);
      h.frequency = MIDItoFreq(N);
    }
  }
  sendToLog("assignPitches complete.");
}
void applyScale() {
  sendToLog("applyScale was called:");
  for (auto& h : hexBoard.keys) {
    if (current.scale().tuning == ALL_TUNINGS) {
      // i.e. "no scale"
      h.inScale = 1;
    } else {
      uint degree = current.keyDegree(h.stepsFromC); 
      if (degree == 0) {
        h.inScale = 1;    // the root is always in the scale
      } else {
        uint tempSum = 0;
        uint iterator = 0;
        while (degree > tempSum) {
          tempSum += current.scale().pattern[iterator];
          iterator++;
        }  // add the steps in the scale, and you're in scale
        h.inScale = (tempSum == degree);   // if the note lands on one of those sums
      }
    }
  }
  // for now // setLEDcolorCodes();
  sendToLog("applyScale complete.");
}
void applyLayout() {       // call this function when the layout changes
  sendToLog("buildLayout was called:");
  // if the layout is supposed to be developed procedurally,
  // i.e. an isomorphic regular layout,
  // then run the procedure here.



  hex_t middleC = hexBoard.button_at_pixel(current.layout().hexMiddleC).coord;
  // in orthogonal coordinates, a single hex distance = 2 steps, either
  // +/- 2X, or +/- 1X +/- 1Y. keep the scale vector doubled so that
  // integer values are not lost. we might change this when steps are
  // changed to floating cents values.
  hex_t doubleStepVector = { current.layout().acrossSteps,
    (2 * current.layout().dnLeftSteps) + current.layout().acrossSteps};
  for (auto& h : hexBoard.keys) {
    h.stepsFromC = dot_product(doubleStepVector, h.coord - middleC) / 2;
    // h.centsFromAnchor in the future
    // will also require defining template<typename T> hex_t<T> to allow
    // floating values of step-vectors.
  }
  applyScale();        // when layout changes, have to re-apply scale and re-apply LEDs
  assignPitches();     // same with pitches
  sendToLog("buildLayout complete.");
}
*/