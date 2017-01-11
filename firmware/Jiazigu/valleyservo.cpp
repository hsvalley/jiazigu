#include "Arduino.h"
#include "valleyservo.h"

#define INIT_POS 1500
valleyservo::valleyservo()
{
  
}

void valleyservo::Init(int pin, int minidelay)
{
  // setup the pins on the microcontroller:
  pinMode(pin, OUTPUT);

  thisservo.attach(pin);
  
  min_delay = minidelay;
  
  state = 0;
  cur_pos = INIT_POS;
  
  thisservo.write(cur_pos);
  last_step_time = millis();

}

int valleyservo::getpos()
{
  return cur_pos;
}

bool valleyservo::gotarget()  //run once
{
  if (millis() >= last_step_time) 
  {
    cur_pos = target_pos;
    return false;
  }
  else
    return true;

}

void valleyservo::settarget_relative(int steps)
{
  target_pos = cur_pos + steps;
  thisservo.write(target_pos);
  last_step_time = millis() + min_delay * abs(target_pos - cur_pos)/10;

}

void valleyservo::settarget_abs(int steps)
{
  target_pos = steps;
  thisservo.write(target_pos);
  last_step_time = millis() + min_delay * abs(target_pos - cur_pos)/10;

}

