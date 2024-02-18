#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

void *memset(void *dst_, int value, size_t size);
void *memcpy(void *dst_, const void *src_, size_t size);
unsigned int hex_to_bin(uint8_t hex);
void setup_args(char *cmdline, size_t cmdline_len, int *argc, char **argv);

#endif
