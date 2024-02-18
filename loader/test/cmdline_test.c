#define _POSIX_SOURCE
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <string.h>
#include <stdio.h>
#include "cmds.h"
#include "cmdline.h"

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

static int read_char(struct parser *parser, uint8_t *value)
{
    if (parser->off >= parser->record_len)
    {
        return ERR_EOL;
    }
    *value = parser->record[parser->off++];
    return SUCCESS;
}

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

static void parser_set_record(struct parser *parser, uint8_t *record, size_t len)
{
    memcpy(parser->record, record, len); 
    parser->record_len = len;
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

int main(int argc, char **argv)
{
    size_t cmdline_used;
    size_t cmdline_remain;
    size_t len;
    char cmdline[MAX_CMDLINE];
    uint8_t record[MAX_RECORD];
    struct parser parser = { 0 };
    

    cmdline_used = 0;
    cmdline_remain = flatten_cmdline(argv + 1, argc - 1, cmdline, sizeof cmdline);
    if (cmdline_remain > SSIZE_MAX)
    {
        return EXIT_FAILURE;
    }
    
    while (cmdline_remain > 0)
    {
        len = cmdline_encode(cmdline + cmdline_used, cmdline_remain,
                             record);
        parser_set_record(&parser, record, len);
        
        // Drop CRFL
        *strchr((const char *) record, '\r') = '\0';
        puts((const char *) record);
        cmdline_used += len;
        cmdline_remain -= len;
    }
    puts(":00000001FF");
        
    return EXIT_SUCCESS;
}
