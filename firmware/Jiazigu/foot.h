#ifndef foot_h
#define foot_h
#include "Arduino.h"

#include "valleyservo.h"


class foot {
  public:
    // constructors:
    foot();
    void Init(int lefgright);
    
    //motor related functions
    int moveonemotor(int value);
    int getmotor();
    
    //drum related functions
    int setdrumbyvalue(int updown,int value);
    int setdrumbypos(int updown);
    int getdrum(int updown, int values[]);
    int movemotorstodrum(int updown);
    
    //playing function
    bool gotarget();
    
    //eeprom related function
    void load(byte *thebuf);
    void store(byte *thebuf);
    
  private:
    valleyservo theservo;

    //drum
    int drums[2];
    
};

#endif
