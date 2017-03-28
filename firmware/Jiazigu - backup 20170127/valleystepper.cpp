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

  this->cur_pos = INIT_POS; //unknown position, assume one
  this->target_pos = INIT_POS;
  this->last_step_time = millis();

}
extern int debug_code1;
extern int debug_code2;

void valleystepper::stepMotor(int thisStep)
{
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

int valleystepper::calc_delay()
{
  int fromdelay, todelay;
  int fromstart = abs(cur_pos - start_pos);
  int toto = abs(target_pos - cur_pos);

  if (fromstart < 10) fromdelay = 20 - fromstart;
  else fromdelay = 10;

  if (toto < 10) todelay = 20 - toto;
  else todelay = 10;

  if (fromdelay > todelay) return fromdelay;
  else return todelay;

}

int valleystepper::goonestep(boolean forward)
{
  unsigned long now = millis();
//  if (now - this->last_step_time >= calc_delay())
  if (now - this->last_step_time >= 10)
  {
    //could step
    if(forward) this->cur_pos++;
    else this->cur_pos--;
    stepMotor(this->cur_pos % 4);
    this->last_step_time = now;
  }
  return this->cur_pos;
}

bool valleystepper::gotarget()  //run once
{
  bool isrunning = true;

  if (this->cur_pos < this->target_pos)
    goonestep(true);
  else if (this->cur_pos > this->target_pos)
    goonestep(false);
  else
    isrunning = false;

  return isrunning;

}

int valleystepper::settarget_abs(int steps)
{
  if (steps < 1450 || steps > 1550) return -1;
  this->target_pos = steps;
  start_pos = cur_pos;
  return 0;
}

