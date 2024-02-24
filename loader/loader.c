#include <stddef.h>
#include <stdint.h>
#include "uart.h"
#include "led.h"
#include "error.h"
#include "cmds.h"
#include "utils.h"
#include "parser.h"

static char cmdline[MAX_CMDLINE];
static int argc;
static char *argv[MAX_ARGS];

static void write_byte(uint8_t val)
{
    uart_write_byte(val);
}

static void write_string(const char *str)
{
    do
    {
        uart_write_byte(*str);
    } while (*str++);
}

// Reads a single record into a record buffer and returns SUCCESS or ERR_TOO_LONG
// if the record is too long to buffer.  Records MUST be terminated with \n or \r\n.
// The termination character(s) are not copied into the record buffer.
static int read_record(uint8_t *record, size_t sz, size_t *len)
{
    size_t i = 0;

    // Lines are expected to end with \n or \r\n.
    while (i < sz)
    {
        record[i] = uart_read_byte();
        if (record[i] == '\n')
        {
            break;
        }
        else if (record[i] != '\r')
        {
            i++;
        }
    }
    if (i == sz)
    {
        return ERR_TOO_LONG;
    }
    *len = i;
    return SUCCESS;
}

void loader_main(uint32_t r0, uint32_t r1, void *atags)
{
    (void) r0;
    (void) r1;
    struct parser parser;
    uint8_t record[MAX_RECORD];
    size_t cmdline_off = 0;
    size_t len;
    int res;

    uart_init();
    led_init();
    memset(&parser, 0, sizeof parser);
    led_blink(1, 1000000);
    // Wait for CMD_INIT before sending commands.
    uart_read_byte();
    write_byte(CMD_START);
    
    for (;;)
    {
        // Ask for records until either an error occurs or an END record is received.
        write_byte(CMD_RECORD);
        res = read_record(record, sizeof record, &len);
        if (res < 0)
        {
            break;
        }
        parser_set_record(&parser, record, len);
        led_blink(1, 1000000);
        
        res = parser_parse(&parser);
        if (res < 0 || parser.type == REC_END)
        {
            break;
        }
        led_blink(1, 1000000);
        
        if (parser.type == REC_DATA)
        {
            // Copy data to the appropriate location in physical memory.
            memcpy((void *) parser.base_addr + parser.offset, parser.record,
                   parser.byte_count);
        }
        else if (parser.type == REC_CMDLINE)
        {
            if (cmdline_off + parser.byte_count > sizeof cmdline)
            {
                res = ERR_CMD_TOO_LONG;
                break;
            }
            memcpy(cmdline + cmdline_off, parser.record, parser.byte_count);
            cmdline_off += parser.byte_count;
        }
    }

    if (res == SUCCESS)
    {
        write_byte(CMD_DONE);
        setup_args(cmdline, cmdline_off, &argc, argv);
        // Wait for CMD_RUN
        uart_read_byte();
        ((void (*)(int, char **, void *)) parser.start_addr)(argc, argv, atags);
    }
    else
    {
        write_byte(CMD_ERROR);
        write_string(err_msgs[-res]);
    }
}
