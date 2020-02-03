#include <LiquidCrystal.h>
#include "scheduler.h"
#include "exponentialaveragingfilter.h"

// Analog pin for joystick x
#define PIN_JOYSTICK_X A9
// Analog pin for joystick y
#define PIN_JOYSTICK_Y A8
// Digital pin for joystick switch
#define PIN_JOYSTICK_SWITCH 21
// Analog pin for photoresistor
#define PIN_PHOTORESISTOR A10

// LCD pins
#define LCD_RS 8
#define LCD_EN 9
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7

struct message {
  uint8_t x_pos;
  uint8_t y_pos;
  uint8_t pressed;
};

struct message global_message = { 69, 69, 69 };

bool is_alive = true;

CREATE_EXPONENTIAL_FILTER(joystick_x);
CREATE_EXPONENTIAL_FILTER(joystick_y);

LiquidCrystal lcd(LCD_RS, LCD_EN, LCD_D4, LCD_D5, LCD_D6, LCD_D7);
char lcd_message[4][17] = { "Kill me quick   ", "                ",
                            "                ", "I am dead......." };

void update_remote() {
  // send global message
  Serial.println("Updating remote.");
  Serial.flush();
  Serial1.write(global_message.x_pos);
  Serial1.write(global_message.y_pos);
  Serial1.write(global_message.pressed);
  Serial1.flush();
}

void update_lcd() {
  if(is_alive) {
    lcd.setCursor(0,0);
    lcd.print(lcd_message[0]);
    lcd.setCursor(0,1);
    lcd.print(lcd_message[1]);
  } else {
    lcd.setCursor(0,0);
    lcd.print(lcd_message[2]);
    lcd.setCursor(0,1);
    lcd.print(lcd_message[3]);
  }
}

void update_from_external() {
  static bool previously_pressed = false;
  
  // read in joystick data to message
  int x_pos = (int)run_exponential_average_filter(&joystick_x);
  int y_pos = (int)run_exponential_average_filter(&joystick_y);

  global_message.x_pos = map(x_pos, 0, 1023, 0, 255);
  global_message.y_pos = map(y_pos, 0, 1023, 0, 255);

  if(digitalRead(PIN_JOYSTICK_SWITCH) == LOW) {
    if(!previously_pressed) {
      global_message.pressed = !global_message.pressed;
      previously_pressed = true;
    } else {
      previously_pressed = !previously_pressed;
    }
  }
}

void setup() {
  // Setup scheduler
  Scheduler_Init();
  Scheduler_StartTask(0, 250, update_remote);
  Scheduler_StartTask(200, 500, update_lcd);
  Scheduler_StartTask(0, 100, update_from_external);

  // Bluetooth
  Serial1.begin(9600);
  Serial.begin(9600);

  // Joystick
  pinMode(PIN_JOYSTICK_X, INPUT);
  init_exponential_average_filter(&joystick_x, 0.75, PIN_JOYSTICK_X);
  pinMode(PIN_JOYSTICK_Y, INPUT);
  init_exponential_average_filter(&joystick_y, 0.75, PIN_JOYSTICK_Y);
  pinMode(PIN_JOYSTICK_SWITCH, INPUT);

  // Photo Resistor
  pinMode(PIN_PHOTORESISTOR, INPUT);

  lcd.begin(16,2);
  lcd.print("IMPL :')");
}

void loop() {
  uint32_t idle_time = Scheduler_Dispatch();
  if (idle_time) {
    delay(idle_time);
  }
}
