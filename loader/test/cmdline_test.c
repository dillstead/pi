#define _POSIX_SOURCE
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "cmds.h"
#include "cmdline.h"
#include "parser.h"
#include "utils.h"

struct test
{
    int expected_argc;
    char *expected_argv[MAX_ARGS + 1];
    bool expected;
};

static bool cmp_args(int argc1, char **argv1, int argc2, char **argv2)
{
    if (argc1 != argc2)
    {
        return false;
    }
    for (int i = 0; i < argc1; i++)
    {
        if (strcmp(argv1[i], argv2[i]))
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
    size_t rlen;
    size_t cmdline_off = 0;
    char cmdline[MAX_CMDLINE];
    uint8_t record[MAX_RECORD];
    struct parser parser = { 0 };
    int actual_argc;
    char *actual_argv[MAX_ARGS];
    int res;

    flat_used = 0;
    flat_remain = flatten_cmdline(test->expected_argv, test->expected_argc, flat, sizeof flat);
    if (flat_remain > SSIZE_MAX)
    {
        return false;
    }
    
    while (flat_remain > 0)
    {
        len = cmdline_encode(flat + flat_used, flat_remain, record, &rlen);
        flat_used += len;
        flat_remain -= len;
            
        parser_set_record(&parser, record, rlen);
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
    return cmp_args(actual_argc, actual_argv, test->expected_argc, test->expected_argv);
}

int main(void)
{
    struct test tests[] =
    {
        // Too many args
        { 3, { "a", "b", "c" }, false },
        // Too long
        { 1, { "abcdefghij" }, false },
        { 2, { "ab", "cdefghi" }, false },
        // No args
        { 0, { 0 }, true },
        { 1, { "a" }, true},
        { 2, { "a", "b" }, true},
        { 1, { "abcdefghi" }, true},
        { 2, { "abcdef", "gh" },  true },
    };
    
    for (size_t i = 0; i < sizeof tests / sizeof tests[0]; i++)
    {
        if (run_test(&tests[i]) != tests[i].expected)
        {
            printf("Test %zu failed\n", i + 1);
        }
    }
    return EXIT_SUCCESS;
}
