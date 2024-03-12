#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "uart.h"
#include "led.h"

// When loaded via the loader, argc and argv (if set during load) will be available. 
void echo_main(int argc, char **argv, void *atags)
{
    (void) argc;
    (void) argv;
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
