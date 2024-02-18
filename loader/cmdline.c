#include <stdio.h>
#include <bsd/string.h>
#include "cmds.h"
#include "cmdline.h"

static uint8_t bin_to_hex[16] =
{
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
};

ssize_t flatten_cmdline(char **argv, int argc, char *cmdline, size_t cmdline_sz)
{
    size_t cmdline_used = 0;
    size_t cmdline_remain = cmdline_sz;
    int num_args = 0;
    size_t len;
    
    for (int i = 0; i < argc; i++)
    {
        len = strlcpy(cmdline + cmdline_used, argv[i], cmdline_remain);
        if (len >= cmdline_remain)
        {
            fprintf(stderr, "Command line too long\n");
            return -1;
        }
        cmdline_used += len + 1;
        cmdline_remain -= len + 1;
        num_args++;
        if (num_args > MAX_ARGS)
        {
            fprintf(stderr, "Too many arguments\n");
            return -1;
        }
    }
    return cmdline_used;
}

size_t cmdline_encode(char *cmdline, size_t cmdline_remain, uint8_t *record)
{
    uint8_t checksum = 0;
    size_t len = cmdline_remain < MAX_DATA ? cmdline_remain : MAX_DATA;

    // Each encoded record is:
    // : start_code byte_count address record_type data checksum crlf
    *record++ = ':';
    // byte_count
    *record++ = bin_to_hex[(len >> 4) & 0xF];
    *record++ = bin_to_hex[len & 0xF];
    checksum += len;
    // address
    *record++ = '0';
    *record++ = '0';
    *record++ = '0';
    *record++ = '0';
    // record_type
    *record++ = '0';
    *record++ = '6';
    checksum += 6;
    // data
    for (unsigned int i = 0; i < len; i++)
    {
        *record++ = bin_to_hex[(cmdline[i] >> 4) & 0xF];
        *record++ = bin_to_hex[cmdline[i] & 0xF];
        checksum += cmdline[i];
    }
    // checksum
    checksum = ~checksum + 1;
    *record++ = bin_to_hex[(checksum >> 4) & 0xF];
    *record++ = bin_to_hex[checksum & 0xF];
    //crlf
    *record++ = '\r';
    *record++ = '\n';
    return len;
}
