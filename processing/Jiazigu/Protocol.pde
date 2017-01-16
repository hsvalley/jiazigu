//ASCII-BASED protocol definition
  
final static char CMD_START_CHAR = '<';
final static char CMD_END_CHAR = '>';
final static char CMD_SEPARE_CHAR = ',';

final static char  RSP_OK = '0';
final static char  RSP_ERROR_LENGTH = '1';
final static char  RSP_ERROR_CHECKSUM = '2';
final static char  RSP_ERROR_BUSY = '3';
final static char  RSP_ERROR_UNKNOWN = '4';

final static char CMD_WORD_SETSERVO = 'A';
final static char CMD_WORD_GETSERVO = 'B';
final static char CMD_WORD_SETDRUM  = 'C';
final static char CMD_WORD_GETDRUM  = 'D';
final static char CMD_WORD_SETSONG  = 'E';
final static char CMD_WORD_GETSONG  = 'F';
final static char CMD_WORD_RUN      = 'G';
final static char CMD_WORD_STOP     = 'H';
final static char CMD_WORD_SETCONFIG = 'I';
final static char CMD_WORD_GETCONFIG = 'J';
final static char CMD_WORD_HEARTBEAT = 'K';
final static char CMD_WORD_LOADEEPROM = 'L';
final static char CMD_WORD_STOREEEPROM = 'M';
final static char CMD_WORD_CLEAREEPROM = 'N';

int ret_code = 0;
int ret_values[] = new int [10];

void clearSerial()
{
  receive_len = 0;
  start_received = false;
  end_received = false;
  send_len = 0;
  sendbuffer[send_len++] = CMD_START_CHAR;
}

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
    sendbuffer[send_len++] = (char)('0' + buffer[weishu - i]);
  }
}

int sendandcheck(int expected_num)
{
  int starttime;
  int para_num=0;
  int search_index;
  
  for(int i=0;i<send_len;i++)
  {
    myPort.write(sendbuffer[i]);
    print(sendbuffer[i]);
  }
  starttime = millis();
  while((millis() - starttime)<1000)
  {
    if(end_received && start_received)  
    {
      if(receivebuffer[start_pos+1] != sendbuffer[1]) return -1;  //returned command is incorrect
      if(receivebuffer[start_pos+2] != '0') 
      {
        ret_code = receivebuffer[start_pos+2];
        return -2;
      }
      search_index = start_pos+3;
      while(search_index < end_pos)
      {
        if(CMD_SEPARE_CHAR == receivebuffer[search_index])
        {
          ret_values[para_num] = 0;
          para_num ++;
        }
        else if (receivebuffer[search_index] >= '0' && receivebuffer[search_index] <= '9')
        {
          ret_values[para_num-1] = ret_values[para_num-1] * 10 + (receivebuffer[search_index]-'0');
        }
        search_index++;
      }
      if(para_num != expected_num) return -3; //the return parameters number is not expected.
      return 0;  //return ok
    }
    delay(1);
  }
  
  return -4;  //timeout //<>//
  
}


//commands
int setRealServo(int servo_index,int value)
{
  int ret;
  myPort.clear();
  clearSerial();

  sendbuffer[send_len++] = CMD_WORD_SETSERVO;
  writeSerialData(servo_index);
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(value);
  sendbuffer[send_len++] = (CMD_END_CHAR);
  
  ret = sendandcheck(0);
  
  if(0!=ret)
  {
    print(ret);
    print(char(ret_code));
  }
  return ret; 
}

int setDrum(int drum_id, int updown, int servo_id,int value)
{
  int ret;
  
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_SETDRUM);
  writeSerialData(drum_id);  
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(updown); //0,up,1,down
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(servo_id);  
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(value);
  sendbuffer[send_len++] = (CMD_END_CHAR); 
  ret = sendandcheck(0);
  if(0!=ret)
  {
    print(ret);
    print(char(ret_code));
  }
  return ret; 
  
}

int getDrum(int drum_id, int updown,int values[])
{
  int ret;
  
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_GETDRUM);
  writeSerialData(drum_id);  
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(updown); //0,up,1,down
  sendbuffer[send_len++] = (CMD_END_CHAR);
  ret = sendandcheck(4);
  if(0!=ret)
  {
    print(ret);
    print(char(ret_code));
  }
  return ret; 
}

int setConfig(int index, int value)
{
  int ret;
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_SETCONFIG);
  writeSerialData(index);
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  writeSerialData(value);
  sendbuffer[send_len++] = (CMD_END_CHAR);
  ret = sendandcheck(0);
  if(0!=ret)
  {
    print(ret);
    print(char(ret_code));
  }
  return ret; 
}

int setSong(int index, char drum_id)
{
  int ret;
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_SETSONG);
  writeSerialData(index);
  sendbuffer[send_len++] = (CMD_SEPARE_CHAR);
  sendbuffer[send_len++] = drum_id;
  sendbuffer[send_len++] = (CMD_END_CHAR);
  
  ret = sendandcheck(0);
  if(0!=ret)
  {
    print(ret);
    print(char(ret_code));
  }
  return ret; 
}

int startRun(int count)
{
  int ret;
  
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_RUN);
  writeSerialData(count);  
  sendbuffer[send_len++] = (CMD_END_CHAR);
  ret = sendandcheck(0);
  if(0!=ret)
  {
    print(ret);
    if(ret == -2) print(char(ret_code));
  }
  return ret; 
}

int loadTheEEPROM()
{
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_LOADEEPROM);
  sendbuffer[send_len++] = (CMD_END_CHAR);
  return 0;
}

int storeTheEEPROM()
{
  myPort.clear();
  clearSerial();
  sendbuffer[send_len++] = (CMD_WORD_STOREEEPROM);
  sendbuffer[send_len++] = (CMD_END_CHAR);
  return 0;
}