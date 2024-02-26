#include "utils.h"

void *memset(void *dst_, int value, size_t size) 
{
    unsigned char *dst = dst_;

    while (size-- > 0)
    {
        *dst++ = value;
    }
    return dst_;
}

void *memcpy(void *dst_, const void *src_, size_t size) 
{
    unsigned char *dst = dst_;
    const unsigned char *src = src_;

    while (size-- > 0)
    {
        *dst++ = *src++;
    }

    return dst_;
}

unsigned int hex_to_bin(uint8_t hex)
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

// Prepares argc and argv to be passed to the loaded program.
void setup_args(char *cmdline, size_t cmdline_len, int *argc, char **argv)
{
    char prev = '\0';

    *argc = 0;
    for (size_t i = 0; i < cmdline_len; i++)
    {
        if (cmdline[i] != '\0' && prev == '\0')
        {
            argv[(*argc)++] = cmdline + i;
        }
        prev = cmdline[i];
    }
}

