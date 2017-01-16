// Need G4P library
import g4p_controls.*;

import controlP5.*;
import processing.serial.*;
char sendbuffer[] = new char[256];
int send_len=0;
char receivebuffer[] = new char[256];
int receive_len = 0;
boolean start_received = false;
int start_pos = 0;
boolean end_received = false;
int end_pos = 0;

int connectted_color=color(26,179,45);
int disconnectted_color=color(202,12,2);

int currentTab=0;

GWindow window;

/******************************/
Serial myPort;             // 定义串口
String serialList[]; 
int serialIndex = -1;
int baudrateIndex = -1;

boolean isConnected=false;

MyAllDrums myDrums;
int currentPos = -1;

/**********controlP5********************/
ControlP5 cp5;
//GDropList dropList_serial,dropList_baudRate,dropList_drums;
Slider lefthandservoSlider0,lefthandservoSlider1,lefthandservoSlider2,lefthandservoSlider3;

Textlabel myStateTxt;

public void setup() {
  size(480, 380, JAVA2D);

  cp5 = new ControlP5(this);

  cp5.addTab("drums")
     .setColorBackground(color(0, 160, 100))
     .setColorLabel(color(255))
     .setColorActive(color(255,128,0))
     ;
     
  // if you want to receive a controlEvent when
  // a  tab is clicked, use activeEvent(true)
  
  cp5.getTab("default")
     .activateEvent(true)
     .setLabel("Basic Setting")
     .setId(1)
     ;

  cp5.getTab("drums")
     .activateEvent(true)
     .setLabel("Drums Setting")
     .setId(2)
     ;
  cp5.getTab("song")
     .activateEvent(true)
     .setLabel("Song Setting")
     .setId(3)
     ;
  
  // create a few controllers

/***********************************************************/   
  //controllers for default page
  String serialList[] = Serial.list(); 
  cp5.addScrollableList("serialport_list")
     .setPosition(10, 30)
     .setSize(80, 180)
     .setBarHeight(20)
     .setItemHeight(20)
     .addItems(serialList)
     .setType(ScrollableList.DROPDOWN) // currently supported DROPDOWN and LIST
     ;    
          
  cp5.addScrollableList("baudrate_list")
     .setPosition(110, 30)
     .setSize(80, 180)
     .setBarHeight(20)
     .setItemHeight(20)
     .addItems(loadStrings("list_baudrate"))
     .setType(ScrollableList.DROPDOWN) // currently supported DROPDOWN and LIST
     ;   

  cp5.addButton("connect")
     .setBroadcast(false)
     .setPosition(210,30)
     .setSize(80,20)
     .setValue(1)
     .setBroadcast(true)
     .getCaptionLabel().align(CENTER,CENTER)
     ;
     
  cp5.getController("connect").moveTo("default");
  cp5.getController("serialport_list").moveTo("default");
  cp5.getController("baudrate_list").moveTo("default");
cp5.get(ScrollableList.class, "serialport_list").close();
cp5.get(ScrollableList.class, "baudrate_list").close();    
/***********************************************************/     
//controler for drums page
  lefthandservoSlider0 = cp5.addSlider("lefthandservo0")
     .setBroadcast(false)
     .setRange(1420,1580)
     .setValue(1500)
     .setPosition(100,20)
     .setSize(200,20)
     .setBroadcast(true)
     ;
  lefthandservoSlider1 = cp5.addSlider("lefthandservo1")
     .setBroadcast(false)
     .setRange(1000,2000)
     .setValue(1500)
     .setPosition(100,42)
     .setSize(200,20)
     .setBroadcast(true)
     ;
     
  lefthandservoSlider2 = cp5.addSlider("lefthandservo2")
     .setBroadcast(false)
     .setRange(1000,2000)
     .setValue(1500)
     .setPosition(100,64)
     .setSize(200,20)
     .setBroadcast(true)
     ;
     
  lefthandservoSlider3 = cp5.addSlider("lefthandservo3")
     .setBroadcast(false)
     .setRange(800,2200)
     .setValue(1500)
     .setPosition(100,86)
     .setSize(200,20)
     .setBroadcast(true)
     ;

  cp5.addScrollableList("lefthanddrums_list")
     .setPosition(10, 20)
     .setSize(80, 200)
     .setBarHeight(20)
     .setItemHeight(20)
     .addItems(loadStrings("drumlist"))
     .setType(ScrollableList.DROPDOWN) // currently supported DROPDOWN and LIST
     ;   
  cp5.addButton("LoadEEPROM")
     .setBroadcast(false)
     .setPosition(386,30)
     .setSize(80,20)
     .setValue(1)
     .setBroadcast(true)
     .getCaptionLabel().align(CENTER,CENTER)
     ;
    cp5.addButton("StoreEEPROM")
     .setBroadcast(false)
     .setPosition(386,80)
     .setSize(80,20)
     .setValue(1)
     .setBroadcast(true)
     .getCaptionLabel().align(CENTER,CENTER)
     ;   
  // arrange controller in separate tabs
  cp5.getController("lefthandservo0").moveTo("drums");
  cp5.getController("lefthandservo1").moveTo("drums");
  cp5.getController("lefthandservo2").moveTo("drums");
  cp5.getController("lefthandservo3").moveTo("drums");
  cp5.getController("lefthanddrums_list").moveTo("drums");
  cp5.getController("LoadEEPROM").moveTo("drums");
  cp5.getController("StoreEEPROM").moveTo("drums");
cp5.get(ScrollableList.class, "lefthanddrums_list").close();  
/***********************************************************/     
//controler for song page
  cp5.addTextfield("txtDelay1")
     .setPosition(10,20)
     .setSize(100,20)
     .setFont(createFont("arial",12))
     .setAutoClear(false)
     ;
   cp5.addTextfield("txtDelay2")
     .setPosition(10,60)
     .setSize(100,20)
     .setFont(createFont("arial",12))
     .setAutoClear(false)
     ; 
    cp5.addTextfield("txtrepeat")
     .setPosition(10,100)
     .setSize(100,20)
     .setFont(createFont("arial",12))
     .setAutoClear(false)
     ;       
     
    cp5.addTextfield("txtsong")
     .setPosition(10,170)
     .setSize(400,20)
     .setFont(createFont("arial",12))
     .setAutoClear(false)
     ;       
          
  cp5.addBang("runsong")
     .setPosition(200,50)
     .setSize(100,100)
     .getCaptionLabel().align(ControlP5.CENTER, ControlP5.CENTER)
     ; 
 
     
  cp5.getController("txtDelay1").moveTo("song");
  cp5.getController("txtDelay2").moveTo("song");
  cp5.getController("txtrepeat").moveTo("song");
  cp5.getController("runsong").moveTo("song");
  cp5.getController("txtsong").moveTo("song");

/*******Common controller***************************************************/     
myStateTxt = cp5.addTextlabel("state")
                    .setPosition(200,3)
                    .setText("Current state")
                    .setColorValue(0xffffff00)
                    .setFont(createFont("Georgia",12));
                    
  cp5.getController("state").moveTo("global");
/********Data handle***************************************************/     
 
  myDrums = new MyAllDrums();
  myDrums.init();
  
}

public void draw() {
  
  if(isConnected)  
    fill(connectted_color);
  else
    fill(disconnectted_color);
  rect(0,0,width,18);
  
   if(2==currentTab)
  {
    //default
    fill(color(9,152,193));
    rect(0,20,width-100,90);
    fill(color(122,221,250));
    rect(0,110,width-100,90); 
    fill(color(9,152,193));
    rect(0,200,width-100,30); 
    fill(color(122,221,250));
    rect(0,230,width-100,30); 
  }
  else 
  {
    //song
    fill(color(185,192,249));
    rect(0,18,width,height-18);  
  }
  
  
}


//event handle
void controlEvent(ControlEvent theControlEvent) {
  if (theControlEvent.isTab()) {
    println("got an event from tab : "+theControlEvent.getTab().getName()+" with id "+theControlEvent.getTab().getId());
    currentTab=theControlEvent.getTab().getId();
  }
}

void serialport_list(int pos) {
  serialIndex = pos;
}
void baudrate_list(int pos) {
  baudrateIndex = pos;
}

void lefthanddrums_list(int pos) {
//lode the servo values
//check if pos is valid
  int value;

  if(pos<0) return;

  currentPos = pos;
  
  value = myDrums.getDrumPos(pos,0);
  lefthandservoSlider0.setValue(value);
//  setRealServo(0,value);
  
  value = myDrums.getDrumPos(pos,1);
  lefthandservoSlider1.setValue(value);
//  setRealServo(1,value);
  
  value = myDrums.getDrumPos(pos,2);
  lefthandservoSlider2.setValue(value);
//  setRealServo(2,value);
  
  value = myDrums.getDrumPos(pos,3);
  lefthandservoSlider3.setValue(value);
//  setRealServo(3,value);

}

void lefthandservo0(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    myStateTxt.setText("Connectted");
    setRealServo(0,newvalue);
  }
   if(currentPos<0) 
  {
    return;
  }
  
  myDrums.setDrumPos(currentPos,0,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,0,newvalue);
  
}


void lefthandservo1(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    myStateTxt.setText("Connectted");
    setRealServo(1,newvalue);
  }
  
  if(currentPos<0) 
  {
    return;
  }
  myDrums.setDrumPos(currentPos,1,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,1,newvalue);

}


void lefthandservo2(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
        myStateTxt.setText("Connectted");
    setRealServo(2,newvalue);
  }
  
  if(currentPos<0) 
  {
    return;
  }
  myDrums.setDrumPos(currentPos,2,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,2,newvalue);

}


void lefthandservo3(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    myStateTxt.setText("Connectted");
    setRealServo(3,newvalue);
  }
  
  if(currentPos<0) 
  {
    return;
  }
  myDrums.setDrumPos(currentPos,3,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,3,newvalue);

}

void keyPressed() {
  if(keyCode==TAB) {
    cp5.getTab("drums").bringToFront();
  }
}

//buttons handle
public void connect() {
int br[]={9600,14400,19200,28800,38400,57600,115200};

  if(serialIndex<0 | baudrateIndex<0)
  {
    myStateTxt.setText("please select an serial port");
    return;
  }
  
  if(isConnected){
    //try to connect the serial port
    if (myPort!=null) {
      myPort.clear();
      myPort.stop();
    }
    isConnected = false;
    cp5.getController("connect").setCaptionLabel("Connect");
    myStateTxt.setText("Serial port unconnected!");
  }else{
    try {
    myPort = new Serial(this,Serial.list()[serialIndex] ,br[baudrateIndex]);
    }
    catch (Exception e) {
    myStateTxt.setText("Error happen when opening Serial port!");
    return;
  }
    isConnected = true;
    cp5.getController("connect").setCaptionLabel("Disconnect");
    myStateTxt.setText("Serial port connected!");
  }
  
}

public void runsong()
{
  String str = new String();
  int data;
  int ret;
  
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
    return;
  }

  str = cp5.get(Textfield.class,"txtDelay1").getText();
  if(str.length() <= 0 )  {
    myStateTxt.setText("error delay1!");
    return;
  }
  data = Integer.parseInt(str);
  if((data<5) || (data>2000))
  {
    myStateTxt.setText("error delay1!");
    return;
  }
  ret = setConfig(0, data);
  if (ret !=0) return;
  
  str = cp5.get(Textfield.class,"txtDelay2").getText();
  if(str.length() <= 0 )  {
    myStateTxt.setText("error delay2!");
    return;
  }  
  data = Integer.parseInt(str);
  if((data<5) || (data>2000))
  {
    myStateTxt.setText("error delay2!");
    return;
  }
  setConfig(1,data);
  
  str = cp5.get(Textfield.class,"txtrepeat").getText();
  if(str.length() <= 0 )  {
    myStateTxt.setText("error repeat!");
    return;
  }  
  data = Integer.parseInt(str);
  if((data<1) || (data>100))
  {
    myStateTxt.setText("error repeat!");
    return;
  }
  ret = setConfig(2,data);
  if (ret !=0) return;
    
  int validtone=0;
  str = cp5.get(Textfield.class,"txtsong").getText();
  for(int i=0;i<str.length();i++)
  {
    char thechar;
    thechar = str.charAt(i);
    if((thechar>='0') && (thechar<='9')) 
    {
      //valid char
      validtone++;
      ret = setSong(i,thechar);
      if (ret !=0) return;
    }
  }
  if(0==validtone)
  {
    myStateTxt.setText("No valid tone!");
  }
  else
  {
    ret = startRun(validtone);
    if (ret !=0) return;
  }
  
}

public void LoadEEPROM() {
  int newvalues[] = new int[4];
  
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
    return;
  }
  loadTheEEPROM();
  
  for(int drum_id=0;drum_id<NumOfDrums;drum_id++)
  {
    if(0==getDrum(drum_id,0,newvalues))
    {//up
      for(int i=0;i<4;i++) myDrums.setDrumPos(drum_id*2,i,newvalues[i]);
    } 
    if(0==getDrum(drum_id,1,newvalues))
    {//down
      for(int i=0;i<4;i++) myDrums.setDrumPos(drum_id*2+1,i,newvalues[i]);
    } 
  }
  
  if(currentPos>=0)
  {
     lefthandservoSlider0.setValue(myDrums.getDrumPos(currentPos,0));
     lefthandservoSlider1.setValue(myDrums.getDrumPos(currentPos,1));
     lefthandservoSlider2.setValue(myDrums.getDrumPos(currentPos,2));
     lefthandservoSlider3.setValue(myDrums.getDrumPos(currentPos,3));
  }

  
}

public void StoreEEPROM() {
  storeTheEEPROM();
}


void serialEvent(Serial myPort) {
char inByte;
 //<>//
  try {
    while(myPort.available() > 0) 
    {
        inByte = (char)myPort.read();
        print(inByte);
        receivebuffer[receive_len]=inByte;
        if(CMD_START_CHAR == inByte) 
        {
            start_received = true;
            start_pos = receive_len;
        }
        if(CMD_END_CHAR == inByte) 
        {
          end_received = true;
          end_pos = receive_len;
        }
        if(receive_len < 255) receive_len++;
    }
  }
  catch (Exception e) {
    myStateTxt.setText("catch a Serial port Error ");
  }
}