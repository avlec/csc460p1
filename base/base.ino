#include "scheduler.h"
#include "exponentialaveragingfilter.h"

// Analog pin for joystick x
#define PIN_JOYSTICK_X 9
// Analog pin for joystick y
#define PIN_JOYSTICK_Y 8
// Digital pin for joystick switch
#define PIN_JOYSTICK_SWITCH 21
// Analog pin for photoresistor
#define PIN_PHOTORESISTOR 10

struct message {
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t pressed;
};

struct message global_message = { 0, 0, 0 };

CREATE_EXPONENTIAL_FILTER(joystick_x);
CREATE_EXPONENTIAL_FILTER(joystick_y);


void update_remote() {
  // send global message
}

void update_lcd() {
  
}

void update_from_external() {
  // read in joystick data to message
  // update 
}

void setup() {
  Scheduler_Init();
  Scheduler_StartTask(0, 250, update_remote);
  Scheduler_StartTask(0, 500, update_lcd);
  Scheduler_StartTask(0, 100, update_from_external);

  pinMode(PIN_JOYSTICK_X, INPUT);
  init_exponential_average_filter(&joystick_x, 0.75, PIN_JOYSTICK_X);
  pinMode(PIN_JOYSTICK_Y, INPUT);
  init_exponential_average_filter(&joystick_y, 0.75, PIN_JOYSTICK_Y);
  
  pinMode(PIN_JOYSTICK_SWITCH, INPUT);
  pinMode(PIN_PHOTORESISTOR, INPUT);
}

void loop() {
  uint32_t idle_time = Scheduler_Dispatch();
  if (idle_time) {
    delay(idle_time);
  }
}
