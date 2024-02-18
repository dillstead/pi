#ifndef __UART_H
#define __UART_H

#include <stdint.h>

void uart_init(void);
uint8_t uart_read_byte(void);
void uart_write_byte(uint8_t value);
#endif
