#ifndef valleystepper_h
#define valleystepper_h

#define INIT_POS 1500

// library interface description
class valleystepper {
  public:
    // constructors:
    valleystepper();
    void Init(int pin_1, int pin_2,int mindelay);
    
    // mover method:

    bool gotarget();
    int settarget_abs(int steps);
    int getpos();
    
  private:
    void stepMotor(int this_step);
    int goonestep();
    int backonestep();
    
    int min_delay;      // the minimam tolarent delay

    // motor pin numbers:
    int motor_pin_1;
    int motor_pin_2;

    int cur_pos;
    int target_pos;
    
    unsigned long last_step_time; // time stamp in us of when the last step was taken
};

#endif

