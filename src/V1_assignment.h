#pragma once
/*
  This section of the code contains broad
  procedures for assigning musical notes
  and related values to each button
  of the hex grid.
*/
// run this if the layout, key, or transposition changes, but not if color or scale changes
void assignPitches() {     
  sendToLog("assignPitch was called:");
  for (auto& h : hexBoard.buttons) {
    if (!(h.isCmd) && (h.pixel >= 0)) {
      // steps is the distance from C
      // the stepsToMIDI function needs distance from A4
      // it also needs to reflect any transposition, but
      // NOT the key of the scale.
      float N = stepsToMIDI(current.pitchRelToA4(h.stepsFromC));
      if (N < 0 || N >= 128) {
        h.note = UNUSED_NOTE;
        h.bend = 0;
        h.frequency = 0.0;
      } else {
        h.note = ((N >= 127) ? 127 : round(N));
        h.bend = (ldexp(N - h.note, 13) / MPEpitchBendSemis);
        h.frequency = MIDItoFreq(N);
      }
    }
  }
  sendToLog("assignPitches complete.");
}
void applyScale() {
  sendToLog("applyScale was called:");
  for (auto& h : hexBoard.buttons) {
    if (!(h.isCmd) && (h.pixel >= 0)) {
      if (current.scale().tuning == ALL_TUNINGS) {
        h.inScale = 1;
      } else {
        uint8_t degree = current.keyDegree(h.stepsFromC); 
        if (degree == 0) {
          h.inScale = 1;    // the root is always in the scale
        } else {
          uint8_t tempSum = 0;
          uint8_t iterator = 0;
          while (degree > tempSum) {
            tempSum += current.scale().pattern[iterator];
            iterator++;
          }  // add the steps in the scale, and you're in scale
          h.inScale = (tempSum == degree);   // if the note lands on one of those sums
        }
      }
    }
  }
  setLEDcolorCodes();
  sendToLog("applyScale complete.");
}
void applyLayout() {       // call this function when the layout changes
  sendToLog("buildLayout was called:");
  for (auto& h : hexBoard.buttons) {
    if (!(h.isCmd) && (h.pixel >= 0)) {
      int8_t distCol = h.coord.q - hexBoard.button_on_pixel(current.layout().hexMiddleC).coord.q;
      int8_t distRow = h.coord.r - hexBoard.button_on_pixel(current.layout().hexMiddleC).coord.r;
      h.stepsFromC = ((distCol * current.layout().acrossSteps) 
        + (distRow * (current.layout().acrossSteps + (2 * current.layout().dnLeftSteps)))) / 2;  
    }
  }
  applyScale();        // when layout changes, have to re-apply scale and re-apply LEDs
  assignPitches();     // same with pitches
  sendToLog("buildLayout complete.");
}