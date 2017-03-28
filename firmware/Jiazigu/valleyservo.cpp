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

  step_delay = minidelay;

  cur_pos = INIT_POS;
  target_pos = INIT_POS;

  thisservo.write(cur_pos * 10);
  last_step_time = millis();
  state = SERVO_STATE_NULL;
  
}

int valleyservo::getpos()
{
  return cur_pos;
}

bool valleyservo::gotarget()  //run once
{
  switch(this->state)
  {
  case  SERVO_STATE_NULL:
    return false;
  break;
  
  case  SERVO_STATE_BEFORE_MID:
  if (millis() - this->last_step_time > this->step_delay)
  {
    if(cur_pos == middle_pos)
    {
      this->state = SERVO_STATE_MID;
    }
    else 
    {
      if (cur_pos < middle_pos) cur_pos ++; else cur_pos--;
      thisservo.write(cur_pos * 10);
    }
    this->last_step_time = millis();
  }
  break;
  
  case  SERVO_STATE_MID:
  if (millis() - this->last_step_time > STOP_IN_MIDDLE)
  {
    this->state = SERVO_STATE_BEFORE_TGT;
    this->last_step_time = millis();
  }
  break;
  
  case  SERVO_STATE_BEFORE_TGT:
  if (millis() - this->last_step_time > this->step_delay)
  {
    if(cur_pos == target_pos)
    {
      this->state = SERVO_STATE_NULL;
      return false;
    }
    else 
    {
      if (cur_pos < target_pos) cur_pos ++; else cur_pos--;
      thisservo.write(cur_pos * 10);
      this->last_step_time = millis();
    }
  }  
  break;
  default:
    return false;  //not running
  break;
  }
  return true;  //isrunning
}

int valleyservo::settarget_abs(unsigned long finishedtime, int mid_pos, int tgt_pos)
{
  if (((cur_pos < mid_pos) && (mid_pos > tgt_pos)) || ((cur_pos > mid_pos) && (mid_pos < tgt_pos)))
  {
    this->state = SERVO_STATE_BEFORE_MID;
    this->middle_pos = mid_pos;
    this->target_pos = tgt_pos;
    if (cur_pos < mid_pos) cur_pos ++; else cur_pos--;
    step_delay = (finishedtime - millis() - STOP_IN_MIDDLE) / (abs(mid_pos - cur_pos) + abs(tgt_pos-mid_pos));
  }
  else
  {
    this->state = SERVO_STATE_BEFORE_TGT;
    this->target_pos = tgt_pos;
    if (cur_pos < tgt_pos) cur_pos ++; else cur_pos--;
    step_delay = (finishedtime - millis()) / abs(tgt_pos-cur_pos);
  }

  thisservo.write(cur_pos * 10);
  last_step_time = millis();
  return 0;
}

