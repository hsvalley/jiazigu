#ifndef hand_h
#define hand_h
#include "Arduino.h"

#include "valleystepper.h"
#include "valleyservo.h"

#define MAXDRUMS 8
#define MAXMOTORS 4

class hand {
  public:
    // constructors:
    hand();
    void Init(int lefgright);
    
    //motor related functions
    int moveonemotor(int motor_index, int value);
    int moveallmotors(int values[]);
    int getmotor(int motor_index);
    
    //drum related functions
    int setdrumbyvalue(int drum_index,int updown, int motor_index,int value);
    int setdrumbypos(int drum_index,int updown);
    int getdrum(int drum_index, int updown, int values[]);
    int movemotorstodrum(int drum_index,int updown);
    
    //playing function
    bool gotarget();
    
    //eeprom related function
    void load(byte *thebuf);
    void store(byte *thebuf);
    
  private:
    valleystepper yaostepper;
    valleyservo jianservo;
    valleyservo zhouservo;  
    valleyservo wanservo;  

    //drum
    int drums[MAXDRUMS][2][MAXMOTORS];
    
};

#endif
