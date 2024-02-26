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
    int expected_argc;
    char **expected_argv;
    bool expected;
};

static bool cmd_args(int argc1, char **argv1, int argc2, char **argv2)
{
    if (argc1 != argc2)
    {
        return false;
    }
    for (int i = 0; i < argc1; i++)
    {
        if (!strcmp(argv1[i], argv2[i]))
        {
            return false;
        }
    }
    return true;
}

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
    int actual_argc;
    char **actual_argv;

    flat_used = 0;
    flat_remain = flatten_cmdline(tests->argv, tests->argc, flat, sizeof flat);
    if (flat__remain < 0)
    {
        return false;
    }
    
    while (flat_remain > 0)
    {
        len = cmdline_encode(flat_cmdline + flat_used, flat_remain, record);
        flat_used += len;
        flat_remain -= len;
            
        parser_set_record(&parser, record, len);
        res = parser_parse(&parser);
        if (res < 0 || parser.type != REC_CMDLINE)
        {
            return false;
        }
            
        if (cmdline_off + parser.byte_count > sizeof cmdline)
        {
            return false;
        }
        memcpy(cmdline + cmdline_off, parser.record, parser.byte_count);
        cmdline_off += parser.byte_count;
    }

    setup_args(cmdline, cmdline_off, &actual_argc, actual_argv);
    return cmp_args(actual_argc, actual_argv, test->argc, test->argv);
}

int main(void)
{
    struct test tests[] =
    {
        // Too many args
        { 5, { "a", "b", "c", "d", "e" }, false },
        // Too long
        { 2, { "abcdefghi", "jklmnopqrstuvwxyz" }, false },
        // Single record
        { 2, { "a", "b" },  true },
        // Multiple records
        { 2, { "abcd", "efcg" },  true },
    };
    
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        run_test(&tests[i]);
    }
    return EXIT_SUCCESS;
}
