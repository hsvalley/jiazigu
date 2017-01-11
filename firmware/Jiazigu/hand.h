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
    void moveonemotor(int motor_index, int value);
    void moveallmotors(int values[]);
    
    //drum related functions
    void setdrumbyvalue(int drum_index,int motor_index,int updown,int value);
    void setdrumbypos(int drum_index,int updown);
    int getdrum(int drum_index,int motor_index, int updown);
    void movemotorstodrum(int drum_index,int updown);
    
    //playing function
    bool gotarget();
    
    //eeprom related function
    void load(byte *thebuf);
    void store(byte *thebuf);
    
  private:
    valleystepper yaostepper;
    valleystepper jianstepper;
    valleyservo zhouservo;  
    valleyservo wanservo;  

    //drum
    int drums[MAXDRUMS][MAXMOTORS][2];
    
};

#endif
