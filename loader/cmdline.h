#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdlib.h>
#include <sys/types.h>
#include <inttypes.h>

ssize_t flatten_cmdline(char **argv, int argc, char *cmdline, size_t cmdline_sz);
size_t cmdline_encode(char *cmdline, size_t cmdline_remain, uint8_t *record, size_t *rlen);

#endif
