#pragma once
#include <Wire.h>
#include "utils.h"
#include "config.h"
void sendToLog(str msg) {
  if (diagnostics_on) {
    Serial.println(msg.c_str());
  }
}