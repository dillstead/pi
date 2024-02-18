#include <stddef.h>
#include <stdint.h>
#include "uart.h"
#include "led.h"
#include "cmds.h"

#define SUCCESS           0
#define ERR_EOL          -1
#define ERR_TOO_LONG     -2
#define ERR_CMD_TOO_LONG -3
#define ERR_INVAL_CHAR   -4
#define ERR_INVAL_TYPE   -5
#define ERR_INVAL_COUNT  -6
#define ERR_INVAL_CKSUM  -7

static const char err_msgs[][MAX_ERRMSG] =
{
    "",
    "end of line",
    "record too long",
    "command line too long",
    "invalid character",
    "invalid record type",
    "invalid byte count",
    "invalid checksum"
};

struct parser
{
    size_t off;
    uint8_t record[MAX_RECORD];
    size_t record_len;
    uint8_t byte_count;
    uint16_t offset;
    uint8_t type;
    uint32_t base_addr;
    uint32_t start_addr;
    uint8_t checksum;
};

static char cmdline[MAX_CMDLINE];
static int argc;
static char *argv[MAX_ARGS];

static void *memset(void *dst_, int value, size_t size) 
{
    unsigned char *dst = dst_;

    while (size-- > 0)
    {
        *dst++ = value;
    }
    return dst_;
}

static void *memcpy(void *dst_, const void *src_, size_t size) 
{
    unsigned char *dst = dst_;
    const unsigned char *src = src_;

    while (size-- > 0)
    {
        *dst++ = *src++;
    }

    return dst_;
}

static unsigned int hex_to_bin(uint8_t hex)
{
    if (hex >= '0' && hex <= '9')
    {
        return hex - '0';
    }
    else if (hex >= 'A' && hex <= 'F')
    {
        return 0xA + hex - 'A';
    }
    else if (hex >= 'a' && hex <= 'f')
    {
        return 0xA + hex - 'a';
    }
    return 256;
}

// Reads a single record into a record buffer and returns SUCCESS or ERR_TOO_LONG
// if the record is too long to buffer.  Records MUST be terminated with \n or \r\n.
// The termination character(s) are not copied into the record buffer.
static int parser_read_record(struct parser *parser)
{
    size_t i = 0;

    // Lines are expected to end with \n or \r\n.
    while (i < sizeof parser->record)
    {
        parser->record[i] = uart_read_byte();
        if (parser->record[i] == '\n')
        {
            break;
        }
        else if (parser->record[i] != '\r')
        {
            i++;
        }
    }
    if (i == sizeof parser->record)
    {
        return ERR_TOO_LONG;
    }
    parser->record_len = i;
    return SUCCESS;
}

// Reads a buffered char and returns SUCCESS or ERR_EOL if there are no more characters
// to read.  Unlike the other read_* functions, reading a character is not computed in
// the checksum.
static int read_char(struct parser *parser, uint8_t *value)
{
    if (parser->off >= parser->record_len)
    {
        return ERR_EOL;
    }
    *value = parser->record[parser->off++];
    return SUCCESS;
}

// Reads a buffered byte and returns SUCCESS, ERR_INVAL_CHAR if the characters are not
// hex digits or ERR_EOL if there are no more characters to read.
static int read_byte(struct parser *parser, uint8_t *value)
{
    uint8_t high;
    uint8_t low;
    unsigned int bin;

    if (parser->off + 1 >= parser->record_len)
    {
        return ERR_EOL;
    }
    high = hex_to_bin(parser->record[parser->off++]);
    low = hex_to_bin(parser->record[parser->off++]);
    bin = (high << 4) | low;
    if (bin > 255)
    {
        return ERR_INVAL_CHAR;
    }
    *value = (uint8_t) (bin & 0xFF);
    parser->checksum += *value;
    return SUCCESS;
}

// Reads a buffered unsigned short and returns SUCCESS, ERR_INVAL_CHAR if the characters are not
// hex digits, or or ERR_EOL if there are no more characters to read.
static int read_ushort(struct parser *parser, uint16_t *value)
{
    int res;
    uint8_t high;
    uint8_t low;

    res = read_byte(parser, &high);
    if (res < 0)
    {
        return res;
    }
    res = read_byte(parser, &low);
    if (res < 0)
    {
        return res;
    }
    *value = high << 8 | low;
    return SUCCESS;
}

// Reads a buffered unsigned int and returns SUCCESS, ERR_INVAL_CHAR if the characters are not
// hex digits, or or ERR_EOL if there are no more characters to read.
static int read_uint(struct parser *parser, uint32_t *value)
{
    int res;
    uint16_t high;
    uint16_t low;

    res = read_ushort(parser, &high);
    if (res < 0)
    {
        return res;
    }
    res = read_ushort(parser, &low);
    if (res < 0)
    {
        return res;
    }
    *value = (uint32_t) high << 16 | low;
    return SUCCESS;
}

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

static int parser_parse(struct parser *parser)
{
    int res;
    uint8_t byte;

    parser->off = 0;
    do
    {
        res = read_char(parser, &byte);
    }
    while (res == SUCCESS && byte != ':');
    if (res < 0)
    {
        return res;
    }

    res = read_byte(parser, &parser->byte_count);
    if (res < 0)
    {
        return res;
    }
    res = read_ushort(parser, &parser->offset);
    if (res < 0)
    {
        return res;
    }
    res = read_byte(parser, &parser->type);
    if (res < 0)
    {
        return res;
    }

    switch (parser->type)
    {
    case REC_DATA:
    case REC_CMDLINE:
    {
        for (int i = 0; i < parser->byte_count; i++)
        {
            res = read_byte(parser, parser->record + i);
            if (res < 0)
            {
                return res;
            }
        }    
        break;
    }
    case REC_END:
    {
        if (parser->byte_count != 0)
        {
            return ERR_INVAL_COUNT;
        }
        break;
    }
    case REC_XSEG:
    case REC_XADDR:
    {
        uint16_t base_addr;
            
        if (parser->byte_count != 2)
        {
            return ERR_INVAL_COUNT;
        }

        res = read_ushort(parser, &base_addr);
        if (res < 0)
        {
            return res;
        }
        parser->base_addr = (uint32_t) base_addr << (parser->type == REC_XSEG ? 4 : 16);
        break;
    }
    case REC_SSEG:
    case REC_SADDR:
    {
        if (parser->byte_count != 4)
        {
            return ERR_INVAL_COUNT;
        }
        
        res = read_uint(parser, &parser->start_addr);
        if (res < 0)
        {
            return res;
        }
        break;
    }
    default:
    {
        return ERR_INVAL_TYPE;
    }
    }

    res = read_byte(parser, &byte);
    if (parser->checksum != 0)
    {
        return ERR_INVAL_CKSUM;
    }

    return SUCCESS;
}

// Prepares argc and argv to be passed to the loaded program.
static void setup_args(char *cmdline, size_t cmdline_len, int *argc, char **argv)
{
    char prev = '\0';

    for (size_t i = 0; i < cmdline_len; i++)
    {
        if (cmdline[i] != '\0' && prev == '\0')
        {
            argv[(*argc)++] = cmdline + i;
        }
        prev = cmdline[i];
    }
}

void loader_main(uint32_t r0, uint32_t r1, void *atags)
{
    (void) r0;
    (void) r1;
    struct parser parser;
    size_t cmdline_off = 0;
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
        res = parser_read_record(&parser);
        if (res < 0)
        {
            break;
        }
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
