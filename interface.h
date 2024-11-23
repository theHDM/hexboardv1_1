#pragma once

// Effective Nov 15, 2024, the portion of the code related to setting the key and rotary pins
// is moved to "hwRotary.h" and "hwKeys.h". This section is solely for interpreting
// input presses.

/*
void setupHardware() {
  if (Hardware_Version == HARDWARE_V1_2) {
      midiD = MIDID_USB | MIDID_SER;
      audioD = AUDIO_PIEZO | AUDIO_AJACK;
      menuPageSynth.addMenuItem(menuItemAudioD, 2);
      globalBrightness = BRIGHT_DIM;
      setLEDcolorCodes();
      rotary.invertDirection(true);
  }
}
*/

void interface_interpret_hexes() {
  // only do this if the pingrid object is free
  if (!(pinGrid.is_background_process_complete())) {
    return;
  }
  // read in the new pin state completely first
  for (auto& h : hexBoard.keys) {
    h.prevState = h.btnState;
    int r = pinGrid.read_key_state(h.hwKey);
    h.btnState = r;
  }
  for (auto& h : hexBoard.commands) {
    h.prevState = h.btnState;
    uint r = pinGrid.read_key_state(h.hwKey);
    h.btnState = r;
  }
  // release pingrid object
  pinGrid.resume_background_process();
  // then cycle each key and perform actions  
  for (auto& h : hexBoard.keys) {
    sendToLog(
      " pxl " + std::to_string(h.pixel)
      + " hw " + std::to_string(h.hwKey)
      + " hex " + std::to_string(h.coord.x) + "," + std::to_string(h.coord.y)
      + " note " + std::to_string(h.note)
      + " scal " + std::to_string(h.inScale)
      + " btn " + std::to_string((h.btnState << 1) | h.prevState)
    );
    //if (h.inScale || (!scaleLock)) {
      switch ((h.btnState << 1) | h.prevState) {
        case 2: // just pressed
          //tryMIDInoteOn(h);
          //trySynthNoteOn(h);
          break;
        case 1: // just released    
          //tryMIDInoteOff(h);
          //trySynthNoteOff(h);
          break; 
      }
    //}
  }
  for (auto& h : hexBoard.commands) {
    sendToLog(
      " pxl " + std::to_string(h.pixel)
      + " hw " + std::to_string(h.hwKey)
      + " hex " + std::to_string(h.coord.x) + "," + std::to_string(h.coord.y)
      + " cmd " + std::to_string(h.cmd)
      + " btn " + std::to_string((h.btnState << 1) | h.prevState)
    );
    switch ((h.btnState << 1) | h.prevState) {
      case 2: // just pressed
        switch (h.cmd) {
          //case CMDB + 3:
            //toggleWheel = !toggleWheel;
          //  break;
          default:
            // the rest should all be taken care of within the wheelDef structure
            break;
        }
      default: // inactive
        break;
    }
  }
}
/*
void interface_update_wheels() {  
  velWheel.setTargetValue();
  bool upd = velWheel.updateValue(runTime);
  if (upd) {
    sendToLog("vel became " + std::to_string(velWheel.curValue));
  }
  if (toggleWheel) {
    pbWheel.setTargetValue();
    upd = pbWheel.updateValue(runTime);
    if (upd) {
      sendMIDIpitchBendToCh1();
      updateSynthWithNewFreqs();
    }
  } else {
    modWheel.setTargetValue();
    upd = modWheel.updateValue(runTime);
    if (upd) {
      sendMIDImodulationToCh1();
    }
  }
}

void interface_interpret_rotary() {
  if (menu.readyForKey()) {
    if (rotary.getClickFromBuffer()) {
      menu.registerKeyPress(GEM_KEY_OK);
      screenTime = 0;
    }
    int getTurn = rotary.getTurnFromBuffer();
    if (getTurn != 0) {
      menu.registerKeyPress((getTurn > 0) ? GEM_KEY_UP : GEM_KEY_DOWN);
      screenTime = 0;
    }
  }
}
*/