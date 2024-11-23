#pragma once
#include "utils.h"
#include "config.h"             // contains pin and timing definitions
#include "timing.h"
#include "hwRotary.h"
#include "hwKeys.h"
#include "hwAudio.h"
#include "hardware/irq.h"       // library of code to let you interrupt code execution to run something of higher priority


constexpr uint target_audio_sample_halfperiod_in_uS = 500'000 / target_audio_sample_rate_in_Hz;
constexpr uint hardware_tick_period_in_uS = 
  ( target_audio_sample_halfperiod_in_uS > keyboard_pin_reset_period_in_uS
  ? target_audio_sample_halfperiod_in_uS : keyboard_pin_reset_period_in_uS ); 
constexpr uint actual_audio_sample_period_in_uS = 2 * hardware_tick_period_in_uS;
constexpr uint actual_audio_sample_rate_in_Hz  = 1'000'000 / actual_audio_sample_period_in_uS;

// this worked on Nov 7 2024
// tested on binji.github.io/wasm-clang

// task manager takes a list of repeat periods and callback functions
// these should run in the background
// set an alarm to run the "on_irq" boolean every "tick" microseconds.
// load in the background functions you want using the "bind" language.

static void on_irq();

class task_mgr_obj {
  private:
    struct task_obj {
      uint counter = 0;
      uint period;
      func exec_on_trigger;
      void set_period(uint arg_period) {
        period = arg_period;
      }
      void set_trigger(func arg_func) {
        exec_on_trigger = arg_func;
      }
      void increment(uint add_uS) {
        counter += add_uS;
      }
      void execute() {
        exec_on_trigger();
      }
      bool triggered() {
        if (counter >= period) {
          counter = counter % period;
          return true;
        }
        return false;
      }
    };
    uint tick_uS = 0;
    time_uS next_trigger = 0;
    uint8_t alarm_ID = 0;
    public:
      void init(uint arg_uS) {
        tick_uS = arg_uS;
        next_trigger = getTheCurrentTime();
      }
      std::vector<task_obj> task_list;
      uint get_tick_uS() {
        return tick_uS;
      }
      void add_task(uint arg_repeat_uS, func arg_on_trigger) {
        task_obj new_task;
        new_task.set_period(arg_repeat_uS);
        new_task.set_trigger(arg_on_trigger);
        task_list.emplace_back(new_task);
      }
      void set_timer() {
        next_trigger += tick_uS;
        timer_hw->alarm[alarm_ID] = next_trigger;
      }
      void begin() {
        hw_set_bits(&timer_hw->inte, 1u << alarm_ID);  // initialize the timer
        irq_set_exclusive_handler(alarm_ID, on_irq);     // function to run every interrupt
        irq_set_enabled(alarm_ID, true);               // ENGAGE!
        set_timer();
      }
      void repeat_timer() {
        hw_clear_bits(&timer_hw->intr, 1u << alarm_ID);
        set_timer();
      }
};

// global definition, required to declare up front
task_mgr_obj task_mgr;

// global routine, required by RP2040 interrupt process
static void on_irq() {
  task_mgr.repeat_timer();
  uint t = task_mgr.get_tick_uS();
  for (auto& i : task_mgr.task_list) {
    i.increment(t);
  }
  for (auto& i : task_mgr.task_list) {
    if (i.triggered()) {
        i.execute();
        break;
    }
  }
}

// global, call this on setup() i.e. the 1st core
void hardware_setup() {
  // note that the audio pins can be set by the user, and that routine can live outside
  // of the setup. in other words, it is safe to run audioOut in the background even
  // if there are no audio outputs selected.
  audioOut.setup(actual_audio_sample_rate_in_Hz);
  // based on settings, enable whichever audio out puns
  audioOut.set_pin(piezoPin, true);
  //  rotary should define a pin A and B. pin C is the center click button if it exists
  rotary.setup(rotaryPinA,rotaryPinB,rotaryPinC);
  //  first T/F: are the column pins analog?
  //  second T/F: iterate thru the multiplex pins before the column pins?
  pinGrid.setup(colPins, false, muxPins, true);
}

// global, call this on setup1() i.e. the 2nd core
void hardware_start_background_process() {
    task_mgr.init(hardware_tick_period_in_uS);
    // audio sample update - highest priority (stable period needed)
    task_mgr.add_task(actual_audio_sample_period_in_uS, std::bind(&audioOut_obj::poll, &audioOut));
    // rotary knob - 2nd highest priority (input drop risk)
    task_mgr.add_task(rotary_pin_fire_period_in_uS,     std::bind(&rotary_obj::poll,   &rotary));
    // keyboard - lowest priority (timing requirements are loose)
    task_mgr.add_task(keyboard_pin_reset_period_in_uS,  std::bind(&pinGrid_obj::poll,  &pinGrid));
    // start receiving input and processing audio output
    task_mgr.begin();
}