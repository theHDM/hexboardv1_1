#pragma once
/*
  This section of the code handles
  LED animation responsive to key
  presses
*/

// Effective Nov 15, 2024, the structure is cleaned up
// to lean on the hex_t definition in utils.h
// and the lookup of buttons at each hex
//
// this library works, so far, on ortho coordinates

uint64_t animFrame(buttonDef& h) {     
  if (h.timePressed) {          // 2^20 microseconds is close enough to 1 second
    return 1 + (((runTime - h.timePressed) * animationFPS) >> 20);
  } else {
    return 0;
  }
}
void flagToAnimate(hex_t x) {
  if (hexBoard.coord_in_bounds(x)) {
    hexBoard.button_at_coord(x).animate = 1;
  }
}
void animateMirror(buttonDef& h) {
  if ((!(h.isCmd)) && (h.MIDIch)) {                   // that is a held note     
    for (auto j : hexBoard.buttons) {
      if ((j.pixel >= 0) && (!(j.isCmd)) && (!(j.MIDIch))) {
        int16_t delta = h.stepsFromC - j.stepsFromC;   // look at difference between notes
        if (animationType == ANIMATE_OCTAVE) {              // set octave diff to zero if need be
          delta = positiveMod(delta, current.tuning().cycleLength);
        }
        j.animate = (!(delta));   // highlight if diff is zero
      }
    }
  }
}

void animateOrbit(buttonDef& h) {
  if ((!(h.isCmd)) && (h.MIDIch) && ((h.inScale) || (!scaleLock))) {    // that is a held note
    uint8_t dir = (animFrame(h) % 6);
    flagToAnimate(h.coord + orthoUnit[dir]);       // different neighbor each frame
  }
}

void animateRadial(buttonDef& h) {
  if (!(h.isCmd) && (h.inScale || !scaleLock)) {                       // that is a note
    uint64_t radius = animFrame(h);
    if ((radius > 0) && (radius < 16)) {                              // played in the last 16 frames
      uint8_t steps = ((animationType == ANIMATE_SPLASH) ? radius : 1);  // star = 1 step to next corner; ring = 1 step per hex
      hex_t turtle = h.coord + (orthoUnit[dir_sw] * radius);
      for (uint8_t dir = dir_e; dir < 6; dir++) {           // walk along the ring in each of the 6 hex directions
        for (uint8_t i = 0; i < steps; i++) {                            // # of steps to the next corner 
          flagToAnimate(turtle);                         // flag for animation
          turtle = turtle + (orthoUnit[dir] * (radius / steps));
        }
      }
    }
  }          
}
void animate_calculate_pixels() {
  if (animationType) {
    for (auto h : hexBoard.buttons) {
      // clear animation flags
      h.animate = 0;
      if (h.pixel >= 0) {
        // recalculate for every hex
        switch (animationType) { 
          case ANIMATE_STAR: case ANIMATE_SPLASH:
            animateRadial(h);
            break;
          case ANIMATE_ORBIT:
            animateOrbit(h);
            break;
          case ANIMATE_OCTAVE: case ANIMATE_BY_NOTE:
            animateMirror(h);
            break;  
          default:
            break;
        }
      }
    }
  }
}