//ASCII-BASED protocol definition
  
final static char CMD_START_CHAR = '<';
final static char CMD_END_CHAR = '>';
final static char CMD_SEPARE_CHAR = ',';

final static int CMD_WORD_SETSERVO = 'A';
final static int CMD_WORD_GETSERVO = 'B';
final static int CMD_WORD_SETDRUM  = 'C';
final static int CMD_WORD_GETDRUM  = 'D';
final static int CMD_WORD_SETSONG  = 'E';
final static int CMD_WORD_GETSONG  = 'F';
final static int CMD_WORD_RUN      = 'G';
final static int CMD_WORD_STOP     = 'H';
final static int CMD_WORD_SETDELAY = 'I';
final static int CMD_WORD_GETDELAY = 'J';
final static int CMD_WORD_SETREPEAT = 'K';
final static int CMD_WORD_GETREPEAT = 'L';
final static int CMD_WORD_HEARTBEAT = 'M';

void writeSerialData(int data)
{
  int weishu=0;
  int buffer[] = new int[10];
  int temp,temp1;
  temp = data;
  
  while(temp>0)
  {
    temp1 = temp/10;
    buffer[weishu++] = temp-temp1*10;
    temp = temp1;
  }
  
  if(weishu==0) weishu=1;
  
  for(int i=1;i<=weishu;i++)
  {
    myPort.write('0' + buffer[weishu - i]);
  }
}

void setRealServo(int servo_index,int value)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_SETSERVO);
  writeSerialData(servo_index);
  myPort.write(CMD_SEPARE_CHAR);
  writeSerialData(value);
  myPort.write(CMD_END_CHAR);
}

void setDrum(int drum_id, int updown, int servo_id,int value)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_SETDRUM);
  writeSerialData(drum_id);  
  myPort.write(CMD_SEPARE_CHAR);
  writeSerialData(updown); //0,up,1,down
  myPort.write(CMD_SEPARE_CHAR);
  writeSerialData(servo_id);  
  myPort.write(CMD_SEPARE_CHAR);
  writeSerialData(value);
  myPort.write(CMD_END_CHAR);  
}

void setDelay(int index, int value)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_SETDELAY);
  writeSerialData(index);
  myPort.write(CMD_SEPARE_CHAR);
  writeSerialData(value);
  myPort.write(CMD_END_CHAR);
}

void setRepeat(int value)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_SETREPEAT);
  writeSerialData(value);
  myPort.write(CMD_END_CHAR);
}

void setSong(int index, char drum_id)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_SETSONG);
  writeSerialData(index);
  myPort.write(CMD_SEPARE_CHAR);
  myPort.write(drum_id);
  myPort.write(CMD_END_CHAR);
}

void startRun(int count)
{
  myPort.write(CMD_START_CHAR);
  myPort.write(CMD_WORD_RUN);
  writeSerialData(count);  
  myPort.write(CMD_END_CHAR);
}