#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "led.h"

void echo_main(uint32_t r0, uint32_t r1, void *atags)
{
    (void) r0;
    (void) r1;
    (void) atags;
    uint8_t c;

    led_init();
    uart_init();
    led_on();
    while (true)
    {
        c = uart_read_byte();
        uart_write_byte(c);
        if (c == 'q')
        {
            break;
        }
    }
    led_off();
}
