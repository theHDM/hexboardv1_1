#pragma once

// Effective Nov 15, 2024, the portion of the code related to setting the PWM pins
// is moved to "hwAudio.h". This section is solely a synth generator, and the resulting samples
// are written to an audio buffer so long as there is free space.
// EQ routines have also been moved because different EQ should be applied to different outputs,
// and those formulas only need a history of samples and are independent of the synth or source.


/*
  These definitions provide 8-bit samples to emulate.
  You can add your own as desired; it must
  be an array of 256 values, each from 0 to 255.
  Ideally the waveform is normalized so that the
  peaks are at 0 to 255, with 127 representing
  no wave movement.
*/
const uint8_t sine[] = {
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   2,   3,   3, 
    4,   5,   6,   7,   8,   9,  10,  12,  13,  15,  16,  18,  19,  21,  23,  25, 
    27,  29,  31,  33,  35,  37,  39,  42,  44,  46,  49,  51,  54,  56,  59,  62, 
    64,  67,  70,  73,  76,  79,  81,  84,  87,  90,  93,  96,  99, 103, 106, 109, 
  112, 115, 118, 121, 124, 127, 131, 134, 137, 140, 143, 146, 149, 152, 156, 159, 
  162, 165, 168, 171, 174, 176, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 
  206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 236, 237, 
  239, 240, 242, 243, 245, 246, 247, 248, 249, 250, 251, 252, 252, 253, 254, 254, 
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 254, 254, 253, 252, 252, 
  251, 250, 249, 248, 247, 246, 245, 243, 242, 240, 239, 237, 236, 234, 232, 230, 
  228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 
  191, 188, 185, 182, 179, 176, 174, 171, 168, 165, 162, 159, 156, 152, 149, 146, 
  143, 140, 137, 134, 131, 127, 124, 121, 118, 115, 112, 109, 106, 103,  99,  96, 
    93,  90,  87,  84,  81,  79,  76,  73,  70,  67,  64,  62,  59,  56,  54,  51, 
    49,  46,  44,  42,  39,  37,  35,  33,  31,  29,  27,  25,  23,  21,  19,  18, 
    16,  15,  13,  12,  10,   9,   8,   7,   6,   5,   4,   3,   3,   2,   1,   1
};
const uint8_t strings[] = {
    0,   0,   0,   1,   3,   6,  10,  14,  20,  26,  33,  41,  50,  59,  68,  77, 
    87,  97, 106, 115, 124, 132, 140, 146, 152, 157, 161, 164, 166, 167, 167, 167, 
  165, 163, 160, 157, 153, 149, 144, 140, 135, 130, 126, 122, 118, 114, 111, 109, 
  106, 104, 103, 101, 101, 100, 100, 100, 100, 101, 101, 102, 103, 103, 104, 105, 
  106, 107, 108, 109, 110, 111, 113, 114, 115, 116, 117, 119, 120, 121, 123, 124, 
  126, 127, 129, 131, 132, 134, 135, 136, 138, 139, 140, 141, 142, 144, 145, 146, 
  147, 148, 149, 150, 151, 152, 152, 153, 154, 154, 155, 155, 155, 155, 154, 154, 
  152, 151, 149, 146, 144, 140, 137, 133, 129, 125, 120, 115, 111, 106, 102,  98, 
    95,  92,  90,  88,  88,  88,  89,  91,  94,  98, 103, 109, 115, 123, 131, 140, 
  149, 158, 168, 178, 187, 196, 205, 214, 222, 229, 235, 241, 245, 249, 252, 254, 
  255, 255, 255, 254, 253, 250, 248, 245, 242, 239, 236, 233, 230, 227, 224, 222, 
  220, 218, 216, 215, 214, 213, 212, 211, 210, 210, 209, 208, 207, 206, 205, 203, 
  201, 199, 197, 194, 191, 188, 184, 180, 175, 171, 166, 161, 156, 150, 145, 139, 
  133, 127, 122, 116, 110, 105,  99,  94,  89,  84,  80,  75,  71,  67,  64,  61, 
    58,  56,  54,  52,  50,  49,  48,  47,  46,  45,  45,  44,  43,  42,  41,  40, 
    39,  37,  35,  33,  31,  28,  25,  22,  19,  16,  13,  10,   7,   5,   2,   1
};
const uint8_t clarinet[] = {
    0,   0,   2,   7,  14,  21,  30,  38,  47,  54,  61,  66,  70,  72,  73,  74, 
    73,  73,  72,  71,  70,  71,  72,  74,  76,  80,  84,  88,  93,  97, 101, 105, 
  109, 111, 113, 114, 114, 114, 113, 112, 111, 110, 109, 109, 109, 110, 112, 114, 
  116, 118, 121, 123, 126, 127, 128, 129, 128, 127, 126, 123, 121, 118, 116, 114, 
  112, 110, 109, 109, 109, 110, 111, 112, 113, 114, 114, 114, 113, 111, 109, 105, 
  101,  97,  93,  88,  84,  80,  76,  74,  72,  71,  70,  71,  72,  73,  73,  74, 
    73,  72,  70,  66,  61,  54,  47,  38,  30,  21,  14,   7,   2,   0,   0,   2, 
    9,  18,  31,  46,  64,  84, 105, 127, 150, 171, 191, 209, 224, 237, 246, 252, 
  255, 255, 253, 248, 241, 234, 225, 217, 208, 201, 194, 189, 185, 183, 182, 181, 
  182, 182, 183, 184, 185, 184, 183, 181, 179, 175, 171, 167, 162, 158, 154, 150, 
  146, 144, 142, 141, 141, 141, 142, 143, 144, 145, 146, 146, 146, 145, 143, 141, 
  139, 136, 134, 132, 129, 128, 127, 126, 127, 128, 129, 132, 134, 136, 139, 141, 
  143, 145, 146, 146, 146, 145, 144, 143, 142, 141, 141, 141, 142, 144, 146, 150, 
  154, 158, 162, 167, 171, 175, 179, 181, 183, 184, 185, 184, 183, 182, 182, 181, 
  182, 183, 185, 189, 194, 201, 208, 217, 225, 234, 241, 248, 253, 255, 255, 252, 
  246, 237, 224, 209, 191, 171, 150, 127, 105,  84,  64,  46,  31,  18,   9,   2, 
};
/*
  The hybrid synth sound blends between
  square, saw, and triangle waveforms
  at different frequencies. Said frequencies
  are controlled via constants here.
*/
  #define TRANSITION_SQUARE    220.0
  #define TRANSITION_SAW_LOW   440.0
  #define TRANSITION_SAW_HIGH  880.0
  #define TRANSITION_TRIANGLE 1760.0
/*
  The poll interval represents how often a
  new sample value is emulated on the PWM
  hardware. It is the inverse of the digital
  audio sample rate. 24 microseconds has been
  determined to be the sweet spot, and corresponds
  to approximately 41 kHz, which is close to
  CD-quality (44.1 kHz). A shorter poll interval
  may produce more pleasant tones, but if the
  poll is too short then the code will not have
  enough time to calculate the new sample and
  the resulting audio becomes unstable and
  inaccurate. 
*/
#define POLL_INTERVAL_IN_MICROSECONDS 24
/*
  Eight voice polyphony can be simulated. 
  Any more voices and the
  resolution is too low to distinguish;
  also, the code becomes too slow to keep
  up with the poll interval. This value
  can be safely reduced below eight if
  there are issues.
  
  Note this is NOT the same as the MIDI
  polyphony limit, which is 15 (based
  on using channel 2 through 16 for
  polyphonic expression mode).
*/
#define POLYPHONY_LIMIT 8
/*
  This class defines a virtual oscillator.
  It stores an oscillation frequency in
  the form of an increment value, which is
  how much a counter would have to be increased
  every time the poll() interval is reached, 
  such that a counter overflows from 0 to 65,535
  back to zero at some frequency per second.
  
  The value of the counter is useful for reading
  a waveform sample, so that an analog signal
  can be emulated by reading the sample at each
  poll() based on how far the counter has moved
  towards 65,536.
*/
struct oscillator {
  uint16_t increment = 0;
  uint16_t counter = 0;
  uint8_t a = 127;
  uint8_t b = 128;
  uint8_t c = 255;
  uint16_t ab = 0;
  uint16_t cd = 0;
  void set_frequency(float f) { 
    counter = 0;
    increment = round(f * POLL_INTERVAL_IN_MICROSECONDS * 0.065536);   // cycle 0-65535 at resultant frequency
    // synth[c].eq = isoTwoTwentySix(f);
    if (currWave == WAVEFORM_HYBRID) {
      if (f < TRANSITION_SQUARE) {
        b = 128;
      } else if (f < TRANSITION_SAW_LOW) {
        b = (uint8_t)(128 + 127 * (f - TRANSITION_SQUARE) / (TRANSITION_SAW_LOW - TRANSITION_SQUARE));
      } else if (f < TRANSITION_SAW_HIGH) {
        b = 255;
      } else if (f < TRANSITION_TRIANGLE) {
        b = (uint8_t)(127 + 128 * (TRANSITION_TRIANGLE - f) / (TRANSITION_TRIANGLE - TRANSITION_SAW_HIGH));
      } else {
        b = 127;
      }
      if (f < TRANSITION_SAW_LOW) {
        a = 255 - b;
        c = 255;
      } else {
        a = 0;
        c = b;
      }
      if (a > 126) {
        ab = 65535;
      } else {
        ab = 65535 / (b - a - 1);
      }
      cd = 65535 / (256 - c);
    }
  }
};



int arpeggiatingNow = UNUSED_NOTE;         // if this is 255, set to off (0% duty cycle)
uint64_t arpeggiateTime = 0;                // Used to keep track of when this note started playing in ARPEG mode
uint64_t arpeggiateLength = 65536;         // in microseconds. approx a 1/32 note at 114 BPM

struct synth_obj {
  std::vector<oscillator> channel;
  byte_queue open_queue;
  const uint8_t attenuation[9] = {64,24,17,14,12,11,10,9,8}; // full volume in mono mode; equalized volume in poly.

  void init() {
    channel.resize(POLYPHONY_LIMIT);
  }
  void setFreq(float frequency, uint8_t ch) {
    float f = frequency * exp2(pbWheel.curValue * PITCH_BEND_SEMIS / 98304.0);
    sendToLog("set synth ch " + std::to_string(ch) + " to " + std::to_string(f));
    channel[ch - 1].set_frequency(f);
  }
  
  uint8_t next_sample() {
    uint32_t mix = 0;
    uint8_t voices = 0;
    for (auto& i : channel) {
      if (i.increment) {
        i.counter += i.increment; // should overflow from 65536 -> 0        
        uint8_t t = i.counter >> 8;       // 0 .. 255
        switch (currWave) {
          case WAVEFORM_SAW:
            mix += i.counter;
            break;
          case WAVEFORM_TRIANGLE:
            mix += 2 * ((t >> 7) ? t : (256 - t));
            break;
          case WAVEFORM_SQUARE:
            // duty cycle = 50% when mod = min; 6.25% when mod = max
            mix += ((t > (128 - (modWheel.curValue >> 3) * 7)) ? 255 : 0);   break;
          case WAVEFORM_HYBRID:
            if (t > i.a) {
              uint16_t p = 65535; 
              if (t < i.b) {
                p = ((t - i.a) * i.ab) >> 8;
              } else if (t <= i.c) {
                mix += 255;
              } else {
                p = ((256 - t) * i.cd) >> 8;
              }
            }
            break;
          case WAVEFORM_SINE:     
            mix += sine[t];                                   
            break;
          case WAVEFORM_STRINGS:  
            mix += strings[t];                                
            break;
          case WAVEFORM_CLARINET: 
            mix += clarinet[t];                               
            break;
          default:
            break;
        }
        ++voices;
      }
    }
    mix *= attenuation[(playbackMode == SYNTH_POLY) * voices]; // [8bit]*atten[6bit] = [14bit]
    mix *= velWheel.curValue; // [14bit]*vel[7bit]=[21bit] 
    return (mix >> 13);  // [21bit] - [8bit] = [13bit]
  }
};

synth_obj synth;

// USE THIS IN MONO OR ARPEG MODE ONLY
// return the hex index of the next held note.
// kind of messy but works.
uint8_t findNextHeldNote() {
  // arpeggiatingNow refers to a pixel number currently.
  // so first find the note that matches it.
  // then iterate through all the notes until next note is found,
  // and return its pixel.
/*
  auto findArpeggiatingNow = &hexBoard.key_at_pixel(arpeggiatingNow);
  
  if (findArpeggiatingNow != hexBoard.keys.end() - 1) {
    for (auto i = findArpeggiatingNow + 1; i != hexBoard.keys.end(); ++i) {
      if (i->MIDIch) {
        return i->pixel;
      }
    }
  }
  if (findArpeggiatingNow > hexBoard.keys.begin()) {
    for (auto i = hexBoard.keys.begin(); i != findArpeggiatingNow; ++i) {
      if (i->MIDIch) {
        return i->pixel;
      }
    }
  }
*/
  return UNUSED_NOTE;
}

void replaceMonoSynthWith(int x) {
  if (arpeggiatingNow == x) return;
  hexBoard.key_at_pixel(arpeggiatingNow).synthCh = 0;
  arpeggiatingNow = x;
  if (arpeggiatingNow != UNUSED_NOTE) {
    hexBoard.key_at_pixel(arpeggiatingNow).synthCh = 1;
    synth.setFreq(hexBoard.key_at_pixel(arpeggiatingNow).frequency, 1);
  } else {
    synth.setFreq(0, 1);
  }
}

// pass all notes thru synth again if the pitch bend changes
void updateSynthWithNewFreqs() {
  for (auto& h : hexBoard.keys) {
    if (h.synthCh) {
      synth.setFreq(h.frequency, h.synthCh);
    }
  }
}

void trySynthNoteOn(music_key_t& h) {
  if (playbackMode != SYNTH_OFF) {
    if (playbackMode == SYNTH_POLY) {
      // operate independently of MIDI
      if (synth.open_queue.empty()) {
        sendToLog("synth channels all firing, so did not add one");
      } else {
        h.synthCh = pop_and_get(synth.open_queue);
        synth.setFreq(h.frequency, h.synthCh);
      }
    } else {    
      // operate in lockstep with MIDI
      if (h.MIDIch) {
        replaceMonoSynthWith(h.index);
      }
    }
  }
}
void trySynthNoteOff(music_key_t& h) {
  if (playbackMode && (playbackMode != SYNTH_POLY)) {
    if (arpeggiatingNow == h.pixel) {
      replaceMonoSynthWith(findNextHeldNote());
    }
  }
  if (playbackMode == SYNTH_POLY) {
    if (h.synthCh) {
      synth.setFreq(0, h.synthCh);
      synth.open_queue.push_back(h.synthCh);
      h.synthCh = 0;
    }
  }
}

void synth_reset() {
  // i don't think there's anything else to do to initialize the synth in this version
  while (!synth.open_queue.empty()) {
    synth.open_queue.pop_front();
  }
  for (auto& i : synth.channel) {
    i.increment = 0;
    i.counter = 0;
  }
  for (auto& h : hexBoard.keys) {
    h.synthCh = 0;
  }
  if (playbackMode == SYNTH_POLY) {
    for (uint8_t i = 0; i < POLYPHONY_LIMIT; i++) {
      synth.open_queue.push_back(i + 1);
    }
  }
  audioOut.set_pin(piezoPin,     audioD & AUDIO_PIEZO);
  audioOut.set_pin(audioJackPin, audioD & AUDIO_AJACK);
  sendToLog("synth is reset.");
}

void synth_setup() {
  synth.init();
  synth_reset();
}

void synth_arpeggiate() {
  if (playbackMode == SYNTH_ARPEGGIO) {
    if (runTime - arpeggiateTime > arpeggiateLength) {
      arpeggiateTime = runTime;
      replaceMonoSynthWith(findNextHeldNote());
    }
  }
}