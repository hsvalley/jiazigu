#ifndef hand_h
#define hand_h
#include "Arduino.h"

#include "valleyservo.h"

#define MAXDRUMS 8
#define MAXMOTORS 4
#define YAO 0
#define JIAN 1
#define ZHOU 2
#define WAN 3

#define UP 0
#define DOWN 1


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
    int getdrum(int drum_index, int updown, int values[]);
    int movemotorstodrum(int drum_index,unsigned long finishedtime);
    
    //playing function
    bool gotarget();
    
    //eeprom related function
    void load(byte *thebuf);
    void store(byte *thebuf);
    
  private:
//    valleystepper yaostepper;
    valleyservo yaoservo;
    valleyservo jianservo;
    valleyservo zhouservo;  
    valleyservo wanservo;  

    //drum
    int drums[MAXDRUMS][MAXMOTORS];
    int daji_value[MAXMOTORS];
    
};

#endif
