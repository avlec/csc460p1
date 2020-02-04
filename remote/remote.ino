#include "scheduler.h"
#include <Servo.h>

#define LASER_PIN 11
#define TILT_PIN 10
#define SWEEP_PIN 9

struct message {
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t pressed;
};
struct message global_message = {0, 0, 0};
Servo sweep_servo;
Servo tilt_servo;
int tilt_pos = 0;
int sweep_pos = 0;

int new_pos(int joystick_val, int current_pos) {
  int joystick_middle = 127;
  int movementDivider = 8;
  int movementBase = joystick_middle - joystick_val;
  
  int min_pos = 10;
  int max_pos = 170;

  int new_pos = current_pos + (movementBase / movementDivider); //the most it could be is around 25
  if(new_pos > max_pos) {
    return max_pos;
  }else if(new_pos < min_pos) {
    return min_pos;
  }else{
    return new_pos;
  }
}

void update_message() {
  if (Serial1.available() > 0) {
    global_message.x_pos = Serial1.read();
  }

  if (Serial1.available() > 0) {
    global_message.y_pos = Serial1.read();
  }

  if (Serial1.available() > 0) {
    global_message.pressed = Serial1.read();
  }
  
  Serial1.flush();
  Serial.print(global_message.x_pos);
  Serial.print(global_message.y_pos);
  Serial.print(global_message.pressed);
  Serial.print("\n");
}

void check_servos_and_laser() {
  sweep_pos = new_pos(global_message.x_pos, sweep_pos);
  tilt_pos = new_pos(global_message.y_pos, tilt_pos);
  Serial.print(sweep_pos);
  Serial.print(tilt_pos);
  Serial.print("\n");
  

  tilt_servo.write(tilt_pos);
  sweep_servo.write(sweep_pos);
  
  if(global_message.pressed){
    analogWrite(LASER_PIN, 255);
  } else {
    analogWrite(LASER_PIN, 0);
  }
}

void setup() {
  //setup bluetooth
  Serial.begin(9600);
  Serial1.begin(9600);

  //setup servo both sweep and tilt
  tilt_servo.attach(TILT_PIN);
  sweep_servo.attach(SWEEP_PIN);

  //setup laser pointer, can use analogwrite to adjust brightness
  pinMode(LASER_PIN, OUTPUT);

  //setup schedule
  Scheduler_StartTask(0, 250, update_message);
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
