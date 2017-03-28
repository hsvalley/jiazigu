#include <Servo.h>
#include <EEPROM.h>

#include "Serialcmd.h"
#include "hand.h"
#include "foot.h"

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
#define STATE_SETTING 2
#define STATE_RUNNING 3

byte mystate;

hand lefthand;
hand righthand;
foot leftfoot;
foot rightfoot;

#define NUM_OF_SYS_PARAS 5
int sys_paras[NUM_OF_SYS_PARAS];

//definition for song
#define SONG_RUNNING_BEFORE_UP 0
#define SONG_RUNNING_IN_UP 1
#define SONG_RUNNING_BEFORE_DOWN 2
#define SONG_RUNNING_IN_DOWN 3

#define SONG_MAX_LENGTH 256
int song_pu[SONG_MAX_LENGTH];
int song_len;
int song_running_pos = 0;
int song_finished_count = 0;
int song_running_state = SONG_RUNNING_BEFORE_UP;

void setup()
{
  Serial.begin(57600);
  Serial.println("I am coming!");
  mystate = STATE_INIT;

  lefthand.Init(0);
  righthand.Init(1);
  leftfoot.Init(0);
  righthand.Init(1);
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
  checkSerial();

  if (STATE_SETTING == mystate)
  {
    if (!lefthand.gotarget())
    {
      mystate = STATE_READY;
    }
  }
  else if (STATE_RUNNING == mystate)
  {
    if (!lefthand.gotarget())
    {
      switch (song_running_state)
      {
        case SONG_RUNNING_BEFORE_UP:
          delay(sys_paras[1]);
          //Start UP
          lefthand.movemotorstodrum(song_pu[song_running_pos], 0);
          song_running_state = SONG_RUNNING_IN_UP;
          break;
        case SONG_RUNNING_IN_UP :
          //finish UP
          song_running_state = SONG_RUNNING_BEFORE_DOWN;
          break;
        case SONG_RUNNING_BEFORE_DOWN:
          delay(sys_paras[0]);
          //start DOWN
          lefthand.movemotorstodrum(song_pu[song_running_pos], 1);
          song_running_state = SONG_RUNNING_IN_DOWN;
          break;
        case SONG_RUNNING_IN_DOWN :
          //finish DOWN, to judge if the song is over
          song_running_pos ++;
          if (song_len == song_running_pos)
          {
            //arrived the end of song
            song_running_pos = 0;
            song_finished_count ++;
            if (song_finished_count == sys_paras[2])
            {
              mystate = STATE_READY; //finish repeat times, stop playing
            }
            else
            {
              song_running_state = SONG_RUNNING_BEFORE_UP; //continue
            }
          }
          else
          {
            song_running_state = SONG_RUNNING_BEFORE_UP; //continue
          }
          break;
        default:
          break;
      }
    }
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

void response(char command, char res_code, int len)
{
  Serial.print(CMD_START_CHAR);
  Serial.print(command);
  Serial.print(res_code);
  for (int i = 0; i < len; i++)
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
  int paras[8];
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
      cmdsetmotors(cmd, numofpara, &paras[0]);
      break;

    case CMD_WORD_GETSERVO  :
      cmdgetmotors(cmd, &paras[0]);
      break;

    case CMD_WORD_SETDRUM   :
      cmdsetdrum(cmd, &paras[0]);
      break;

    case CMD_WORD_GETDRUM   :
      cmdgetdrum(cmd, &paras[0]);
      break;

    case CMD_WORD_SETSONG   :
      cmdsetsong(cmd, &paras[0]);
      break;

    case CMD_WORD_GETSONG   :
      cmdgetsong(cmd, &paras[0]);
      break;

    case CMD_WORD_RUN   :
      if (paras[0] > SONG_MAX_LENGTH)
      {
        response(cmd, RSP_ERROR_PARAMETER, 0);
      }
      else
      {
        song_len = paras[0];
        mystate = STATE_RUNNING;
        song_running_pos = 0;
        song_finished_count = 0;
        song_running_state = SONG_RUNNING_BEFORE_UP;
        response(cmd, RSP_OK, 0);
      }
      break;

    case CMD_WORD_STOP      :
      //TEMP add debug code
      Serial.println(debug_code1);
      Serial.println(debug_code2);
      break;

    case CMD_WORD_SETCONFIG  :
      cmdsetconfig(cmd, &paras[0]);
      break;

    case CMD_WORD_GETCONFIG  :
      cmdgetconfig(cmd, &paras[0]);
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


void cmdsetmotors(char cmd, int num, int parameters[])
{
  int ret;
  int motor_id = parameters[0]; //0-3,lefthand;4-7,righthand;8,leftfoot;12,rightfoot

  if (4 > motor_id)
  {
    //left hand
    if (2 == num)
    {
      ret = lefthand.moveonemotor(motor_id, parameters[1]); //move to one motor
    }
    else if (5 == num)
    {
      ret = lefthand.moveallmotors(&parameters[1]);
    }
  }
  else if (8 > motor_id)
  {
    //right hand
    if (2 == num)
    {
      ret = righthand.moveonemotor(motor_id - 4, parameters[1]);
    }
    else if (5 == num)
    {
      ret = righthand.moveallmotors(&parameters[1]);
    }
  }
  else if (8 == motor_id)
  {
    //left foot
    ret = leftfoot.moveonemotor(parameters[1]);
  }
  else if (12 == motor_id)
  {
    //right foot
    ret = rightfoot.moveonemotor(parameters[1]);
  }
  else if (16 == motor_id)
  {
    //head
    //ret = head.moveonemotor(parameters[1]);
  }
  mystate = STATE_SETTING;
  //response
  if (0 == ret)
    response(cmd, RSP_OK, 0);
  else
    response(cmd, RSP_ERROR_PARAMETER, 0);
}

void cmdgetmotors(char cmd, int parameters[])
{
  int motor_id = parameters[0]; //0-3,lefthand;4-7,righthand;8,leftfoot;12,rightfoot
  response_buffer[0] = motor_id;

  if (4 > motor_id)
  {
    //left hand
    response_buffer[1] = lefthand.getmotor(motor_id);
  }
  else if (8 > motor_id)
  {
    //right hand
    response_buffer[1] = righthand.getmotor(motor_id - 4);
  }
  else if (8 == motor_id)
  {
    //left foot
    response_buffer[1] = leftfoot.getmotor();
  }
  else if (12 == motor_id)
  {
    //left foot
    response_buffer[1] = rightfoot.getmotor();
  }
  else if (10 == motor_id)
  {
    //head
    //response_buffer = head.getpos();
  }
  //response
  response(cmd, RSP_OK, 2);
}

void cmdsetdrum(char cmd, int parameters[])
{
  int ret;
  int drum_id = parameters[0]; //0-7,lefthand;8-15,righthand;16,leftfoot;24,rightfoot
  int updown = parameters[1];
  int motor_id = parameters[2]; //0-3,lefthand;4-7,righthand;
  int value = parameters[3];

  if (drum_id < 8) {
    //lefthand
    if (motor_id < 4) ret = lefthand.setdrumbyvalue(drum_id, updown, motor_id, value);
    else ret = -1;
  } else if (drum_id < 16) {
    //right hand
    if (motor_id >= 4 && motor_id < 8) ret = righthand.setdrumbyvalue(drum_id - 8, updown, motor_id - 4, value);
    else ret = -1;
  } else if (drum_id == 16) {
    //left foot
    ret = leftfoot.setdrumbyvalue(updown, value);
  } else if (drum_id == 24) {
    //right foot
    ret = rightfoot.setdrumbyvalue(updown, value);
  } else {
    ret = -1;
  }
  if (ret == 0) {
    response(cmd, RSP_OK, 0);
  } else {
    response(cmd, RSP_ERROR_PARAMETER, 0);
  }
}

void cmdgetdrum(char cmd, int parameters[])
{
  int ret;
  int ret_num = 6;
  int drum_id = parameters[0]; //0-7,lefthand;8-15,righthand;16,leftfoot;24,rightfoot
  int updown = parameters[1];

  response_buffer[0] = drum_id;
  response_buffer[1] = updown;

  if (drum_id < 8) {
    //left hand
    ret = lefthand.getdrum(drum_id, updown, &response_buffer[2]);
  } else if (drum_id < 16) {
    //right hand
    ret = righthand.getdrum(drum_id - 8, updown, &response_buffer[2]);
  } else if (drum_id == 16) {
    //left foot
    ret = leftfoot.getdrum(updown, &response_buffer[2]);
    ret_num = 3;
  } else if (drum_id == 24) {
    //right foot
    ret = rightfoot.getdrum(updown, &response_buffer[2]);
    ret_num = 3;
  } else {
    ret = -1;
  }
  
  mystate = STATE_SETTING ;
  
  if (ret == 0)
    response(cmd, RSP_OK, ret_num);
  else
    response(cmd, RSP_ERROR_PARAMETER, 2);
}

void cmdsetconfig(char cmd, int parameters[])
{
  if (parameters[0] >= NUM_OF_SYS_PARAS)
  {
    response(cmd, RSP_ERROR_PARAMETER, 0);
    return;
  }
  sys_paras[parameters[0]] = parameters[1];
  response(cmd, RSP_OK, 0);
}

void cmdgetconfig(char cmd, int parameters[])
{
  if (parameters[0] >= NUM_OF_SYS_PARAS)
  {
    response(cmd, RSP_ERROR_PARAMETER, 0);
    return;
  }
  response_buffer[0] = sys_paras[parameters[0]];
  response(cmd, RSP_OK, 1);
}


void cmdsetsong(char cmd, int parameters[])
{
  if ((parameters[0] >= SONG_MAX_LENGTH) || (parameters[1] >= MAXDRUMS) )
  {
    response(cmd, RSP_ERROR_PARAMETER, 0);
  }
  else
  {
    song_pu[parameters[0]] = parameters[1];
    response(cmd, RSP_OK, 0);
  }
}

void cmdgetsong(char cmd, int parameters[])
{
  if (parameters[0] >= SONG_MAX_LENGTH)
  {
    response(cmd, RSP_ERROR_PARAMETER, 0);
    return;
  }
  response_buffer[0] = song_pu[parameters[0]];
  response(cmd, RSP_OK, 1);
}
