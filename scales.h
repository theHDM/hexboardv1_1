#pragma once
#include "utils.h"
#include "microtonal.h"
/*
  This class defines a scale pattern
  for a given tuning. It is basically
  an array with the number of steps in
  between each degree of the scale. For
  example, the major scale in 12EDO
  is 2, 2, 1, 2, 2, 2, 1.
  
  A scale is tied to a specific tuning.
*/
struct scaleDef {
  str name;
  uint tuning;
  uint pattern[MAX_SCALE_DIVISIONS];
};
scaleDef scaleOptions[] = {
  { "None",              ALL_TUNINGS,      { 0 } },
  // 12 EDO
  { "Major",             TUNING_12EDO,     { 2,2,1,2,2,2,1 } },
  { "Minor, natural",    TUNING_12EDO,     { 2,1,2,2,1,2,2 } },
  { "Minor, melodic",    TUNING_12EDO,     { 2,1,2,2,2,2,1 } },
  { "Minor, harmonic",   TUNING_12EDO,     { 2,1,2,2,1,3,1 } },
  { "Pentatonic, major", TUNING_12EDO,     { 2,2,3,2,3 } },
  { "Pentatonic, minor", TUNING_12EDO,     { 3,2,2,3,2 } },
  { "Blues",             TUNING_12EDO,     { 3,1,1,1,1,3,2 } },
  { "Double Harmonic",   TUNING_12EDO,     { 1,3,1,2,1,3,1 } },
  { "Phrygian",          TUNING_12EDO,     { 1,2,2,2,1,2,2 } },
  { "Phrygian Dominant", TUNING_12EDO,     { 1,3,1,2,1,2,2 } },
  { "Dorian",            TUNING_12EDO,     { 2,1,2,2,2,1,2 } },
  { "Lydian",            TUNING_12EDO,     { 2,2,2,1,2,2,1 } },
  { "Lydian Dominant",   TUNING_12EDO,     { 2,2,2,1,2,1,2 } },
  { "Mixolydian",        TUNING_12EDO,     { 2,2,1,2,2,1,2 } },
  { "Locrian",           TUNING_12EDO,     { 1,2,2,1,2,2,2 } },
  { "Whole tone",        TUNING_12EDO,     { 2,2,2,2,2,2 } },
  { "Octatonic",         TUNING_12EDO,     { 2,1,2,1,2,1,2,1 } },
  // 17 EDO; for more: https://en.xen.wiki/w/17edo#Scales
  { "Diatonic",          TUNING_17EDO,  { 3,3,1,3,3,3,1 } },
  { "Pentatonic",        TUNING_17EDO,  { 3,3,4,3,4 } },
  { "Harmonic",          TUNING_17EDO,  { 3,2,3,2,2,2,3 } },
  { "Husayni maqam",     TUNING_17EDO,  { 2,2,3,3,2,1,1,3 } },
  { "Blues",             TUNING_17EDO,  { 4,3,1,1,1,4,3 } },
  { "Hydra",             TUNING_17EDO,  { 3,3,1,1,2,3,2,1,1 } },
  // 19 EDO; for more: https://en.xen.wiki/w/19edo#Scales
  { "Diatonic",          TUNING_19EDO,   { 3,3,2,3,3,3,2 } },
  { "Pentatonic",        TUNING_19EDO,   { 3,3,5,3,5 } },
  { "Semaphore",         TUNING_19EDO,   { 3,1,3,1,3,3,1,3,1 } },
  { "Negri",             TUNING_19EDO,   { 2,2,2,2,2,1,2,2,2,2 } },
  { "Sensi",             TUNING_19EDO,   { 2,2,1,2,2,2,1,2,2,2,1 } },
  { "Kleismic",          TUNING_19EDO,   { 1,3,1,1,3,1,1,3,1,3,1 } },
  { "Magic",             TUNING_19EDO,   { 3,1,1,1,3,1,1,1,3,1,1,1,1 } },
  { "Kind of blues",     TUNING_19EDO,   { 4,4,1,2,4,4 } },
  // 22 EDO; for more: https://en.xen.wiki/w/22edo_modes
  { "Diatonic",          TUNING_22EDO,  { 4,4,1,4,4,4,1 } },
  { "Pentatonic",        TUNING_22EDO,  { 4,4,5,4,5 } },
  { "Orwell",            TUNING_22EDO,  { 3,2,3,2,3,2,3,2,2 } },
  { "Porcupine",         TUNING_22EDO,  { 4,3,3,3,3,3,3 } },
  { "Pajara",            TUNING_22EDO,  { 2,2,3,2,2,2,3,2,2,2 } },
  // 24 EDO; for more: https://en.xen.wiki/w/24edo_scales
  { "Diatonic 12",       TUNING_24EDO, { 4,4,2,4,4,4,2 } },
  { "Diatonic Soft",     TUNING_24EDO, { 3,5,2,3,5,4,2 } },
  { "Diatonic Neutral",  TUNING_24EDO, { 4,3,3,4,3,4,3 } },
  { "Pentatonic (12)",   TUNING_24EDO, { 4,4,6,4,6 } },
  { "Pentatonic (Haba)", TUNING_24EDO, { 5,5,5,5,4 } },
  { "Invert Pentatonic", TUNING_24EDO, { 6,3,6,6,3 } },
  { "Rast maqam",        TUNING_24EDO, { 4,3,3,4,4,2,1,3 } },
  { "Bayati maqam",      TUNING_24EDO, { 3,3,4,4,2,1,3,4 } },      
  { "Hijaz maqam",       TUNING_24EDO, { 2,6,2,4,2,1,3,4 } },
  { "8-EDO",             TUNING_24EDO, { 3,3,3,3,3,3,3,3 } },
  { "Wyschnegradsky",    TUNING_24EDO, { 2,2,2,2,2,1,2,2,2,2,2,2,1 } },
  // 31 EDO; for more: https://en.xen.wiki/w/31edo#Scales
  { "Diatonic",          TUNING_31EDO,  { 5,5,3,5,5,5,3 } },
  { "Pentatonic",        TUNING_31EDO,  { 5,5,8,5,8 } },
  { "Harmonic",          TUNING_31EDO,  { 5,5,4,4,4,3,3,3 } },
  { "Mavila",            TUNING_31EDO,  { 5,3,3,3,5,3,3,3,3 } },
  { "Quartal",           TUNING_31EDO,  { 2,2,7,2,2,7,2,7 } },
  { "Orwell",            TUNING_31EDO,  { 4,3,4,3,4,3,4,3,3 } },
  { "Neutral",           TUNING_31EDO,  { 4,4,4,4,4,4,4,3 } },
  { "Miracle",           TUNING_31EDO,  { 4,3,3,3,3,3,3,3,3,3 } },
  // 41 EDO; for more: https://en.xen.wiki/w/41edo#Scales_and_modes
  { "Diatonic",          TUNING_41EDO,   { 7,7,3,7,7,7,3 } },
  { "Pentatonic",        TUNING_41EDO,   { 7,7,10,7,10 } },
  { "Pure major",        TUNING_41EDO,   { 7,6,4,7,6,7,4 } },
  { "5-limit chromatic", TUNING_41EDO,   { 4,3,4,2,4,3,4,4,2,4,3,4 } },
  { "7-limit chromatic", TUNING_41EDO,   { 3,4,2,4,4,3,4,2,4,3,3,4 } },
  { "Harmonic",          TUNING_41EDO,   { 5,4,4,4,4,3,3,3,3,3,2,3 } },
  { "Middle East-ish",   TUNING_41EDO,   { 7,5,7,5,5,7,5 } },
  { "Thai",              TUNING_41EDO,   { 6,6,6,6,6,6,5 } },
  { "Slendro",           TUNING_41EDO,   { 8,8,8,8,9 } },
  { "Pelog / Mavila",    TUNING_41EDO,   { 8,5,5,8,5,5,5 } },
  // 53 EDO
  { "Diatonic",          TUNING_53EDO, { 9,9,4,9,9,9,4 } },
  { "Pentatonic",        TUNING_53EDO, { 9,9,13,9,13 } },
  { "Rast makam",        TUNING_53EDO, { 9,8,5,9,9,4,4,5 } },
  { "Usshak makam",      TUNING_53EDO, { 7,6,9,9,4,4,5,9 } },
  { "Hicaz makam",       TUNING_53EDO, { 5,12,5,9,4,9,9 } },
  { "Orwell",            TUNING_53EDO, { 7,5,7,5,7,5,7,5,5 } },
  { "Sephiroth",         TUNING_53EDO, { 6,5,5,6,5,5,6,5,5,5 } },
  { "Smitonic",          TUNING_53EDO, { 11,11,3,11,3,11,3 } },
  { "Slendric",          TUNING_53EDO, { 7,3,7,3,7,3,7,3,7,3,3 } },
  { "Semiquartal",       TUNING_53EDO, { 9,2,9,2,9,2,9,2,9 } },
  // 72 EDO
  { "Diatonic",          TUNING_72EDO, { 12,12,6,12,12,12,6 } },
  { "Pentatonic",        TUNING_72EDO, { 12,12,18,12,18 } },
  { "Ben Johnston",      TUNING_72EDO, { 6,6,6,5,5,5,9,8,4,4,7,7 } },
  { "18-EDO",            TUNING_72EDO, { 4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4,4 } },
  { "Miracle",           TUNING_72EDO, { 5,2,5,2,5,2,2,5,2,5,2,5,2,5,2,5,2,5,2,5,2 } },
  { "Marvolo",           TUNING_72EDO, { 5,5,5,5,5,5,5,2,5,5,5,5,5,5 } },
  { "Catakleismic",      TUNING_72EDO, { 4,7,4,4,4,7,4,4,4,7,4,4,4,7,4 } },
  { "Palace",            TUNING_72EDO, { 10,9,11,12,10,9,11 } },
  // BP
  { "Lambda",            TUNING_BP, { 2,1,2,1,2,1,2,1,1 } },
  // Alpha
  { "Super Meta Lydian", TUNING_ALPHA, { 3,2,2,2 } },
  // Beta
  { "Super Meta Lydian", TUNING_BETA,  { 3,3,3,2 } },
  // Gamma
  { "Super Meta Lydian", TUNING_GAMMA, { 6,5,5,4 } }
};
const uint scaleCount = sizeof(scaleOptions) / sizeof(scaleDef);