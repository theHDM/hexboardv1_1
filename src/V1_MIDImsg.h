#pragma once
/*
  This section of the code handles all
  things related to MIDI messages.
*/
#include <Adafruit_TinyUSB.h>   // library of code to get the USB port working
#include <MIDI.h>               // library of code to send and receive MIDI messages
#include <queue>                // standard C++ library construction to store open channels in microtonal mode (use "std::queue" to invoke it)
/*
  These values support correct MIDI output.
  Note frequencies are converted to MIDI note
  and pitch bend messages assuming note 69
  equals concert A4, as defined below. 
*/
#define CONCERT_A_HZ 440.0
/*
  Pitch bend messages are calibrated 
  to a pitch bend range where
  -8192 to 8191 = -200 to +200 cents, 
  or two semitones.
*/
#define PITCH_BEND_SEMIS 2
/*
  We use pitch bends to retune notes in MPE mode.
  Some setups can adjust to fit this, but some need us to adjust it.
*/
uint8_t MPEpitchBendSemis = 2;
/*
  Create a new instance of the Arduino MIDI Library,
  and attach usb_midi as the transport.
*/
Adafruit_USBD_MIDI usb_midi;
MIDI_CREATE_INSTANCE(Adafruit_USBD_MIDI, usb_midi, UMIDI);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, SMIDI);
// midiD takes the following bitwise flags
#define MIDID_NONE 0
#define MIDID_USB 1
#define MIDID_SER 2
#define MIDID_BOTH 3
uint8_t midiD = MIDID_USB | MIDID_SER;

// What program change number we last sent (General MIDI/Roland MT-32)
uint8_t programChange = 0;

byte_queue MPEchQueue;
uint8_t MPEpitchBendsNeeded; 

float freqToMIDI(float Hz) {             // formula to convert from Hz to MIDI note
  return 69.0 + 12.0 * log2f(Hz / 440.0);
}
float MIDItoFreq(float midi) {           // formula to convert from MIDI note to Hz
  return 440.0 * exp2((midi - 69.0) / 12.0);
}
float stepsToMIDI(int16_t stepsFromA) {  // return the MIDI pitch associated
  return freqToMIDI(CONCERT_A_HZ) + ((float)stepsFromA * (float)current.tuning().stepSize / 100.0);
}

// wrapper to pass message to one or both MIDI instances
void routeMIDImsg();
// research how to extract the message from the functions
// instead of just sending it.
// also need to take into account MPE, MIDI 2.0, round off, and tuning table

void setPitchBendRange(uint8_t Ch, uint8_t semitones) {
  if (midiD&MIDID_USB) {
      UMIDI.beginRpn(0, Ch);
      UMIDI.sendRpnValue(semitones << 7, Ch);
      UMIDI.endRpn(Ch);
  }
  if (midiD&MIDID_SER) {
      SMIDI.beginRpn(0, Ch);
      SMIDI.sendRpnValue(semitones << 7, Ch);
      SMIDI.endRpn(Ch);
  }
  sendToLog(
    "set pitch bend range on ch " +
    std::to_string(Ch) + " to be " + 
    std::to_string(semitones) + " semitones"
  );
}

void setMPEzone(uint8_t masterCh, uint8_t sizeOfZone) {
  if (midiD&MIDID_USB) {
      UMIDI.beginRpn(6, masterCh);
      UMIDI.sendRpnValue(sizeOfZone << 7, masterCh);
      UMIDI.endRpn(masterCh);
  }
  if (midiD&MIDID_SER) {
      SMIDI.beginRpn(6, masterCh);
      SMIDI.sendRpnValue(sizeOfZone << 7, masterCh);
      SMIDI.endRpn(masterCh);
  }
  sendToLog(
    "tried sending MIDI msg to set MPE zone, master ch " +
    std::to_string(masterCh) + ", zone of this size: " + std::to_string(sizeOfZone)
  );
}

void resetTuningMIDI() {
  /*
    currently the only way that microtonal
    MIDI works is via MPE (MIDI polyphonic expression).
    This assigns re-tuned notes to an independent channel
    so they can be pitched separately.
  
    if operating in a standard 12-EDO tuning, or in a
    tuning with steps that are all exact multiples of
    100 cents, then MPE is not necessary.
  */
  if (current.tuning().stepSize == 100.0) {
    MPEpitchBendsNeeded = 1;
  /*  this was an attempt to allow unlimited polyphony for certain EDOs. doesn't work in Logic Pro.
  } else if (round(current.tuning().cycleLength * current.tuning().stepSize) == 1200) {
    MPEpitchBendsNeeded = current.tuning().cycleLength / std::gcd(12, current.tuning().cycleLength);
  */
  } else {
    MPEpitchBendsNeeded = 255;
  }
  if (MPEpitchBendsNeeded > 15) {
    setMPEzone(1, 15);   // MPE zone 1 = ch 2 thru 16
    while (!MPEchQueue.empty()) {     // empty the channel queue
      MPEchQueue.pop_front();
    }
    for (uint8_t i = 2; i <= 16; i++) {
      MPEchQueue.push_back(i);           // fill the channel queue
    }
  } else {
    setMPEzone(1, 0);
  }
  // force pitch bend back to the expected range of 2 semitones.
  for (uint8_t i = 1; i <= 16; i++) {
    if(midiD&MIDID_USB)UMIDI.sendControlChange(123, 0, i);
    if(midiD&MIDID_SER)SMIDI.sendControlChange(123, 0, i);
    setPitchBendRange(i, MPEpitchBendSemis);   
  }
}

void sendMIDImodulationToCh1() {
  if(midiD&MIDID_USB)UMIDI.sendControlChange(1, modWheel.curValue, 1);
  if(midiD&MIDID_SER)SMIDI.sendControlChange(1, modWheel.curValue, 1);
  sendToLog("sent mod value " + std::to_string(modWheel.curValue) + " to ch 1");
}

void sendMIDIpitchBendToCh1() {
  if(midiD&MIDID_USB)UMIDI.sendPitchBend(pbWheel.curValue, 1);
  if(midiD&MIDID_SER)SMIDI.sendPitchBend(pbWheel.curValue, 1);
  sendToLog("sent pb wheel value " + std::to_string(pbWheel.curValue) + " to ch 1");
}

void tryMIDInoteOn(music_key_t& h) {
  // this gets called on any non-command hex
  // that is not scale-locked.
  if (!(h.MIDIch)) {    
    if (MPEpitchBendsNeeded == 1) {
      h.MIDIch = 1;
    } else if (MPEpitchBendsNeeded <= 15) {
      h.MIDIch = 2 + positiveMod(h.stepsFromC, MPEpitchBendsNeeded);
    } else {
      if (MPEchQueue.empty()) {   // if there aren't any open channels
        sendToLog("MPE queue was empty so did not play a midi note");
      } else {
        h.MIDIch = pop_and_get(MPEchQueue);   // value in MIDI terms (1-16)
      }
    }
    if (h.MIDIch) {
      if(midiD&MIDID_USB)UMIDI.sendNoteOn(h.note, velWheel.curValue, h.MIDIch); // ch 1-16
      if(midiD&MIDID_SER)SMIDI.sendNoteOn(h.note, velWheel.curValue, h.MIDIch); // ch 1-16

      if(midiD&MIDID_USB)UMIDI.sendPitchBend(h.bend, h.MIDIch); // ch 1-16
      if(midiD&MIDID_SER)SMIDI.sendPitchBend(h.bend, h.MIDIch); // ch 1-16
      sendToLog(
        "sent MIDI noteOn: " + std::to_string(h.note) +
        " pb "  + std::to_string(h.bend) +
        " vel " + std::to_string(velWheel.curValue) +
        " ch "  + std::to_string(h.MIDIch)
      );
    } 
  }
} 

void tryMIDInoteOff(music_key_t& h) {
  // this gets called on any non-command hex
  // that is not scale-locked.
  if (h.MIDIch) {    // but just in case, check
    if(midiD&MIDID_USB)UMIDI.sendNoteOff(h.note, velWheel.curValue, h.MIDIch);
    if(midiD&MIDID_SER)SMIDI.sendNoteOff(h.note, velWheel.curValue, h.MIDIch);
    sendToLog(
      "sent MIDI noteOff: " + std::to_string(h.note) +
      " pb " + std::to_string(h.bend) +
      " vel " + std::to_string(velWheel.curValue) +
      " ch " + std::to_string(h.MIDIch)
    );
    if (MPEpitchBendsNeeded > 15) {
      MPEchQueue.push_back(h.MIDIch);
    }
    h.MIDIch = 0;
  }
}

void sendProgramChange() {
  if(midiD&MIDID_USB)UMIDI.sendProgramChange(programChange - 1, 1);
  if(midiD&MIDID_SER)SMIDI.sendProgramChange(programChange - 1, 1);
}

void MIDI_setup() {
  usb_midi.setStringDescriptor("HexBoard MIDI");  // Initialize MIDI, and listen to all MIDI channels
  UMIDI.begin(MIDI_CHANNEL_OMNI);                 // This will also call usb_midi's begin()
  SMIDI.begin(MIDI_CHANNEL_OMNI);
  resetTuningMIDI();
  sendToLog("setupMIDI okay");
}
