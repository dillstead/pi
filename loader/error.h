#ifndef ERROR_H
#define ERROR_H

#define MAX_ERRMSG        128
#define SUCCESS           0
#define ERR_EOL          -1
#define ERR_TOO_LONG     -2
#define ERR_CMD_TOO_LONG -3
#define ERR_INVAL_CHAR   -4
#define ERR_INVAL_TYPE   -5
#define ERR_INVAL_COUNT  -6
#define ERR_INVAL_CKSUM  -7
#define ERR_ADDR_OVERLAP -8

static const char err_msgs[][MAX_ERRMSG] =
{
    "",
    "end of line",
    "record too long",
    "command line too long",
    "invalid character",
    "invalid record type",
    "invalid byte count",
    "invalid checksum",
    "address overlap"
};

#endif
