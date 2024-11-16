#pragma once
#include "utils.h"

// Hardware pin constants
// If you rewire the HexBoard then
// change these pin values as needed

const byte_vec muxPins = {4,5,2,3};
const byte_vec colPins = {6,7,8,9,10,11,12,13,14,15};
const uint8_t OLED_sdaPin = 16;
const uint8_t OLED_sclPin = 17;
const uint8_t rotaryPinA = 20;
const uint8_t rotaryPinB = 21;
const uint8_t ledPin = 22;
const uint8_t rotaryPinC = 24;
const uint8_t piezoPin = 23;
const uint8_t audioJackPin = 25;

// The particular neoPixel LEDs you use are
// probably going to be the same length of 140.
// But if that ever changes, you can put that here.
// Note that number of buttons might not equal
// the number of pixels, so it is not set to be
// the product of rows and columns. I mean, you never know.

const uint pixelCount = 140;

// These are timing constants that are hardware dependent.
// If you use different components you may need to change
// these timings if there is unexpected behavior.

const uint rotary_pin_fire_period_in_uS    =    768;
const uint keyboard_pin_reset_period_in_uS =     16;
const uint target_audio_sample_rate_in_Hz  = 31'250;
const uint target_LED_frame_rate_in_Hz     =     30;

// The hexboard is wired to read the state of a grid of buttons
// using a set number of column pins (i.e. 10) and a multiplexer
// with a set number of pins (i.e. 4) that can switch 2^P states.
// The hardware routine exports the read voltage of each column
// pin at each multiplex state into a 2d matrix. The mux state
// is the 1st dimension, the column pin is the 2nd dimension.

// This constant is a lookup table to match the export to the associated
// pixel number on the LED strip that is under that associated button.
// If there are pin combinations that are not physical buttons with LEDs,
// they can be marked as a negative integer to be skipped.
//
// If the wiring of the keyboard or the direction of the LED strips change
// then you'll want to update this table.
//
// hwKey = column# * (2 ^ muxPins) + muxState#
// e.g. 4 mux pins, column #3 of 10 [2, from 0..9], muxState 5 [binary 0101]
// hwKey = 2 * 16 + 5 = #37
//
const int_vec hwKey_to_pixel = {
	//  MUX:
  //  0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F     COL: 
      0,  10, 20, 30, 40, 50, 60, 70, 80, 90, 100,110,120,130,-1, -1,  // 0
      1,  11, 21, 31, 41, 51, 61, 71, 81, 91, 101,111,121,131,-1, -1,  // 1
      2,  12, 22, 32, 42, 52, 62, 72, 82, 92, 102,112,122,132,-1, -1,  // 2
      3,  13, 23, 33, 43, 53, 63, 73, 83, 93, 103,113,123,133,-1, -1,  // 3
      4,  14, 24, 34, 44, 54, 64, 74, 84, 94, 104,114,124,134,-1, -1,  // 4
      5,  15, 25, 35, 45, 55, 65, 75, 85, 95, 105,115,125,135,-1, -1,  // 5
      6,  16, 26, 36, 46, 56, 66, 76, 86, 96, 106,116,126,136,-1, -1,  // 6
      7,  17, 27, 37, 47, 57, 67, 77, 87, 97, 107,117,127,137,-1, -1,  // 7
      8,  18, 28, 38, 48, 58, 68, 78, 88, 98, 108,118,128,138,-1, -1,  // 8
      9,  19, 29, 39, 49, 59, 69, 79, 89, 99, 109,119,129,139,-1, -1   // 9
};

//
// This is the physical coordinate location of each pixel.
// It matters for note layout and animation purposes.
// Depending on if you're using axial or orthogonal coordinates
// you can choose which constant to include.
/*
const std::vector<hex_t> axial_coord_of_pixel = {
  //   0       1       2       3       4       5       6       7       8       9 
  {-5, 0},{-1,-6},{ 0,-6},{ 1,-6},{ 2,-6},{ 3,-6},{ 4,-6},{ 5,-6},{ 6,-6},{ 7,-6},     // +  0
      {-2,-5},{-1,-5},{ 0,-5},{ 1,-5},{ 2,-5},{ 3,-5},{ 4,-5},{ 5,-5},{ 6,-5},{ 7,-5}, // + 10
  {-6, 1},{-2,-4},{-1,-4},{ 0,-4},{ 1,-4},{ 2,-4},{ 3,-4},{ 4,-4},{ 5,-4},{ 6,-4},     // + 20
      {-3,-3},{-2,-3},{-1,-3},{ 0,-3},{ 1,-3},{ 2,-3},{ 3,-3},{ 4,-3},{ 5,-3},{ 6,-3}, // + 30
  {-6, 2},{-3,-2},{-2,-2},{-1,-2},{ 0,-2},{ 1,-2},{ 2,-2},{ 3,-2},{ 4,-2},{ 5,-2},     // + 40
      {-4,-1},{-3,-1},{-2,-1},{-1,-1},{ 0,-1},{ 1,-1},{ 2,-1},{ 3,-1},{ 4,-1},{ 5,-1}, // + 50
  {-7, 3},{-4, 0},{-3, 0},{-2, 0},{-1, 0},{ 0, 0},{ 1, 0},{ 2, 0},{ 3, 0},{ 4, 0},     // + 60
      {-5, 1},{-4, 1},{-3, 1},{-2, 1},{-1, 1},{ 0, 1},{ 1, 1},{ 2, 1},{ 3, 1},{ 4, 1}, // + 70
  {-7, 4},{-5, 2},{-4, 2},{-3, 2},{-2, 2},{-1, 2},{ 0, 2},{ 1, 2},{ 2, 2},{ 3, 2},     // + 80
      {-6, 3},{-5, 3},{-4, 3},{-3, 3},{-2, 3},{-1, 3},{ 0, 3},{ 1, 3},{ 2, 3},{ 3, 3}, // + 90
  {-8, 5},{-6, 4},{-5, 4},{-4, 4},{-3, 4},{-2, 4},{-1, 4},{ 0, 4},{ 1, 4},{ 2, 4},     // +100
      {-7, 5},{-6, 5},{-5, 5},{-4, 5},{-3, 5},{-2, 5},{-1, 5},{ 0, 5},{ 1, 5},{ 2, 5}, // +110
  {-8, 6},{-7, 6},{-6, 6},{-5, 6},{-4, 6},{-3, 6},{-2, 6},{-1, 6},{ 0, 6},{ 1, 6},     // +120
      {-8, 7},{-7, 7},{-6, 7},{-5, 7},{-4, 7},{-3, 7},{-2, 7},{-1, 7},{ 0, 7},{ 1, 7}  // +130
};
*/
const hex_t ortho_coord_of_pixel[pixelCount] = {
  //   0       1       2       3       4       5       6       7       8       9 
  {-10,0},{-8,-6},{-6,-6},{-4,-6},{-2,-6},{ 0,-6},{ 2,-6},{ 4,-6},{ 6,-6},{ 8,-6},     // +  0
      {-9,-5},{-7,-5},{-5,-5},{-3,-5},{-1,-5},{ 1,-5},{ 3,-5},{ 5,-5},{ 7,-5},{ 9,-5}, // + 10
  {-9, 1},{-8,-4},{-6,-4},{-4,-4},{-2,-4},{ 0,-4},{ 2,-4},{ 4,-4},{ 6,-4},{ 8,-4},     // + 20
      {-9,-3},{-7,-3},{-5,-3},{-3,-3},{-1,-3},{ 1,-3},{ 3,-3},{ 5,-3},{ 7,-3},{ 9,-3}, // + 30
  {-10,2},{-8,-2},{-6,-2},{-4,-2},{-2,-2},{ 0,-2},{ 2,-2},{ 4,-2},{ 6,-2},{ 8,-2},     // + 40
      {-9,-1},{-7,-1},{-5,-1},{-3,-1},{-1,-1},{ 1,-1},{ 3,-1},{ 5,-1},{ 7,-1},{ 9,-1}, // + 50
  {-9, 3},{-8, 0},{-6, 0},{-4, 0},{-2, 0},{ 0, 0},{ 2, 0},{ 4, 0},{ 6, 0},{ 8, 0},     // + 60
      {-9, 1},{-7, 1},{-5, 1},{-3, 1},{-1, 1},{ 1, 1},{ 3, 1},{ 5, 1},{ 7, 1},{ 9, 1}, // + 70
  {-10,4},{-8, 2},{-6, 2},{-4, 2},{-2, 2},{ 0, 2},{ 2, 2},{ 4, 2},{ 6, 2},{ 8, 2},     // + 80
      {-9, 3},{-7, 3},{-5, 3},{-3, 3},{-1, 3},{ 1, 3},{ 3, 3},{ 5, 3},{ 7, 3},{ 9, 3}, // + 90
  {-9, 5},{-8, 4},{-6, 4},{-4, 4},{-2, 4},{ 0, 4},{ 2, 4},{ 4, 4},{ 6, 4},{ 8, 4},     // +100
      {-9, 5},{-7, 5},{-5, 5},{-3, 5},{-1, 5},{ 1, 5},{ 3, 5},{ 5, 5},{ 7, 5},{ 9, 5}, // +110
  {-10,6},{-8, 6},{-6, 6},{-4, 6},{-2, 6},{ 0, 6},{ 2, 6},{ 4, 6},{ 6, 6},{ 8, 6},     // +120
      {-9, 7},{-7, 7},{-5, 7},{-3, 7},{-1, 7},{ 1, 7},{ 3, 7},{ 5, 7},{ 7, 7},{ 9, 7}  // +130
};


/*
  Note names and palette arrays are allocated in memory
  at runtime. Their usable size is based on the number
  of steps (in standard tuning, semitones) in a tuning 
  system before a new period is reached (in standard
  tuning, the octave). This value provides a maximum
  array size that handles almost all useful tunings
  without wasting much space.
  In the future this will not be necessary since
  we'll just store things as vectors.
*/
#define MAX_SCALE_DIVISIONS 72
