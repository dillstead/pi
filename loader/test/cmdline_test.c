#define _POSIX_SOURCE
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#define TEST
#include "cmds.h"
#include "cmdline.h"
#include "parser.h"

struct test
{
    int argc;
    char **argv;
    int expected;
};

static bool run_test(struct test *test)
{
    size_t flat_used;
    size_t flat_remain;
    char flat[MAX_CMDLINE];
    size_t len;
    size_t cmdline_off = 0;
    char cmdline[MAX_CMDLINE];
    uint8_t record[MAX_RECORD];
    struct parser parser = { 0 };

    flat_used = 0;
    flat_remain = flatten_cmdline(tests->argv, tests->argc, flat, sizeof flat);
    if (flat__remain < 0)
    {
        return false;
    }
    
    while (flat_remain > 0)
    {
        len = cmdline_encode(flat_cmdline + flat_cmdline_used, flat_cmdline_remain,
                             record);
        cmdline_used += len;
        cmdline_remain -= len;
            
        parser_set_record(&parser, record, len);
        res = parser_parse(&parser);
        if (res < 0 || parser.type != REC_CMDLINE)
        {
            // error
        }
            

        if (cmdline_off + parser.byte_count > sizeof cmdline)
        {
            res = ERR_CMD_TOO_LONG;
            break;
        }
        memcpy(cmdline + cmdline_off, parser.record, parser.byte_count);
        cmdline_off += parser.byte_count;
    }

    setup_args(cmdline, cmdline_off, &argc, argv);
    // compare 
    
    return 0;
}

// too many args
// cmd line too long
// zero args
// single record
// split across multiple records
int main(int argc, char **argv)
{
    struct test tests[] =
        {
            { 3, { "cmdline_test", "test1" }, 0 }
        };
    
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        run_test(&tests[i]);
    }
    return EXIT_SUCCESS;
}
