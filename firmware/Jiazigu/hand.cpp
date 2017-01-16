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
    jianservo.Init(6, 5);
    zhouservo.Init(7, 5);
    wanservo.Init(8, 5);
  }
  else
  {
  }

  for (int i = 0; i < MAXDRUMS; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 4; k++)
        drums[i][j][k] = INIT_POS;
}

int hand::moveonemotor(int motor_index, int value)
{
  switch (motor_index)
  {
    case 0:
      return yaostepper.settarget_abs(value);
      break;
    case 1:
      return jianservo.settarget_abs(value);
      break;
    case 2:
      return zhouservo.settarget_abs(value);
      break;
    case 3:
      return wanservo.settarget_abs(value);
      break;
    default:
      return -2;
      break;
  }

}

int hand::moveallmotors(int values[])
{
  char ret;

  ret = yaostepper.settarget_abs(values[0]);
  if (ret != 0) return ret;

  ret = jianservo.settarget_abs(values[1]); if (ret != 0) return ret;
  ret = zhouservo.settarget_abs(values[2]); if (ret != 0) return ret;
  ret = wanservo.settarget_abs(values[3]); return ret;
}

int hand::getmotor(int motor_index)
{
  switch (motor_index)
  {
    case 0:
      return yaostepper.getpos();
      break;
    case 1:
      return jianservo.getpos();
      break;
    case 2:
      return zhouservo.getpos();
      break;
    case 3:
      return wanservo.getpos();
      break;
    default:
      return -1;
  }
}

int hand::setdrumbyvalue(int drum_index, int updown, int motor_index, int value)
{
  if (drum_index > 7) return -1;
  if (motor_index > 4) return -1;
  if (updown > 1) return -1;

  if (motor_index == 0)
  {
    if ((value < 1400) || (value > 1600)) return -1;
  }
  else
  {
    if ((value < 800) || (value > 2200)) return -1;
  }

  drums[drum_index][updown][motor_index] = value;
  return 0;
}

int hand::setdrumbypos(int drum_index, int updown)
{
  if (drum_index > 7) return -1;
  if (updown > 1) return -1;

  drums[drum_index][updown][0] = yaostepper.getpos();
  drums[drum_index][updown][1] = jianservo.getpos();
  drums[drum_index][updown][2] = zhouservo.getpos();
  drums[drum_index][updown][3] = wanservo.getpos();
  return 0;
}

int hand::movemotorstodrum(int drum_index, int updown)
{
  if (drum_index > 7) return -1;
  if (updown > 1) return -1;

  yaostepper.settarget_abs(drums[drum_index][updown][0]);
  jianservo.settarget_abs(drums[drum_index][updown][1]);
  zhouservo.settarget_abs(drums[drum_index][updown][2]);
  wanservo.settarget_abs(drums[drum_index][updown][3]);
  return 0;
}

int hand::getdrum(int drum_index, int updown, int values[])
{
  if (drum_index > 7) return -1;
  if (updown > 1) return -1;

  values[0] = drums[drum_index][updown][0];
  values[1] = drums[drum_index][updown][1];
  values[2] = drums[drum_index][updown][2];
  values[3] = drums[drum_index][updown][3];
  return 0;
}

bool hand::gotarget()
{
  bool isrunning = false;
  isrunning |= yaostepper.gotarget();
  isrunning |= jianservo.gotarget();
  isrunning |= zhouservo.gotarget();
  isrunning |= wanservo.gotarget();
  return isrunning; //return true if any of motor is running

}

void hand::load(byte *thebuf)
{
  memcpy((byte*)&drums[0], thebuf, 128);
}

void hand::store(byte *thebuf)
{
  memcpy(thebuf, (byte*)&drums[0], 128);
}
