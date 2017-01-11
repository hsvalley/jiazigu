#include <Servo.h>

#include "hand.h"

hand::hand()
{

}

void hand::Init(int leftright)
{
  if (0 == leftright)
  {
    yaostepper.Init(2, 3, 5);
    jianstepper.Init(4, 5, 5);
    zhouservo.Init(10,5);
    wanservo.Init(11,5);
  }
  else
  {
  }

  for (int i = 0; i < MAXDRUMS; i++)
    for (int j = 0; j < MAXMOTORS; j++)
    {
      drums[i][j][0] = 1500;
      drums[i][j][1] = 1500;
    }
}

void hand::moveonemotor(int motor_index, int value)
{
  switch (motor_index)
  {
    case 0:
      yaostepper.settarget_abs(value);
      break;
    case 1:
      jianstepper.settarget_abs(value);
      break;
    case 2:
      zhouservo.settarget_abs(value);
      break;
    case 3:
      wanservo.settarget_abs(value);
      break;
    default:
      break;
  }

}

void hand::moveallmotors(int values[])
{
    yaostepper.settarget_abs(values[0]);
    jianstepper.settarget_abs(values[1]);
    zhouservo.settarget_abs(values[2]);
    wanservo.settarget_abs(values[3]);
}

void hand::setdrumbyvalue(int drum_index, int motor_index, int updown, int value)
{
  drums[drum_index][motor_index][updown] = value;
}

void hand::setdrumbypos(int drum_index,int updown)
{
  drums[drum_index][0][updown] = yaostepper.getpos();
  drums[drum_index][1][updown] = jianstepper.getpos();
  drums[drum_index][2][updown] = zhouservo.getpos();
  drums[drum_index][3][updown] = wanservo.getpos();
  
}

void hand::movemotorstodrum(int drum_index, int updown)
{
  yaostepper.settarget_abs(drums[drum_index][0][updown]);
  jianstepper.settarget_abs(drums[drum_index][1][updown]);
  zhouservo.settarget_abs(drums[drum_index][2][updown]);
  wanservo.settarget_abs(drums[drum_index][3][updown]);
}

bool hand::gotarget()
{
  bool unfinished = true;
  unfinished |= yaostepper.gotarget();
  unfinished |= jianstepper.gotarget();
  unfinished |= zhouservo.gotarget();
  unfinished |= wanservo.gotarget();
  return unfinished;
  
}

void hand::load(byte *thebuf)
{
  memcpy((byte*)&drums[0], thebuf, 128);
}

void hand::store(byte *thebuf)
{
  memcpy(thebuf, (byte*)&drums[0], 128);
}
