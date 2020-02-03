#include "scheduler.h"
#include <Servo.h>

#define LASER_PIN 13

struct message {
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t pressed;
};
struct message global_message = {};
Servo sweep_servo;
Servo tilt_servo;
int tilt_pos = 0;
int sweep_pos = 0;

int new_pos(int joystick_val, int current_pos) {
  //To safely test, if joystick val is past a threshhold move position by 1
  int joystick_middle = 512;
  int joystick_threshold = 10;
  int min_pos = 0;
  int max_pos = 180;  
  if(joystick_val < joystick_middle - joystick_threshold && tilt_pos > min_pos) {
    return current_pos - 1;
  }else if(joystick_val > joystick_middle + joystick_threshold && tilt_pos < max_pos){
    return current_pos += 1;
  }else {
    return current_pos;
  }
}

void update_message() {
  byte temp = -1;
  while(temp == -1) 
    temp = Serial1.read();
 
  global_message.x_pos = temp;
 
  temp = -1;
  while(temp == -1) 
     temp = Serial1.read();
 
  global_message.y_pos = temp;
 
  temp = -1;
  while(temp == -1) 
     temp = Serial1.read();
 
  global_message.pressed = temp;

  Serial1.flush();
}

void check_servos_and_laser() {
  sweep_pos = new_pos(global_message.x_pos, sweep_pos);
  tilt_pos = new_pos(global_message.y_pos, tilt_pos);

  tilt_servo.write(tilt_pos);
  sweep_servo.write(sweep_pos);
  
  if(global_message.pressed){
  analogWrite(LASER_PIN, 255);
  }
}

void setup() {
  //setup bluetooth
  Serial.begin(9600);
  Serial1.begin(9600);

  //setup servo both sweep and tilt
  tilt_servo.attach(9);
  sweep_servo.attach(10);

  //setup laser pointer, can use analogwrite to adjust brightness
  pinMode(LASER_PIN, OUTPUT);

  //setup schedule
  Scheduler_StartTask(0, 300, update_message);
  Scheduler_StartTask(0, 500, check_servos_and_laser);
}

void loop() {
  // put your main code here, to run repeatedly:
    uint32_t idle_period = Scheduler_Dispatch();
    if (idle_period)
    {
      delay(idle_period);
    }

}
