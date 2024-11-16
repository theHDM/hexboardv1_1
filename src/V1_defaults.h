#pragma once
#include "Arduino.h"

//  This section sets default values for user-editable options

int transposeSteps = 0;
uint8_t scaleLock = 0;
uint8_t perceptual = 1;
uint8_t paletteBeginsAtKeyCenter = 1;
uint8_t animationFPS = 32;             // actually frames per 2^20 microseconds. close enough to 30fps

uint8_t wheelMode = 0;                 // standard vs. fine tune mode
uint8_t modSticky = 0;
uint8_t pbSticky = 0;
uint8_t velSticky = 1;
int modWheelSpeed = 8;
int pbWheelSpeed = 1024;
int velWheelSpeed = 8;

#define SYNTH_OFF 0
#define SYNTH_MONO 1
#define SYNTH_ARPEGGIO 2
#define SYNTH_POLY 3
uint8_t playbackMode = SYNTH_OFF;

#define WAVEFORM_SINE 0
#define WAVEFORM_STRINGS 1
#define WAVEFORM_CLARINET 2
#define WAVEFORM_HYBRID 7
#define WAVEFORM_SQUARE 8
#define WAVEFORM_SAW 9
#define WAVEFORM_TRIANGLE 10 
uint8_t currWave = WAVEFORM_HYBRID;

#define RAINBOW_MODE 0
#define TIERED_COLOR_MODE 1
#define ALTERNATE_COLOR_MODE 2
uint8_t colorMode = RAINBOW_MODE;

#define ANIMATE_NONE 0
#define ANIMATE_STAR 1 
#define ANIMATE_SPLASH 2 
#define ANIMATE_ORBIT 3 
#define ANIMATE_OCTAVE 4 
#define ANIMATE_BY_NOTE 5
uint8_t animationType = ANIMATE_NONE;

#define BRIGHT_MAX 255
#define BRIGHT_HIGH 210
#define BRIGHT_MID 180
#define BRIGHT_LOW 150
#define BRIGHT_DIM 110
#define BRIGHT_DIMMER 70
#define BRIGHT_OFF 0
uint8_t globalBrightness = BRIGHT_MID;

#define AUDIO_NONE 0
#define AUDIO_PIEZO 1
#define AUDIO_AJACK 2
#define AUDIO_BOTH 3
uint8_t audioD = AUDIO_PIEZO;