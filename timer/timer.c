#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "interrupts.h"
#include "system_timer.h"
#include "uart.h"
#include "led.h"
#include "utils.h"

static void cb(void *data)
{
    bool led_state = *((bool *) data);
    
    led_state = !led_state;
    led_state ? led_on() : led_off();
}

// When loaded via the loader, argc and argv (if set during load) will be available. 
void timer_main(int argc, char **argv, void *atags)
{
    (void) atags;
    volatile uint32_t tick;
    bool led_state = false;

    if (argc > 1)
    {
        tick = atoi(argv[1]);
    }

    interrupts_init();
    led_init();
    uart_init();
    system_timer_init();
    interrupts_enable();

    system_timer_tick(tick, cb, &led_state);
}
