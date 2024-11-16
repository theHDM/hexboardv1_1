#pragma once
#include "utils.h"
#include <Arduino.h>
#include <Wire.h>

// appears to work on https://binji.github.io/wasm-clang/
// tested Nov 7 2024
class pinGrid_obj {
  private:
    bool _isAnalog;
    bool _isEnabled;
    bool _cycle_mux_pins_first;
    byte_vec _colPins;
    byte_vec _muxPins;
    uint _colSize;
    uint _muxSize;
    uint _muxMaxValue;
    uint _keyCount;
    int_vec _keyState;
    uint _colCounter;
    uint _muxCounter;
    uint _gridCounter;
    bool _readComplete;
    void init_pin_states() {
      for (auto& m : _muxPins) {
        pinMode(m, OUTPUT); 
      }
      for (auto& c : _colPins) {
        if (_isAnalog) {
          pinMode(c, INPUT_PULLUP);
        } else {
          pinMode(c, INPUT);
        }
      }
    }
    bool advanceCol() {
      pinMode(_colPins[_colCounter], INPUT);
      _colCounter = (++_colCounter) % _colSize;
      pinMode(_colPins[_colCounter], INPUT_PULLUP);
      return (!(_colCounter));
    }
    bool advanceMux() {
      _muxCounter = (++_muxCounter) % _muxMaxValue;
      for (int b = 0; b < _muxSize; b++) {
        digitalWrite(_muxPins[b], (_muxCounter >> b) & 1);
      }
      return (!(_muxCounter));
    }
  public:
    uint linear_index(uint c, uint m) {
      return ((c << _muxSize) | m);
    }
    void resume_background_process() {
      _readComplete = false;
      _gridCounter = 0;
    }
    void setup(byte_vec colPins, bool isAnalog, byte_vec muxPins, bool cycleMuxFirst = true) {
      _isAnalog = isAnalog;
      _colPins = colPins;
      _muxPins = muxPins;
      init_pin_states();

      _colSize = _colPins.size();
      _muxSize = _muxPins.size();
      _muxMaxValue = (1u << _muxSize);
      _keyCount = (_colSize << _muxSize);
      _keyState.resize(_keyCount);

      _colCounter = 0;
      _muxCounter = 0;
      _cycle_mux_pins_first = cycleMuxFirst;
      resume_background_process();
    }
    void poll() {
      if (!(_readComplete)) {
        if (_isAnalog) {
          _keyState[linear_index(_colCounter,_muxCounter)] = analogRead(_colPins[_colCounter]);
        } else {
          _keyState[linear_index(_colCounter,_muxCounter)] = digitalRead(_colPins[_colCounter]);
        }
        ++_gridCounter;
        if (_cycle_mux_pins_first) {
          if (advanceMux()) {
            _readComplete = advanceCol();
          }
        } else {
          if (advanceCol()) {
            _readComplete = advanceMux();
          }
        }
      }
    }
    bool is_background_process_complete() {
      return _readComplete;
    }
    int read_key_state(uint muxCtr, uint colCtr) {
      return _keyState[linear_index(colCtr,muxCtr)];
    }
    int read_key_state(uint keyIndex) {
      return _keyState[keyIndex];
    }
    uint colPinCount() {
      return _colSize;
    }
    uint muxPinMaxValue() {
      return _muxMaxValue;
    }
    uint buttonCount() {
      return _keyCount;
    }
};
pinGrid_obj pinGrid;