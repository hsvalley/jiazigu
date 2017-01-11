/*
   The sequence of controls signals for 2 control wires is as follows
   (columns C1 and C2 from above):

   Step C0 C1
      1  0  1
      2  1  1
      3  1  0
      4  0  0
*/

#include "Arduino.h"
#include "valleystepper.h"

#define INIT_POS 1500

valleystepper::valleystepper()
{

}

void valleystepper::Init(int pin_1, int pin_2, int minidelay)
{
  // Arduino pins for the motor control connection:
  this->motor_pin_1 = pin_1;
  this->motor_pin_2 = pin_2;

  // setup the pins on the microcontroller:
  pinMode(this->motor_pin_1, OUTPUT);
  pinMode(this->motor_pin_2, OUTPUT);

  this->min_delay = minidelay;

  state = 0;
  this->cur_pos = INIT_POS; //unknown position, assume one
  this->last_step_time = millis();

}
extern int debug_code1;
extern int debug_code2;

void valleystepper::stepMotor(int thisStep)
{
  debug_code1++;
  debug_code2 +=thisStep;
  
  switch (thisStep) {
    case 0:  // 01
      digitalWrite(this->motor_pin_1, LOW);
      digitalWrite(this->motor_pin_2, HIGH);
      break;
    case 1:  // 11
      digitalWrite(this->motor_pin_1, HIGH);
      digitalWrite(this->motor_pin_2, HIGH);
      break;
    case 2:  // 10
      digitalWrite(this->motor_pin_1, HIGH);
      digitalWrite(this->motor_pin_2, LOW);
      break;
    case 3:  // 00
      digitalWrite(this->motor_pin_1, LOW);
      digitalWrite(this->motor_pin_2, LOW);
      break;
  }
}

int valleystepper::getpos()
{
  return cur_pos;
}

int valleystepper::goonestep()
{
  unsigned long now = millis();
  if (now - this->last_step_time >= this->min_delay)
  {
    //could step
    this->cur_pos++;
    stepMotor(this->cur_pos % 4);
    this->last_step_time = now;
  }
  return this->cur_pos;
}

int valleystepper::backonestep()
{
  unsigned long now = millis();
  if (now - this->last_step_time >= min_delay)
  {
    //could step
    this->cur_pos--;
    stepMotor(this->cur_pos % 4);
    this->last_step_time = now;
  }
  return this->cur_pos;
}

bool valleystepper::gotarget()  //run once
{
  bool unfinished = true;

  if (this->cur_pos < this->target_pos)
    goonestep();
  else if (this->cur_pos > this->target_pos)
    backonestep();
  else
    unfinished = false;

  return unfinished;

}

void valleystepper::settarget_relative(int steps)
{
  this->target_pos = this->cur_pos + steps;
}

void valleystepper::settarget_abs(int steps)
{
  this->target_pos = steps;
}

