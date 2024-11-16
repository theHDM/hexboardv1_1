#pragma once
#include <U8g2lib.h>
#include <Wire.h>               // this is necessary to deal with the pins and wires
#define SDAPIN 16
#define SCLPIN 17
#define CONTRAST_AWAKE 63
#define CONTRAST_SCREENSAVER 1
bool screenSaverOn = 0;                         
uint64_t screenTime = 0;                        // GFX timer to count if screensaver should go on
const uint64_t screenSaverTimeout = (1u << 23); // 2^23 microseconds ~ 8 seconds

// Create an instance of the U8g2 graphics library.
U8G2_SH1107_SEEED_128X128_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE);

void OLED_setup() {
  Wire.setSDA(SDAPIN);                // OLED is connected via I2C. set the SDA
  Wire.setSCL(SCLPIN);                // and SCL pins first before starting the object.
  u8g2.begin();                       // Menu and graphics setup
  u8g2.setBusClock(1000000);          // Speed up display
  u8g2.setContrast(CONTRAST_AWAKE);   // Set contrast
  sendToLog("U8G2 graphics initialized.");
}
void OLED_screenSaver() {
  if (screenTime <= screenSaverTimeout) {
    screenTime = screenTime + lapTime;
    if (screenSaverOn) {
      screenSaverOn = 0;
      u8g2.setContrast(CONTRAST_AWAKE);
    }
  } else {
    if (!screenSaverOn) {
      screenSaverOn = 1;
      u8g2.setContrast(CONTRAST_SCREENSAVER);
    }
  }
}