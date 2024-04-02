#include "uart.h"
#include "utils.h"
#include "debug_log.h"

void log_str(const char *str)
{
    do
    {
        uart_write_byte(*str);
    } while (*str++);
}

void log_int(int num)
{
    char str[sizeof num * 3 + 1];
    log_str(itoa(num, str));
}
