#include <Servo.h>

#include "foot.h"

foot::foot()
{

}

void foot::Init(int leftright)
{
  if (0 == leftright)
  {
    theservo.Init(12, 5);
  }
  else
  {
     theservo.Init(13, 5);
  }

  drums[0] = INIT_POS;
  drums[1] = INIT_POS;
}

int foot::moveonemotor(int value)
{
//  theservo.settarget_abs(value);
  return 0;
}

int foot::getmotor()
{
  return theservo.getpos();
}

int foot::setdrumbyvalue(int updown, int value)
{

  if (updown > 1) return -1;
  if ((value < 1000) || (value > 2000)) return -1;
  drums[updown] = value;
  return 0;
}

int foot::setdrumbypos(int updown)
{
  if (updown > 1) return -1;

  drums[updown] = theservo.getpos();
  return 0;
}

int foot::movemotorstodrum(int updown)
{
  if (updown > 1) return -1;

//  theservo.settarget_abs(drums[updown]);
  return 0;
}

int foot::getdrum(int updown, int values[])
{
  if (updown > 1) return -1;
  values[0] = drums[updown];
  return 0;
}

bool foot::gotarget()
{
  return theservo.gotarget();; 
}

