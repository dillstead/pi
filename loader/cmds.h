#ifndef CMDS_H
#define CMDS_H

#ifndef TEST
//#define MAX_RECORD    560
#define MAX_RECORD    522
#define MAX_DATA      255
#define MAX_CMDLINE   1024
#define MAX_ARGS      512
#else
#define MAX_RECORD    30
#define MAX_DATA      5
#define MAX_CMDLINE   10
#define MAX_ARGS      2
#endif

// Commands
#define CMD_INIT     0
#define CMD_START    1
#define CMD_RUN      2
#define CMD_RECORD   3
#define CMD_DONE     4
#define CMD_ERROR    5

// Record types
#define REC_DATA    0x00
#define REC_END     0x01
#define REC_XSEG    0x02
#define REC_SSEG    0x03
#define REC_XADDR   0x04
#define REC_SADDR   0x05
#define REC_SADDR   0x05
#define REC_CMDLINE 0x06
#endif
