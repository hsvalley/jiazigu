#include <Servo.h>

#include "hand.h"

hand::hand()
{

}

void hand::Init(int leftright)
{
  if (0 == leftright)
  {
    yaoservo.Init(5, 1);
    jianservo.Init(6, 1);
    zhouservo.Init(7, 1);
    wanservo.Init(8, 1);
  }
  else
  {
    yaoservo.Init(12, 5);
    jianservo.Init(9, 5);
    zhouservo.Init(10, 5);
    wanservo.Init(11, 5);
  }

  for (int i = 0; i < MAXDRUMS; i++) for (int k = 0; k < 4; k++) drums[i][k] = INIT_POS;
  daji_value[YAO] = 0;
  daji_value[JIAN]  = 25;
  daji_value[ZHOU]  = -30;
  daji_value[WAN]  = -30;
}

int hand::moveonemotor(int motor_index, int value)
{
  switch (motor_index)
  {
    case 0:
      return yaoservo.settarget_abs(millis() + 500,value,value);
      break;
    case 1:
      return jianservo.settarget_abs(millis() + 500,value,value);
      break;
    case 2:
      return zhouservo.settarget_abs(millis() + 500,value,value);
      break;
    case 3:
      return wanservo.settarget_abs(millis() + 500,value,value);
      break;
    default:
      return -2;
      break;
  }

}

int hand::moveallmotors(int values[])
{
  char ret;

  ret = yaoservo.settarget_abs(millis() + 500,values[0],values[0]); if (ret != 0) return ret;
  ret = jianservo.settarget_abs(millis() + 500,values[1],values[1]); if (ret != 0) return ret;
  ret = zhouservo.settarget_abs(millis() + 500,values[2],values[2]); if (ret != 0) return ret;
  ret = wanservo.settarget_abs(millis() + 500,values[3],values[3]); return ret;
}

int hand::getmotor(int motor_index)
{
  switch (motor_index)
  {
    case 0:
      return yaoservo.getpos();
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

  if ((value < 80) || (value > 220)) return -1;

  if (updown == UP)
  {
    drums[drum_index][motor_index] = value;
  }
  else
  {
    drums[drum_index][motor_index] = value + daji_value[motor_index];
  }
  return 0;
}

int hand::movemotorstodrum(int drum_index,unsigned long finishedtime)
{
  
  if (drum_index > 7) return -1;

  yaoservo.settarget_abs(finishedtime,drums[drum_index][YAO],drums[drum_index][YAO]);
  jianservo.settarget_abs(finishedtime,drums[drum_index][JIAN],drums[drum_index][JIAN] - daji_value[JIAN]);
  zhouservo.settarget_abs(finishedtime,drums[drum_index][ZHOU],drums[drum_index][ZHOU] - daji_value[ZHOU]);
  wanservo.settarget_abs(finishedtime,drums[drum_index][WAN],drums[drum_index][WAN] - daji_value[WAN]);

  return 0;
}

int hand::getdrum(int drum_index, int updown, int values[])
{
  if (drum_index > 7) return -1;
  if (updown > 1) return -1;


  if (UP == updown)
  {
    values[YAO] = drums[drum_index][YAO];
    values[JIAN] = drums[drum_index][JIAN];
    values[ZHOU] = drums[drum_index][ZHOU];
    values[WAN] = drums[drum_index][WAN];
  }
  else
  {
    values[YAO] = drums[drum_index][YAO] - daji_value[YAO];
    values[JIAN] = drums[drum_index][JIAN] - daji_value[JIAN];
    values[ZHOU] = drums[drum_index][ZHOU] - daji_value[ZHOU];
    values[WAN] = drums[drum_index][WAN] - daji_value[WAN];
  }
  yaoservo.settarget_abs(millis() + 500,values[YAO],values[YAO]);
  jianservo.settarget_abs(millis() + 500,values[JIAN],values[JIAN]);
  zhouservo.settarget_abs(millis() + 500,values[ZHOU],values[ZHOU]);
  wanservo.settarget_abs(millis() + 500,values[WAN],values[WAN]);
  
  return 0;
}

bool hand::gotarget()
{
  bool isrunning = false;
  isrunning |= yaoservo.gotarget();
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
