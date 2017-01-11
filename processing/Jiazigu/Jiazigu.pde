// Need G4P library
import g4p_controls.*;

import controlP5.*;
import processing.serial.*;


int connectted_color=color(128,128,0);
int disconnectted_color=color(16,0,0);

/*********MyScope********************/
String g_sendData="";
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
Textarea serialErrorArea;
//GDropList dropList_serial,dropList_baudRate,dropList_drums;
Slider servoSlider0,servoSlider1,servoSlider2,servoSlider3;

Textlabel myStateTxt,myPeerStateTxt;

void createNotes_serial() {
 serialErrorArea = cp5.addTextarea("serialError")
    .setPosition(210, 28)
    .setSize(600, 320)
    .setText("\n\n你确定你的板子连接上了电脑吗？\n\n请检查！") 
    .setColor(0xa8ff0000)
    .setColorBackground(color(255, 255, 255))
    .setFont(createFont("SimHei", 28));
}

public void setup() {
  size(500, 300, JAVA2D);
//  background(28, 170, 234);

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

    
/***********************************************************/     
//controler for drums page
  servoSlider0 = cp5.addSlider("servo0")
     .setBroadcast(false)
     .setRange(500,2500)
     .setValue(1500)
     .setPosition(10,20)
     .setSize(200,20)
     .setBroadcast(true)
     ;
  servoSlider1 = cp5.addSlider("servo1")
     .setBroadcast(false)
     .setRange(500,2500)
     .setValue(1500)
     .setPosition(10,50)
     .setSize(200,20)
     .setBroadcast(true)
     ;
     
  servoSlider2 = cp5.addSlider("servo2")
     .setBroadcast(false)
     .setRange(500,2500)
     .setValue(1500)
     .setPosition(10,80)
     .setSize(200,20)
     .setBroadcast(true)
     ;
     
  servoSlider3 = cp5.addSlider("servo3")
     .setBroadcast(false)
     .setRange(500,2500)
     .setValue(1500)
     .setPosition(10,110)
     .setSize(200,20)
     .setBroadcast(true)
     ;

  cp5.addScrollableList("alldrums_list")
     .setPosition(300, 30)
     .setSize(80, 180)
     .setBarHeight(20)
     .setItemHeight(20)
     .addItems(loadStrings("drumlist"))
     .setType(ScrollableList.DROPDOWN) // currently supported DROPDOWN and LIST
     ;   
   
     
  // arrange controller in separate tabs
  cp5.getController("servo0").moveTo("drums");
  cp5.getController("servo1").moveTo("drums");
  cp5.getController("servo2").moveTo("drums");
  cp5.getController("servo3").moveTo("drums");
  cp5.getController("alldrums_list").moveTo("drums");
  
/***********************************************************/     
//controler for song page
  cp5.addTextfield("txtDelay1")
     .setPosition(10,20)
     .setSize(100,20)
     .setFont(createFont("arial",16))
     .setAutoClear(false)
     ;
   cp5.addTextfield("txtDelay2")
     .setPosition(10,60)
     .setSize(100,20)
     .setFont(createFont("arial",16))
     .setAutoClear(false)
     ; 
    cp5.addTextfield("txtrepeat")
     .setPosition(10,100)
     .setSize(100,20)
     .setFont(createFont("arial",16))
     .setAutoClear(false)
     ;       
     
    cp5.addTextfield("txtsong")
     .setPosition(10,170)
     .setSize(400,20)
     .setFont(createFont("arial",16))
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
                    .setPosition(10,280)
                    .setText("Current state")
                    .setColorValue(0xffffff00)
                    .setFont(createFont("Georgia",16));
                    
myPeerStateTxt = cp5.addTextlabel("peerstate")
                    .setPosition(200,280)
                    .setText("Current peer state")
                    .setColorValue(0xffffff00)
                    .setFont(createFont("Georgia",16))                   
                    ;
                    
  cp5.getController("state").moveTo("global");
  cp5.getController("peerstate").moveTo("global");
/********Data handle***************************************************/     
 
  myDrums = new MyAllDrums();
  myDrums.init();
  
}

public void draw() {
  background(isConnected?disconnectted_color:connectted_color);
  fill(color(128,64,64));
  rect(0,0,width,18);
}


//event handle
void controlEvent(ControlEvent theControlEvent) {
  if (theControlEvent.isTab()) {
    println("got an event from tab : "+theControlEvent.getTab().getName()+" with id "+theControlEvent.getTab().getId());
  }
}

void serialport_list(int pos) {
  serialIndex = pos;
}
void baudrate_list(int pos) {
  baudrateIndex = pos;
}

void alldrums_list(int pos) {
//lode the servo values
//check if pos is valid
  int value;

  if(pos<0) return;

  currentPos = pos;
  
  value = myDrums.getDrumPos(pos,0);
  servoSlider0.setValue(value);
//  setRealServo(0,value);
  
  value = myDrums.getDrumPos(pos,1);
  servoSlider1.setValue(value);
//  setRealServo(1,value);
  
  value = myDrums.getDrumPos(pos,2);
  servoSlider2.setValue(value);
//  setRealServo(2,value);
  
  value = myDrums.getDrumPos(pos,3);
  servoSlider3.setValue(value);
//  setRealServo(3,value);
}

void servo0(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    setRealServo(0,newvalue);
  }
  
  
  if(currentPos<0) 
  {
    return;
  }
  
  myDrums.setDrumPos(currentPos,0,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,0,newvalue);
  
}


void servo1(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    setRealServo(1,newvalue);
  }
  
  if(currentPos<0) 
  {
    return;
  }
  myDrums.setDrumPos(currentPos,1,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,1,newvalue);

}


void servo2(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
    setRealServo(2,newvalue);
  }
  
  if(currentPos<0) 
  {
    return;
  }
  myDrums.setDrumPos(currentPos,2,newvalue);
  if(isConnected) setDrum(currentPos/2,currentPos-(currentPos/2)*2,2,newvalue);

}


void servo3(int newvalue)
{
  if(isConnected == false)
  {
    myStateTxt.setText("Serial port is unconnected!");
  }
  else
  {
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
public void connect(int theValue) {
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
  }
  
  myStateTxt.setText("Serial port connected!");
  
}

public void runsong()
{
  String str = new String();
  int data;

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
  setDelay(0, data);
  
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
  setDelay(1,data);
  
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
  setRepeat(data);
  
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
      setSong(i,thechar);
    }
  }
  if(0==validtone)
  {
    myStateTxt.setText("No valid tone!");
  }
  else
  {
    startRun(validtone);
  }
  
}

void serialEvent(Serial myPort) {
  int head=0;
  String g_inString = new String(); //<>//
  
  try {
    if(myPort.available() > 0) 
    {
        g_inString = myPort.readStringUntil('\n');
    }
    myPeerStateTxt.setText(g_inString);
  }
  catch (Exception e) {
    myStateTxt.setText("catch a Error ");
  }
}