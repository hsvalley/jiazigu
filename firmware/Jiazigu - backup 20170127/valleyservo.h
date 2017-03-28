#ifndef valleyservo_h
#define valleyservo_h
#include <Servo.h>

#define INIT_POS 150

// library interface description
class valleyservo {
  public:
    // constructors:
    valleyservo();
    void Init(int pin,int mindelay);
    bool gotarget();
    int settarget_abs(int steps);
    int getpos();
    
  private:
    int min_delay;      // the minimam tolarent delay
    Servo thisservo;
    
    int cur_pos;
    int target_pos;
    
    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

