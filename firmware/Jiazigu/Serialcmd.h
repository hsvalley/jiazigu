#ifndef __SERIALCMD_H__
#define __SERIALCMD_H__

//ASCII-BASED protocol definition
#define CMD_START_CHAR '<'
#define CMD_END_CHAR '>'
#define CMD_SEPARE_CHAR ','

enum {
  CMD_WORD_MIN =  'A',
  CMD_WORD_SETSERVO = CMD_WORD_MIN,
  CMD_WORD_GETSERVO  ,
  CMD_WORD_SETDRUM   ,
  CMD_WORD_GETDRUM   ,
  CMD_WORD_SETSONG   ,
  CMD_WORD_GETSONG   ,
  CMD_WORD_RUN       ,
  CMD_WORD_STOP      ,
  CMD_WORD_SETDELAY  ,
  CMD_WORD_GETDELAY  ,
  CMD_WORD_SETREPEAT  ,
  CMD_WORD_GETREPEAT  ,
  CMD_WORD_HEARTBEAT ,
  CMD_WORD_LOADEEPROM  ,
  CMD_WORD_STOREEEPROM  ,
  CMD_WORD_CLEAREEPROM ,
  CMD_WORD_MAX
}cmd_word_types;

#define CMD_NOT_START 0
#define CMD_STARTED 1
#define CMD_MAX_LENGTH 24

extern char cmd_buffer[];
extern int cmd_position;
extern int cmd_state;


void checkSerial();
int parsecmd(char *cmd, int *num, int *parameters);
void runcommand();
void startSerial();

#endif //__SERIALCMD_H__
