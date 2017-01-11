#ifndef valleyservo_h
#define valleyservo_h
#include <Servo.h>

// library interface description
class valleyservo {
  public:
    // constructors:
    valleyservo();
    void Init(int pin,int mindelay);
    bool gotarget();
    void settarget_relative(int steps);
    void settarget_abs(int steps);
    int getpos();
    
  private:
    int min_delay;      // the minimam tolarent delay
    Servo thisservo;
    
    //state,0:init;1,qualified;2,running
    int state;  

    int cur_pos;
    int target_pos;
    
    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

