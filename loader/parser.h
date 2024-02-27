#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>

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

int read_char(struct parser *parser, uint8_t *value);
int read_byte(struct parser *parser, uint8_t *value);
int read_ushort(struct parser *parser, uint16_t *value);
int read_uint(struct parser *parser, uint32_t *value);
void parser_set_record(struct parser *parser, uint8_t *record, size_t len);
int parser_parse(struct parser *parser);

#endif
