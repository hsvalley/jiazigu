final int NumOfServos = 4; // Constant, cannot be modified
final int DefaultServoValue = 1500; // Constant, cannot be modified
  

public class oneDrum {
  

  private int servo_up[] = new int[NumOfServos];
  private int servo_down[] = new int[NumOfServos];
  
  
  void oneDrum() {

  }

 void init()
 {
    for(int i=0;i<NumOfServos;i++){
      servo_up[i] = DefaultServoValue;
      servo_down[i] = DefaultServoValue;
    } 
 }
 
 public void setUp(int index,int value)
 {
   servo_up[index]=value;
 }

 public void setDown(int index,int value)
 {
   servo_down[index]=value;
 }
 
public int getUp(int index)
{
  return servo_up[index];
}

public int getDown(int index)
{
  return servo_down[index];
}
  
}