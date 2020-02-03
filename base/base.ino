#include "scheduler.h"
#include "exponentialaveragingfilter.h"

struct message {
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t pressed;
};




void update_remote() {
  // send global message
}

void check_target() {
  // check for signal


  // if signal, set done flag
}

void joystick_handler() {
  // read in joystick data to message
}

void setup() {
  // 
  Scheduler_Init();
  Scheduler_StartTask(0, 500, update_remote);
  Scheduler_StartTask(0, 500, check_target);
  Scheduler_StartTask(0, 500, joystick_handler);

  
}

void loop() {
  // put your main code here, to run repeatedly:

}
