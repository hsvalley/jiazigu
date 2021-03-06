#ifndef valleyservo_h
#define valleyservo_h
#include <Servo.h>

#define INIT_POS 150
#define SERVO_STATE_NULL 0
#define SERVO_STATE_BEFORE_MID 1
#define SERVO_STATE_MID 2
#define SERVO_STATE_BEFORE_TGT 3

#define STOP_IN_MIDDLE 20

// library interface description
class valleyservo {
  public:
    // constructors:
    valleyservo();
    void Init(int pin,int mindelay);
    bool gotarget();
    int settarget_abs(unsigned long finishedtime,int mid_pos, int tgt_pos);
    int getpos();
    
  private:
    int step_delay;      // the minimam tolarent delay
    Servo thisservo;
    
    int cur_pos;
    int target_pos;
    int middle_pos;
    
    unsigned long last_step_time; // time stamp in us of when the last step was taken
    int state; //0, none, 1, before 
};

#endif

