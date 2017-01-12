#include <Servo.h>
#include <EEPROM.h>

#include "Serialcmd.h"
#include "hand.h"

char cmd_buffer[CMD_MAX_LENGTH];
int cmd_position = 0;
int cmd_state = CMD_NOT_START;
int response_buffer[10];

#define __DEBUG_PRINT__
//#define __FURTHER_CHECKING__
int debug_code1 = 0;
int debug_code2 = 0;

#ifdef __FURTHER_CHECKING__
int NumOfCmd[CMD_WORD_MAX - CMD_WORD_MIN] = {
  2,  //CMD_WORD_SETSERVO = CMD_WORD_MIN,
  1,  //CMD_WORD_GETSERVO  ,
  4,  //CMD_WORD_SETDRUM   ,
  3,  //CMD_WORD_GETDRUM   ,
  2,  //CMD_WORD_SETSONG   ,
  1,  //CMD_WORD_GETSONG   ,
  1,  //CMD_WORD_RUN       ,
  0,  //CMD_WORD_STOP      ,
  2,  //CMD_WORD_SETDELAY  ,
  1,  //CMD_WORD_GETDELAY  ,
  1,  //CMD_WORD_SETREPEAT  ,
  0,  //CMD_WORD_GETREPEAT  ,
  0   //CMD_WORD_HEARTBEAT ,
};

/*
#define checkServoNum(a)   ((a<SERVO_NUM)?0:-4)
#define checkServoValue(a) (((a>=500) && (a<=2500))?0:-5)
#define checkDrumNum(a)    ((a<TOTAL_DRUM+1)?0:-6)
#define checkToneNum(a)    ((a<MAX_TONES)?0:-7)
#define checkDelayNum(a)   ((a<2)?0:-8)
#define checkDelayValue(a) (((a>=50) && (a<=2000))?0:-9)
#define checkRepeatNum(a)  (((a>0) && (a<=16))?0:-10)
#define checkUpdownNum(a)  ((a<2)?0:-11)
*/
#endif



#define STATE_INIT 0
#define STATE_READY 1
#define STATE_PLAYING 2

byte mystate;

hand lefthand;
//hand *righthand;
//foot *leftfoot;
//foot *rightfoot;

Servo gwanservo;
Servo gzhouservo;

void setup()
{
  Serial.begin(19200);
  Serial.println("I am coming!");
  lefthand.Init(0);
  //righthand = new hand(1);
  //  LoadEEPROM();
}

void LoadEEPROM()
{
  byte buf[268];

  //check the maggic code
  if ((EEPROM.read(0) == 'V') && (EEPROM.read(1) == 'A'))
  {
    for (int i = 0; i < 128; i++)
    {
      buf[i] = EEPROM.read(i + 2);
    }
    lefthand.load(buf);

  }
}


void StoreEEPROM()
{
  byte buf[128];

  //save the maggic code
  EEPROM.write(0, 'V');
  EEPROM.write(1, 'A');
  lefthand.store(buf);
  for (int i = 0; i < 128; i++)
  {
    EEPROM.write(i + 2, buf[i]);
  }
  /*
  righthand.store(&buf[128]);
  for (int i = 0; i < 128; i++)
  {
    EEPROM.write(i + 130, buf[i]);
  }
  leftfoot.store(&buf[256]);
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i + 258, buf[i]);
  }
  rightfootstore(&buf[260]);
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i + 262, buf[i]);
  }
  head.store(&buf[264]);
  for (int i = 0; i < 4; i++)
  {
    EEPROM.write(i + 266, buf[i]);
  }
  */

}

void ClearEEPROM()
{
  EEPROM.write(0, 0);
  EEPROM.write(1, 0);
}


void loop()
{
  static unsigned long count = 0;
  char temp;
  checkSerial();
  count++;
  if (count == 300000)
  {
    count = 0;

    Serial.println(debug_code1);
    Serial.println(debug_code2);
  }

  if (STATE_PLAYING == mystate)
  {
    lefthand.gotarget();
  }
}

void checkSerial()
{
  char temp;
  // read all the available characters
  while (Serial.available() > 0) {
    temp = Serial.read();
    switch (cmd_state) {
      case CMD_NOT_START:
        if (temp == CMD_START_CHAR) {
          //start command
          cmd_position = 0;
          cmd_state = CMD_STARTED;
        }
        break;

      case CMD_STARTED:
        if (temp == CMD_START_CHAR) {
          //restart the command
          cmd_position = 0;
          cmd_state = CMD_STARTED;
        } else if (temp == CMD_END_CHAR) {
          //got command end char, run the command
          runcommand();
          cmd_state = CMD_NOT_START;
        } else {
          cmd_buffer[cmd_position++] = temp;
          if (cmd_position >= CMD_MAX_LENGTH - 1) {
            cmd_state = CMD_NOT_START;
          }
        }
        break;

      default:
        cmd_state = CMD_NOT_START;
        break;
    }
  }
}


int parsecmd(char *cmd, int *num, int *parameters)
{
  int count = 0;
  parameters[count] =  0;

  //check command
  *cmd = cmd_buffer[0];
  if ((*cmd < CMD_WORD_MIN) || (*cmd >= CMD_WORD_MAX)) {
    return -1;
  }

  for (int pos = 1; pos < cmd_position; pos++) {
    char thischar = cmd_buffer[pos];
    if (thischar == ',') {
      //parameter seperater
      count ++;
      parameters[count] =  0;
    }
    else if ((thischar >= '0') && (thischar <= '9')) {
      //valid data
      parameters[count] = parameters[count] * 10 + (thischar - '0');
    }
    else if (thischar != ' ') {
      //invalid data
      return -2;
    } //else if it's space, ignore it
  }

  if (cmd_position == 1) {
    *num = 0;
  }
  else {
    *num = count + 1;
  }

#ifdef __FURTHER_CHECKING__
  int ret;
  if (*num != NumOfCmd[*cmd - CMD_WORD_MIN]) {
    return -3;
  }

  switch (*cmd) {
    case CMD_WORD_SETSERVO  :
      //0:servo no, 1:servo value
      ret = checkServoNum(parameters[0]); if (ret != 0) return ret;
      ret = checkServoValue(parameters[1]); if (ret != 0) return ret;
      break;

    case CMD_WORD_GETSERVO  :
      ret = checkServoNum(parameters[0]); if (ret != 0) return ret;
      break;

    case CMD_WORD_SETDRUM   :
      //0:drum no, 1:servo no, 1:servo value
      ret = checkDrumNum(parameters[0]); if (ret != 0) return ret;
      ret = checkUpdownNum(parameters[1]); if (ret != 0) return ret;
      ret = checkServoNum(parameters[2]); if (ret != 0) return ret;
      ret = checkServoValue(parameters[3]); if (ret != 0) return ret;
      break;

    case CMD_WORD_GETDRUM   :
      //0:drum no, 1:servo no,
      ret = checkDrumNum(parameters[0]); if (ret != 0) return ret;
      ret = checkServoNum(parameters[1]); if (ret != 0) return ret;
      break;

    case CMD_WORD_SETSONG   :
      //0:tone no, 1:drum value
      ret = checkToneNum(parameters[0]); if (ret != 0) return ret;
      ret = checkDrumNum(parameters[1]); if (ret != 0) return ret;
      break;

    case CMD_WORD_GETSONG   :
      //0:tone no,
      ret = checkToneNum(parameters[0]); if (ret != 0) return ret;
      break;

    case CMD_WORD_RUN   :
      //0:tone count
      ret = checkToneNum(parameters[0]); if (ret != 0) return ret;
      break;

    case CMD_WORD_STOP:
      //no need check
      break;

    case CMD_WORD_SETDELAY  :
      //0:delay no, 1:delay value
      ret = checkDelayNum(parameters[0]); if (ret != 0) return ret;
      ret = checkDelayValue(parameters[1]); if (ret != 0) return ret;

      break;

    case CMD_WORD_GETDELAY  :
      //0:delay no,
      ret = checkDelayNum(parameters[0]); if (ret != 0) return ret;
      break;

    case CMD_WORD_SETREPEAT :
      //0:repeat
      ret = checkRepeatNum(parameters[0]); if (ret != 0) return ret;
      break;

    case CMD_WORD_GETREPEAT :
      break;

    case CMD_WORD_HEARTBEAT :
      break;

    default:
      break;
  }
#endif

  return 0;
}

void response(char command, int res_code,int len)
{
      Serial.print(CMD_START_CHAR);
      Serial.print(command);
      Serial.print(res_code);
      for(int i=0;i<len;i++)
      {
        Serial.print(CMD_SEPARE_CHAR);
        Serial.print(response_buffer[i]);
      }
      Serial.println(CMD_END_CHAR);
}

void runcommand()
{
  char cmd;
  int numofpara;
  int paras[5];
  int ret;
  unsigned long now;
   
  ret = parsecmd(&cmd, &numofpara, &paras[0]);
  if (ret < 0)
  {
#ifdef __DEBUG_PRINT__
    Serial.print("Error command ");
    Serial.println(ret);
#endif
    cmd_state = CMD_NOT_START;
    return;
  }

  switch (cmd)
  {
    case CMD_WORD_SETSERVO  :
      if (4 > paras[0])
      {
        //left hand
        if (2 == numofpara)
        {
          lefthand.moveonemotor(paras[0], paras[1]);
        }
        else if (5 == numofpara)
        {
          lefthand.moveallmotors(&paras[1]);
        }
      }
      else if (8 > paras[0])
      {
        //right hand
        if (2 == numofpara)
        {
          //righthand.moveonemotor(paras[0] - 4, paras[1]);
        }
        else if (5 == numofpara)
        {
          //righthand.moveallmotors(&paras[1]);
        }
      }
      else if (8 == paras[0])
      {
        //left foot
        //leftfoot.moveonemotor(paras[1]);
      }
      else if (9 == paras[0])
      {
        //left foot
        //rightfoot.moveonemotor(paras[1]);
      }
      else if (10 == paras[0])
      {
        //head
        //head.moveonemotor(paras[1]);
      }
      mystate = STATE_PLAYING;
      //response
      response(cmd,RSP_OK,0);

      break;

    case CMD_WORD_GETSERVO  :
   
      if (4 > paras[0])
      {
        //left hand
        response_buffer[0] = lefthand.getmotor(paras[0]);
      }
      else if (8 > paras[0])
      {
        //right hand
        //response_buffer[0] = righthand.getmotor(paras[0] - 4);
      }
      else if (8 == paras[0])
      {
        //left foot
        //response_buffer[0] = leftfoot.getpos();
      }
      else if (9 == paras[0])
      {
        //left foot
        //response_buffer[0] = rightfoot.getpos();
      }
      else if (10 == paras[0])
      {
        //head
        //thepos = head.getpos();
      }
      //response
      response(cmd,RSP_OK,1);
      break;

    case CMD_WORD_SETDRUM   :
      break;

    case CMD_WORD_GETDRUM   :
      break;

    case CMD_WORD_SETSONG   :
      break;

    case CMD_WORD_GETSONG   :
      break;

    case CMD_WORD_RUN   :
      break;

    case CMD_WORD_STOP      :
      break;

    case CMD_WORD_SETCONFIG  :
      break;

    case CMD_WORD_GETCONFIG  :
      break;

    case CMD_WORD_SETREPEAT :
      //0:repeat
      break;

    case CMD_WORD_GETREPEAT :
      break;

    case CMD_WORD_HEARTBEAT :
      break;

    case CMD_WORD_LOADEEPROM:
             LoadEEPROM();
      break;

    case CMD_WORD_STOREEEPROM:
             StoreEEPROM();
      break;

    case CMD_WORD_CLEAREEPROM:
             ClearEEPROM();
      break;

    default:
      break;
  }
  cmd_state = CMD_NOT_START;
}

