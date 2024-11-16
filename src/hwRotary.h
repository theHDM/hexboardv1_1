#pragma once
#include "utils.h"
#include <Arduino.h>
#include <Wire.h>
/*
  Documentation:
    Rotary knob code derived from:
      https://github.com/buxtronix/arduino/tree/master/libraries/Rotary
  Copyright 2011 Ben Buxton. Licenced under the GNU GPL Version 3.
  Contact: bb@cactii.net

  when the mechanical rotary knob is turned,
  the two pins go through a set sequence of
  states during one physical "click", as follows:
    Direction          Binary state of pin A\B
    Counterclockwise = 1\1, 0\1, 0\0, 1\0, 1\1
    Clockwise        = 1\1, 1\0, 0\0, 0\1, 1\1

  The neutral state of the knob is 1\1; a turn
  is complete when 1\1 is reached again after
  passing through all the valid states above,
  at which point action should be taken depending
  on the direction of the turn.
  
  The variable "state" captures all this as follows
    Value    Meaning
    0        Knob is in neutral state
    1, 2, 3  CCW turn state 1, 2, 3
    4, 5, 6   CW turn state 1, 2, 3
    8, 16    Completed turn CCW, CW
*/

class rotary_obj {
private:
  uint8_t _Apin;
  uint8_t _Bpin;
  uint8_t _Cpin;
  bool _invert;
  uint8_t _turnState;
  uint8_t _clickState;
  int _turnBuffer;
  time_uS _lastClickTime;
  time_queue _clickQueue;
  const uint8_t stateMatrix[7][4] = {
    {0,4,1,0},
    {2,0,1,0},{2,3,1,0},{2,3,0,8},
    {5,4,0,0},{5,4,6,0},{5,0,6,16}
  };
public:
  void setup(uint8_t Apin, uint8_t Bpin, uint8_t Cpin) {
    _Apin = Apin;
    _Bpin = Bpin;
    _Cpin = Cpin;
    pinMode(_Apin, INPUT_PULLUP);
    pinMode(_Bpin, INPUT_PULLUP);
    pinMode(_Cpin, INPUT_PULLUP);
    _invert = false;
    _turnState = 0;
    _clickState = 0;
    _turnBuffer = 0;
    _lastClickTime = 0;
    _clickQueue.clear();
  }
  void invertDirection(bool invert) {
    _invert = invert;
  }
  void poll() {
    uint8_t A = digitalRead(_Apin);
    uint8_t B = digitalRead(_Bpin);
    uint8_t getRotation = (_invert ? ((A << 1) | B) : ((B << 1) | A));
    _turnState = stateMatrix[_turnState & 0b00111][getRotation];
    _turnBuffer += (_turnState & 0b01000) >> 3;
    _turnBuffer -= (_turnState & 0b10000) >> 4;
    _clickState = (0b00011 & ((_clickState << 1) + (digitalRead(_Cpin) == LOW)));
    switch (_clickState) { 
      case 0b11:
        _lastClickTime = getTheCurrentTime();
        break;
      case 0b10:
        _clickQueue.push_back(getTheCurrentTime() - _lastClickTime);
        break;
      default:
        break;
    }
  }
  int getTurnFromBuffer() {
    int x = ((_turnBuffer > 0) ? 1 : -1) * (_turnBuffer != 0);
    _turnBuffer -= x;
    return x;
  }
  time_uS getClickFromBuffer() {
    time_uS x = 0;
    if (!(_clickQueue.empty())) {
      x = pop_and_get(_clickQueue);
    }
    return x;
  }
};
rotary_obj rotary;