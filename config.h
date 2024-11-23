#pragma once
#include "utils.h"

const bool diagnostics_on = true;

// wiring switch states for each pin
enum {
  unused_pin = 0,  // ignored, even if it does something
  hardwired = 1,   // read only once at startup 
  digital_key = 2, // monitor during operation
  analog_key = 3,  // monitor during operation
};
struct key_identification {
  uint column_pin_index;  // 0..x, x = # of colPins
  uint multiplexer_value; // 0..(2^n)-1, n = # of muxPins
  uint switch_type = unused_pin;   // use enum above
  int hex_coordinate_x = N_A;   // physical location on board
  int hex_coordinate_y = N_A;   // physical location on board
  int associated_pixel = N_A;  // as counted on neoPixel strip 
};

// Hardware pin constants

// If you rewire the HexBoard then
// change these pin values
                      //  x8 x4 x2 x1
const uint_vec muxPins = {4, 5, 2, 3};
const uint_vec colPins = {6,7,8,9,10,11,12,13,14,15};
// and this big data table, too, while you're at it:
const std::vector<key_identification> config_hexboard_layout = {
  //col mux switch type   x   y  pxl
  { 0,  0, digital_key, -10,  0,   0 },
  { 0,  1, digital_key,  -9, -5,  10 },
  { 0,  2, digital_key, -11,  1,  20 },
  { 0,  3, digital_key,  -9, -3,  30 },
  { 0,  4, digital_key, -10,  2,  40 },
  { 0,  5, digital_key,  -9, -1,  50 },
  { 0,  6, digital_key, -11,  3,  60 },
  { 0,  7, digital_key,  -9,  1,  70 },
  { 0,  8, digital_key, -10,  4,  80 },
  { 0,  9, digital_key,  -9,  3,  90 },
  { 0, 10, digital_key, -11,  5, 100 },
  { 0, 11, digital_key,  -9,  5, 110 },
  { 0, 12, digital_key, -10,  6, 120 },
  { 0, 13, digital_key,  -9,  7, 130 },
  { 0, 14, hardwired,   N_A,N_A, N_A }, // off = FW 1.0/1.1, on = FW 1.2
  { 0, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 1,  0, digital_key,  -8, -6,   1 },
  { 1,  1, digital_key,  -7, -5,  11 },
  { 1,  2, digital_key,  -8, -4,  21 },
  { 1,  3, digital_key,  -7, -3,  31 },
  { 1,  4, digital_key,  -8, -2,  41 },
  { 1,  5, digital_key,  -7, -1,  51 },
  { 1,  6, digital_key,  -8,  0,  61 },
  { 1,  7, digital_key,  -7,  1,  71 },
  { 1,  8, digital_key,  -8,  2,  81 },
  { 1,  9, digital_key,  -7,  3,  91 },
  { 1, 10, digital_key,  -8,  4, 101 },
  { 1, 11, digital_key,  -7,  5, 111 },
  { 1, 12, digital_key,  -8,  6, 121 },
  { 1, 13, digital_key,  -7,  7, 131 },
  { 1, 14, unused_pin,  N_A,N_A, N_A },
  { 1, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 2,  0, digital_key,  -6, -6,   2 },
  { 2,  1, digital_key,  -5, -5,  12 },
  { 2,  2, digital_key,  -6, -4,  22 },
  { 2,  3, digital_key,  -5, -3,  32 },
  { 2,  4, digital_key,  -6, -2,  42 },
  { 2,  5, digital_key,  -5, -1,  52 },
  { 2,  6, digital_key,  -6,  0,  62 },
  { 2,  7, digital_key,  -5,  1,  72 },
  { 2,  8, digital_key,  -6,  2,  82 },
  { 2,  9, digital_key,  -5,  3,  92 },
  { 2, 10, digital_key,  -6,  4, 102 },
  { 2, 11, digital_key,  -5,  5, 112 },
  { 2, 12, digital_key,  -6,  6, 122 },
  { 2, 13, digital_key,  -5,  7, 132 },
  { 2, 14, unused_pin,  N_A,N_A, N_A },
  { 2, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 3,  0, digital_key,  -4, -6,   3 },
  { 3,  1, digital_key,  -3, -5,  13 },
  { 3,  2, digital_key,  -4, -4,  23 },
  { 3,  3, digital_key,  -3, -3,  33 },
  { 3,  4, digital_key,  -4, -2,  43 },
  { 3,  5, digital_key,  -3, -1,  53 },
  { 3,  6, digital_key,  -4,  0,  63 },
  { 3,  7, digital_key,  -3,  1,  73 },
  { 3,  8, digital_key,  -4,  2,  83 },
  { 3,  9, digital_key,  -3,  3,  93 },
  { 3, 10, digital_key,  -4,  4, 103 },
  { 3, 11, digital_key,  -3,  5, 113 },
  { 3, 12, digital_key,  -4,  6, 123 },
  { 3, 13, digital_key,  -3,  7, 133 },
  { 3, 14, unused_pin,  N_A,N_A, N_A },
  { 3, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 4,  0, digital_key,  -2, -6,   4 },
  { 4,  1, digital_key,  -1, -5,  14 },
  { 4,  2, digital_key,  -2, -4,  24 },
  { 4,  3, digital_key,  -1, -3,  34 },
  { 4,  4, digital_key,  -2, -2,  44 },
  { 4,  5, digital_key,  -1, -1,  54 },
  { 4,  6, digital_key,  -2,  0,  64 },
  { 4,  7, digital_key,  -1,  1,  74 },
  { 4,  8, digital_key,  -2,  2,  84 },
  { 4,  9, digital_key,  -1,  3,  94 },
  { 4, 10, digital_key,  -2,  4, 104 },
  { 4, 11, digital_key,  -1,  5, 114 },
  { 4, 12, digital_key,  -2,  6, 124 },
  { 4, 13, digital_key,  -1,  7, 134 },
  { 4, 14, unused_pin,  N_A,N_A, N_A },
  { 4, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 5,  0, digital_key,   0, -6,   5 },
  { 5,  1, digital_key,   1, -5,  15 },
  { 5,  2, digital_key,   0, -4,  25 },
  { 5,  3, digital_key,   1, -3,  35 },
  { 5,  4, digital_key,   0, -2,  45 },
  { 5,  5, digital_key,   1, -1,  55 },
  { 5,  6, digital_key,   0,  0,  65 },
  { 5,  7, digital_key,   1,  1,  75 },
  { 5,  8, digital_key,   0,  2,  85 },
  { 5,  9, digital_key,   1,  3,  95 },
  { 5, 10, digital_key,   0,  4, 105 },
  { 5, 11, digital_key,   1,  5, 115 },
  { 5, 12, digital_key,   0,  6, 125 },
  { 5, 13, digital_key,   1,  7, 135 },
  { 5, 14, unused_pin,  N_A,N_A, N_A },
  { 5, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 6,  0, digital_key,   2, -6,   6 },
  { 6,  1, digital_key,   3, -5,  16 },
  { 6,  2, digital_key,   2, -4,  26 },
  { 6,  3, digital_key,   3, -3,  36 },
  { 6,  4, digital_key,   2, -2,  46 },
  { 6,  5, digital_key,   3, -1,  56 },
  { 6,  6, digital_key,   2,  0,  66 },
  { 6,  7, digital_key,   3,  1,  76 },
  { 6,  8, digital_key,   2,  2,  86 },
  { 6,  9, digital_key,   3,  3,  96 },
  { 6, 10, digital_key,   2,  4, 106 },
  { 6, 11, digital_key,   3,  5, 116 },
  { 6, 12, digital_key,   2,  6, 126 },
  { 6, 13, digital_key,   3,  7, 136 },
  { 6, 14, unused_pin,  N_A,N_A, N_A },
  { 6, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 7,  0, digital_key,   4, -6,   7 },
  { 7,  1, digital_key,   5, -5,  17 },
  { 7,  2, digital_key,   4, -4,  27 },
  { 7,  3, digital_key,   5, -3,  37 },
  { 7,  4, digital_key,   4, -2,  47 },
  { 7,  5, digital_key,   5, -1,  57 },
  { 7,  6, digital_key,   4,  0,  67 },
  { 7,  7, digital_key,   5,  1,  77 },
  { 7,  8, digital_key,   4,  2,  87 },
  { 7,  9, digital_key,   5,  3,  97 },
  { 7, 10, digital_key,   4,  4, 107 },
  { 7, 11, digital_key,   5,  5, 117 },
  { 7, 12, digital_key,   4,  6, 127 },
  { 7, 13, digital_key,   5,  7, 137 },
  { 7, 14, unused_pin,  N_A,N_A, N_A },
  { 7, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 8,  0, digital_key,   6, -6,   8 },
  { 8,  1, digital_key,   7, -5,  18 },
  { 8,  2, digital_key,   6, -4,  28 },
  { 8,  3, digital_key,   7, -3,  38 },
  { 8,  4, digital_key,   6, -2,  48 },
  { 8,  5, digital_key,   7, -1,  58 },
  { 8,  6, digital_key,   6,  0,  68 },
  { 8,  7, digital_key,   7,  1,  78 },
  { 8,  8, digital_key,   6,  2,  88 },
  { 8,  9, digital_key,   7,  3,  98 },
  { 8, 10, digital_key,   6,  4, 108 },
  { 8, 11, digital_key,   7,  5, 118 },
  { 8, 12, digital_key,   6,  6, 128 },
  { 8, 13, digital_key,   7,  7, 138 },
  { 8, 14, unused_pin,  N_A,N_A, N_A },
  { 8, 15, unused_pin,  N_A,N_A, N_A },
  //col mux switch type   x   y  pxl
  { 9,  0, digital_key,   8, -6,   9 },
  { 9,  1, digital_key,   9, -5,  19 },
  { 9,  2, digital_key,   8, -4,  29 },
  { 9,  3, digital_key,   9, -3,  39 },
  { 9,  4, digital_key,   8, -2,  49 },
  { 9,  5, digital_key,   9, -1,  59 },
  { 9,  6, digital_key,   8,  0,  69 },
  { 9,  7, digital_key,   9,  1,  79 },
  { 9,  8, digital_key,   8,  2,  89 },
  { 9,  9, digital_key,   9,  3,  99 },
  { 9, 10, digital_key,   8,  4, 109 },
  { 9, 11, digital_key,   9,  5, 119 },
  { 9, 12, digital_key,   8,  6, 129 },
  { 9, 13, digital_key,   9,  7, 139 },
  { 9, 14, unused_pin,  N_A,N_A, N_A },
  { 9, 15, unused_pin,  N_A,N_A, N_A },
};

const uint rotaryPinA = 20;
const uint rotaryPinB = 21;
const uint rotaryPinC = 24;

const uint ledCount = 140;
const uint ledPin = 22;

const uint piezoPin = 23;
const uint audioJackPin = 25;

const uint OLED_sdaPin = 16;
const uint OLED_sclPin = 17;

// These are timing constants that are hardware dependent.
// If you use different components you may need to change
// these timings if there is unexpected behavior.

const uint rotary_pin_fire_period_in_uS    =    768;
const uint keyboard_pin_reset_period_in_uS =     16;
const uint target_audio_sample_rate_in_Hz  = 31'250;
const uint target_LED_frame_rate_in_Hz     =     30;