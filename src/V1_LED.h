#pragma once
/*
  This section of the code handles sending
  color data to the LED pixels underneath
  the hex buttons.
*/
#include <Adafruit_NeoPixel.h>  // library of code to interact with the LED array
#define LED_PIN 22

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);  
int32_t rainbowDegreeTime = 65'536; // microseconds to go through 1/360 of rainbow
/*
  This is actually a hacked together approximation
  of the color space OKLAB. A true conversion would
  take the hue, saturation, and value bits and
  turn them into linear RGB to feed directly into
  the LED class. This conversion is... not very OK...
  but does the job for now. A proper implementation
  of OKLAB is in the works.
  
  For transforming hues, the okLAB hue degree (0-360) is
  mapped to the RGB hue degree from 0 to 65535, using
  simple linear interpolation I created by hand comparing
  my HexBoard outputs to a Munsell color chip book.
*/
int16_t transformHue(float h) {
  float D = fmod(h,360);
  if (!perceptual) {
    return 65536 * D / 360;
  } else {
    //                red            yellow             green        cyan         blue
    int hueIn[] =  {    0,    9,   18,  102,  117,  135,  142,  155,  203,  240,  252,  261,  306,  333,  360};
    //              #ff0000          #ffff00           #00ff00      #00ffff     #0000ff     #ff00ff
    int hueOut[] = {    0, 3640, 5861,10922,12743,16384,21845,27306,32768,38229,43690,49152,54613,58254,65535};
    byte B = 0;
    while (D - hueIn[B] > 0) {
      B++;
    }
    float T = (D - hueIn[B - 1]) / (float)(hueIn[B] - hueIn[B - 1]);
    return (hueOut[B - 1] * (1 - T)) + (hueOut[B] * T);
  }
}
/*
  Saturation and Brightness are taken as is (already in a 0-255 range).
  The global brightness / 255 attenuates the resulting color for the
  user's brightness selection. Then the resulting RGB (HSV) color is
  "un-gamma'd" to be converted to the LED strip color.
*/
uint32_t getLEDcode(colorDef c) {
  return strip.gamma32(strip.ColorHSV(transformHue(c.hue),c.sat,c.val * globalBrightness / 255));
}
/*
  This function cycles through each button, and based on what color
  palette is active, it calculates the LED color code in the palette, 
  plus its variations for being animated, played, or out-of-scale, and
  stores it for recall during playback and animation. The color
  codes remain in the object until this routine is called again.
*/

colorDef kiteColor(float cents) {
  // This mode assigns each note a color based on the interval it forms with the root note.
  // This is an adaptation of an algorithm developed by Nicholas Fox and Kite Giedraitis.
  //
  //                                            close to       define the closest
  //         range of cents vs. the octave   octave/4th/5th?  perf or neutral interval
                                           bool perf = 0; float center = 0.0;
         if                    (cents <   50)  {perf = 1;    center =    0.0;} // unison
    else if ((cents >=  50) && (cents <  250)) {             center =  147.1;} // 2nd
    else if ((cents >= 250) && (cents <  450)) {             center =  351.0;} // 3rd
    else if ((cents >= 450) && (cents <  600)) {perf = 1;    center =  498.0;} // 4th
    else if ((cents >= 600) && (cents <= 750)) {perf = 1;    center =  702.0;} // 5th
    else if ((cents >  750) && (cents <= 950)) {             center =  849.0;} // 6th
    else if ((cents >  950) && (cents <=1150)) {             center = 1053.0;} // 7th
    else if ((cents > 1150) && (cents < 1250)) {perf = 1;    center = 1200.0;} // octave
    else if ((cents >=1250) && (cents < 1450)) {             center = 1347.1;} // 9th
    else if ((cents >=1450) && (cents < 1650)) {             center = 1551.0;} // 10th
    else if ((cents >=1650) && (cents < 1850)) {perf = 1;    center = 1698.0;} // 11th
    else if ((cents >=1800) && (cents <=1950)) {perf = 1;    center = 1902.0;} // tritave

  // how far away from the perfect interval, if it's a octave / 4th / 5th
  // which side of minor / major, if it's a non-perfect interval
  float offCenter = cents - center;

  // if close to a perfect interval, color white or close to white.
  // if flat of major, or sharp of minor, color yellow/green/cyan
  // if far from perfect interval, or sharp of major, or flat of minor, color purple/red/blue/violet
  int16_t altHue = positiveMod((int)(150 + (perf * ((offCenter > 0) ? -72 : 72)) - round(1.44 * offCenter)), 360);
  float deSaturate = perf * (abs(offCenter) < 20) * (1 - (0.02 * abs(offCenter)));
  return { 
    (float)altHue, 
    (byte)(255 - round(255 * deSaturate)), 
    (byte)(cents ? VALUE_SHADE : VALUE_NORMAL) 
  };
}

void setLEDcolorCodes() {
  for (auto& h : hexBoard.buttons) {
    if (!(h.isCmd)) {
      colorDef setColor;
      byte paletteIndex = positiveMod(h.stepsFromC,current.tuning().cycleLength);
      if (paletteBeginsAtKeyCenter) {
        paletteIndex = current.keyDegree(paletteIndex);
      }
      switch (colorMode) {
        case TIERED_COLOR_MODE: // This mode sets the color based on the palettes defined above.
          setColor = palette[current.tuningIndex].getColor(paletteIndex);
          break;
        case RAINBOW_MODE:      // This mode assigns the root note as red, and the rest as saturated spectrum colors across the rainbow.
          setColor = { 360 * ((float)paletteIndex / (float)current.tuning().cycleLength), SAT_VIVID, VALUE_NORMAL };
          break;
        case ALTERNATE_COLOR_MODE:
          setColor = kiteColor(current.tuning().stepSize * paletteIndex);
          break;
      }
      h.LEDcodeRest = getLEDcode(setColor);
      h.LEDcodePlay = getLEDcode(setColor.tint()); 
      h.LEDcodeDim  = getLEDcode(setColor.shade());  
      setColor = {HUE_NONE,SAT_BW,VALUE_BLACK};
      h.LEDcodeOff  = getLEDcode(setColor);                // turn off entirely
      h.LEDcodeAnim = h.LEDcodePlay;
    }
  }
  sendToLog("LED codes re-calculated.");
}

void resetVelocityLEDs() {
  colorDef tempColor = { 
    (runTime % (rainbowDegreeTime * 360)) / (float)rainbowDegreeTime, 
    SAT_MODERATE, 
    (uint8_t)clip(6 * (velWheel.curValue - 84),0,255)
  };
  strip.setPixelColor(assignCmd[0], getLEDcode(tempColor));

  tempColor.val = clip(6 * (velWheel.curValue-42),0,255);
  strip.setPixelColor(assignCmd[1], getLEDcode(tempColor));
  
  tempColor.val = clip(6 * (velWheel.curValue-0),0,255);
  strip.setPixelColor(assignCmd[2], getLEDcode(tempColor));
}
void resetWheelLEDs() {
  // middle button
  byte tempSat = SAT_BW;
  colorDef tempColor = {HUE_NONE, tempSat, (byte)(toggleWheel ? VALUE_SHADE : VALUE_LOW)};
  strip.setPixelColor(assignCmd[3], getLEDcode(tempColor));
  if (toggleWheel) {
    // pb red / green
    tempSat = SAT_BW + ((uint)((SAT_VIVID - SAT_BW) * std::abs(pbWheel.curValue)) >> 13);
    tempColor = {(float)((pbWheel.curValue > 0) ? HUE_RED : HUE_CYAN), tempSat, VALUE_FULL};
    strip.setPixelColor(assignCmd[5], getLEDcode(tempColor));

    tempColor.val = tempSat * (pbWheel.curValue > 0);
    strip.setPixelColor(assignCmd[4], getLEDcode(tempColor));

    tempColor.val = tempSat * (pbWheel.curValue < 0);
    strip.setPixelColor(assignCmd[6], getLEDcode(tempColor));
  } else {
    // mod blue / yellow
    tempSat = SAT_BW + (((uint)(SAT_VIVID - SAT_BW) * abs(modWheel.curValue - 63)) >> 6);
    tempColor = {
      (float)((modWheel.curValue > 63) ? HUE_YELLOW : HUE_INDIGO), 
      tempSat, 
      (byte)(127 + (tempSat / 2))
    };
    strip.setPixelColor(assignCmd[6], getLEDcode(tempColor));

    if (modWheel.curValue <= 63) {
      tempColor.val = 127 - (tempSat / 2);
    }
    strip.setPixelColor(assignCmd[5], getLEDcode(tempColor));
    
    tempColor.val = tempSat * (modWheel.curValue > 63);
    strip.setPixelColor(assignCmd[4], getLEDcode(tempColor));
  }
}

uint32_t applyNotePixelColor(buttonDef& h) {
       if (h.animate) { return h.LEDcodeAnim;} 
  else if (h.MIDIch)  { return h.LEDcodePlay;} 
  else if (h.inScale) { return h.LEDcodeRest;} 
  else if (scaleLock) { return h.LEDcodeOff; } 
  else                { return h.LEDcodeDim; }
}

void LED_setup() { 
  strip.begin();    // INITIALIZE NeoPixel strip object
  strip.show();     // Turn OFF all pixels ASAP
  sendToLog("LEDs started..."); 
  setLEDcolorCodes();
}

void LED_update_pixels() {   
  for (auto& h : hexBoard.buttons) {
    if (!(h.isCmd)) {
      strip.setPixelColor(h.pixel,applyNotePixelColor(h));
    }
  }
  resetVelocityLEDs();
  resetWheelLEDs();
  strip.show();
}