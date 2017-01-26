#include "Arduino.h"
#include "valleyservo.h"

valleyservo::valleyservo()
{
  
}

void valleyservo::Init(int pin, int minidelay)
{
  // setup the pins on the microcontroller:
  pinMode(pin, OUTPUT);

  thisservo.attach(pin);
  
  min_delay = minidelay;
  
  cur_pos = INIT_POS;
  target_pos = INIT_POS;
  
  thisservo.write(cur_pos);
  last_step_time = millis();

}

int valleyservo::getpos()
{
  return cur_pos;
}

bool valleyservo::gotarget()  //run once
{
  //return if it is in running
  if (millis() >= last_step_time) 
  {
    cur_pos = target_pos;
    return false; //not running
  }
  else
    return true; //rnnning

}

int valleyservo::settarget_abs(int steps)
{
  if (steps < 80 || steps >220) return -1;
  target_pos = steps;
  thisservo.write(target_pos*10);
  last_step_time = millis() + min_delay * abs(target_pos - cur_pos);
  return 0;
}

