final int NumOfDrums = 5; // Constant, cannot be modified
 
public class MyAllDrums {
  
private oneDrum myDrums[]=new oneDrum[NumOfDrums];
//private oneDrum myDrums[]; //=new oneDrum[NumOfDrums];

  void MyAllDrums() {
//nothing to do
  }

  public void init()
  {
    for(int i=0;i<NumOfDrums;i++)
    {
       myDrums[i] = new oneDrum();
       myDrums[i].init();
    }
  }
  
  public void setDrumPos(int drum_index,int servo_index, int value)
  {
    int real_index = drum_index/2;
    if((drum_index - (drum_index/2)*2) == 0)
    {
      //up
      myDrums[real_index].setUp(servo_index,value);
    }
    else
    {
      //down
      myDrums[real_index].setDown(servo_index,value);
   }    
  }
   
  public int getDrumPos(int drum_index,int servo_index)
  {
    int real_index = drum_index/2;
    if((drum_index - (drum_index/2)*2) == 0)
    {
      //up
      return myDrums[real_index].getUp(servo_index);
    }
    else
    {
      //down
      return myDrums[real_index].getDown(servo_index);
    }    
  } 
  
}