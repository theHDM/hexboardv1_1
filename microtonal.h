#pragma once
#include "utils.h" // string types
#include <GEM_u8g2.h> // select option tables
/*
  Most users will stick to playing in standard Western
  tuning, but for those looking to play microtonally,
  the Hexboard accommodates equal step tuning systems
  of any arbitrary size.
*/
/*
  Each tuning system needs to be
  pre-defined, pre-counted, and enumerated as below.
  Future editions of this sketch may enable free
  definition and smart pointer references to tuning
  presets without requiring an enumeration.
*/
#define TUNINGCOUNT 13
#define TUNING_12EDO 0
#define TUNING_17EDO 1
#define TUNING_19EDO 2
#define TUNING_22EDO 3
#define TUNING_24EDO 4
#define TUNING_31EDO 5
#define TUNING_41EDO 6
#define TUNING_53EDO 7
#define TUNING_72EDO 8
#define TUNING_BP 9
#define TUNING_ALPHA 10
#define TUNING_BETA 11
#define TUNING_GAMMA 12     
/*
  Note names and palette arrays are allocated in memory
  at runtime. Their usable size is based on the number
  of steps (in standard tuning, semitones) in a tuning 
  system before a new period is reached (in standard
  tuning, the octave). This value provides a maximum
  array size that handles almost all useful tunings
  without wasting much space.
*/
#define MAX_SCALE_DIVISIONS 72
/*
  A dictionary of musical scales is defined in the code.
  A scale is tied to one tuning system, with the exception
  of "no scale" (i.e. every note is part of the scale).
  "No scale" is tied to this value "ALL_TUNINGS" so it can
  always be chosen in the menu.
*/
#define ALL_TUNINGS 255
/*
  MIDI notes are enumerated 0-127 (7 bits).
  Values of 128-255 can be used to indicate
  command instructions for non-note buttons.
  These definitions support this function.
*/
#define CMDB 192
#define UNUSED_NOTE 255
/*
  This class provides the seed values
  needed to map buttons to note frequencies
  and palette colors, and to populate
  the menu with correct key names and
  scale choices, for a given equal step
  tuning system.
*/
class tuningDef {
public:
  str name;         // limit is 17 characters for GEM menu
  uint cycleLength;         // steps before period/cycle/octave repeats
  float stepSize;           // in cents, 100 = "normal" semitone.
  SelectOptionInt keyChoices[MAX_SCALE_DIVISIONS];
  int spanCtoA() {
    return keyChoices[0].val_int;
  }
};
/*
  Note that for all practical musical purposes,
  expressing step sizes to six significant figures is
  sufficient to eliminate any detectable tuning artifacts
  due to rounding.
  
  The note names are formatted in an array specifically to
  match the format needed for the GEM Menu to accept directly
  as a spinner selection item. The number next to the note name
  is the number of steps from the anchor note A that key is.
  
  There are other ways the tuning could be calculated.
  Some microtonal players choose an anchor note
  other than A 440. Future versions will allow for
  more flexibility in anchor selection, which will also
  change the implementation of key options.
*/ 
tuningDef tuningOptions[] = {
  { "12 EDO", 12, 100.000, 
    {{"C" ,-9},{"C#",-8},{"D" ,-7},{"Eb",-6},{"E" ,-5},{"F",-4}
    ,{"F#",-3},{"G" ,-2},{"G#",-1},{"A" , 0},{"Bb", 1},{"B", 2}
  }},
  { "17 EDO", 17, 70.5882, 
    {{"C",-13},{"Db",-12},{"C#",-11},{"D",-10},{"Eb",-9},{"D#",-8}
    ,{"E", -7},{"F" , -6},{"Gb", -5},{"F#",-4},{"G", -3},{"Ab",-2}
    ,{"G#",-1},{"A" ,  0},{"Bb",  1},{"A#", 2},{"B",  3}
  }},
  { "19 EDO", 19, 63.1579, 
    {{"C" ,-14},{"C#",-13},{"Db",-12},{"D",-11},{"D#",-10},{"Eb",-9},{"E",-8}
    ,{"E#", -7},{"F" , -6},{"F#", -5},{"Gb",-4},{"G",  -3},{"G#",-2}
    ,{"Ab", -1},{"A" ,  0},{"A#",  1},{"Bb", 2},{"B",   3},{"Cb", 4}
  }},  
  { "22 EDO", 22, 54.5455, 
    {{" C", -17},{"^C",-16},{"vC#",-15},{"vD",-14},{" D",-13},{"^D",-12}
    ,{"^Eb",-11},{"vE",-10},{" E",  -9},{" F", -8},{"^F", -7},{"vF#",-6}
    ,{"vG",  -5},{" G", -4},{"^G",  -3},{"vG#",-2},{"vA", -1},{" A",  0}
    ,{"^A",   1},{"^Bb", 2},{"vB",   3},{" B",  4}
  }},
  { "24 EDO", 24, 50.0000, 
    {{"C", -18},{"C+",-17},{"C#",-16},{"Dd",-15},{"D",-14},{"D+",-13}
    ,{"Eb",-12},{"Ed",-11},{"E", -10},{"E+", -9},{"F", -8},{"F+", -7}
    ,{"F#", -6},{"Gd", -5},{"G",  -4},{"G+", -3},{"G#",-2},{"Ad", -1}
    ,{"A",   0},{"A+",  1},{"Bb",  2},{"Bd",  3},{"B",  4},{"Cd",  5}
  }},
  { "31 EDO", 31, 38.7097, 
    {{"C",-23},{"C+",-22},{"C#",-21},{"Db",-20},{"Dd",-19}
    ,{"D",-18},{"D+",-17},{"D#",-16},{"Eb",-15},{"Ed",-14}
    ,{"E",-13},{"E+",-12}                      ,{"Fd",-11}
    ,{"F",-10},{"F+", -9},{"F#", -8},{"Gb", -7},{"Gd", -6}
    ,{"G", -5},{"G+", -4},{"G#", -3},{"Ab", -2},{"Ad", -1}
    ,{"A",  0},{"A+",  1},{"A#",  2},{"Bb",  3},{"Bd",  4}
    ,{"B",  5},{"B+",  6}                      ,{"Cd",  7}
  }},
  { "41 EDO", 41, 29.2683, 
    {{" C",-31},{"^C",-30},{" C+",-29},{" Db",-28},{" C#",-27},{" Dd",-26},{"vD",-24}
    ,{" D",-24},{"^D",-23},{" D+",-22},{" Eb",-21},{" D#",-20},{" Ed",-19},{"vE",-18}
    ,{" E",-17},{"^E",-16}                                                ,{"vF",-15}
    ,{" F",-14},{"^F",-13},{" F+",-12},{" Gb",-11},{" F#",-10},{" Gd", -9},{"vG", -8}
    ,{" G", -7},{"^G", -6},{" G+", -5},{" Ab", -4},{" G#", -3},{" Ad", -2},{"vA", -1}
    ,{" A",  0},{"^A",  1},{" A+",  2},{" Bb",  3},{" A#",  4},{" Bd",  5},{"vB",  6}
    ,{" B",  7},{"^B",  8}                                                ,{"vC",  9}
  }},
  { "53 EDO", 53, 22.6415, 
    {{" C", -40},{"^C", -39},{">C",-38},{"vDb",-37},{"Db",-36}
    ,{" C#",-35},{"^C#",-34},{"<D",-33},{"vD", -32}
    ,{" D", -31},{"^D", -30},{">D",-29},{"vEb",-28},{"Eb",-27}
    ,{" D#",-26},{"^D#",-25},{"<E",-24},{"vE", -23}
    ,{" E", -22},{"^E", -21},{">E",-20},{"vF", -19}
    ,{" F", -18},{"^F", -17},{">F",-16},{"vGb",-15},{"Gb",-14}
    ,{" F#",-13},{"^F#",-12},{"<G",-11},{"vG", -10}
    ,{" G",  -9},{"^G",  -8},{">G", -7},{"vAb", -6},{"Ab", -5}
    ,{" G#", -4},{"^G#", -3},{"<A", -2},{"vA",  -1}
    ,{" A",   0},{"^A",   1},{">A",  2},{"vBb",  3},{"Bb",  4}
    ,{" A#",  5},{"^A#",  6},{"<B",  7},{"vB",   8}
    ,{" B",   9},{"^B",  10},{"<C", 11},{"vC",  12}
  }},
  { "72 EDO", 72, 16.6667, 
    {{" C", -54},{"^C", -53},{">C", -52},{" C+",-51},{"<C#",-50},{"vC#",-49}
    ,{" C#",-48},{"^C#",-47},{">C#",-46},{" Dd",-45},{"<D" ,-44},{"vD" ,-43}
    ,{" D", -42},{"^D", -41},{">D", -40},{" D+",-39},{"<Eb",-38},{"vEb",-37}
    ,{" Eb",-36},{"^Eb",-35},{">Eb",-34},{" Ed",-33},{"<E" ,-32},{"vE" ,-31}
    ,{" E", -30},{"^E", -29},{">E", -28},{" E+",-27},{"<F" ,-26},{"vF" ,-25}
    ,{" F", -24},{"^F", -23},{">F", -22},{" F+",-21},{"<F#",-20},{"vF#",-19}
    ,{" F#",-18},{"^F#",-17},{">F#",-16},{" Gd",-15},{"<G" ,-14},{"vG" ,-13}
    ,{" G", -12},{"^G", -11},{">G", -10},{" G+", -9},{"<G#", -8},{"vG#", -7}
    ,{" G#", -6},{"^G#", -5},{">G#", -4},{" Ad", -3},{"<A" , -2},{"vA" , -1}
    ,{" A",   0},{"^A",   1},{">A",   2},{" A+",  3},{"<Bb",  4},{"vBb",  5}
    ,{" Bb",  6},{"^Bb",  7},{">Bb",  8},{" Bd",  9},{"<B" , 10},{"vB" , 11}
    ,{" B",  12},{"^B",  13},{">B",  14},{" Cd", 15},{"<C" , 16},{"vC" , 17}
  }},
  { "Bohlen-Pierce", 13, 146.304, 
    {{"C",-10},{"Db",-9},{"D",-8},{"E",-7},{"F",-6},{"Gb",-5}
    ,{"G",-4},{"H",-3},{"Jb",-2},{"J",-1},{"A",0},{"Bb",1},{"B",2}
  }},
  { "Carlos Alpha", 9, 77.9650, 
    {{"I",0},{"I#",1},{"II-",2},{"II+",3},{"III",4}
    ,{"III#",5},{"IV-",6},{"IV+",7},{"Ib",8}
  }},
  { "Carlos Beta", 11, 63.8329,
    {{"I",0},{"I#",1},{"IIb",2},{"II",3},{"II#",4},{"III",5}
    ,{"III#",6},{"IVb",7},{"IV",8},{"IV#",9},{"Ib",10}
  }},
  { "Carlos Gamma", 20, 35.0985,
    {{" I",  0},{"^I",  1},{" IIb", 2},{"^IIb", 3},{" I#",   4},{"^I#",   5}
    ,{" II", 6},{"^II", 7}
    ,{" III",8},{"^III",9},{" IVb",10},{"^IVb",11},{" III#",12},{"^III#",13}
    ,{" IV",14},{"^IV",15},{" Ib", 16},{"^Ib", 17},{" IV#", 18},{"^IV#", 19}
  }},
};