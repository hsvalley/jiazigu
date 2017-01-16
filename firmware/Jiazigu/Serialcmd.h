#ifndef __SERIALCMD_H__
#define __SERIALCMD_H__

//ASCII-BASED protocol definition
#define CMD_START_CHAR '<'
#define CMD_END_CHAR '>'
#define CMD_SEPARE_CHAR ','

#define RSP_OK '0'
#define RSP_ERROR_LENGTH '1'
#define RSP_ERROR_CHECKSUM '2'
#define RSP_ERROR_BUSY '3'
#define RSP_ERROR_PARAMETER '4'

 #define CMD_WORD_SETSERVO    'A'
 #define CMD_WORD_GETSERVO    'B'
 #define CMD_WORD_SETDRUM     'C'
 #define CMD_WORD_GETDRUM     'D'
 #define CMD_WORD_SETSONG     'E'
 #define CMD_WORD_GETSONG     'F'
 #define CMD_WORD_RUN         'G'
 #define CMD_WORD_STOP        'H'
 #define CMD_WORD_SETCONFIG   'I'
 #define CMD_WORD_GETCONFIG   'J'
 #define CMD_WORD_HEARTBEAT   'K'
 #define CMD_WORD_LOADEEPROM  'L'
 #define CMD_WORD_STOREEEPROM 'M'
 #define CMD_WORD_CLEAREEPROM 'N'

#define CMD_WORD_MIN CMD_WORD_SETSERVO
#define CMD_WORD_MAX CMD_WORD_CLEAREEPROM

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
