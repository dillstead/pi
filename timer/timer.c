#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#inlcude "interrupts.h"
#include "led.h"

// When loaded via the loader, argc and argv (if set during load) will be available. 
void timer_main((int argc, char **argv, void *atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    led_init();
    interrupts_init();
    system_timer_init();
    interrupts_enable();
}
