#pragma once
/*
  This section of the code handles
  optional sending of log messages
  to the Serial port
*/
#include <Adafruit_TinyUSB.h>   // library of code to get the USB port working
#include <string>               // standard C++ library string classes (use "std::string" to invoke it); these do not cause the memory corruption that Arduino::String does.
#define DIAGNOSTICS_ON true 
void sendToLog(std::string msg) {
  if (DIAGNOSTICS_ON) {
    Serial.println(msg.c_str());
  }
}