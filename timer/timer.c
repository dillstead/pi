#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "interrupts.h"
#include "system_timer.h"
#include "led.h"

static uint32_t str_to_uint(char *num)
{
    int res = 0;
    int power = 0;
    int digit;
    char *start = num;

    while (*num >= '0' && *num <= '9')
    {
        num++;     
    }
    num--;

    while (num != start)
    {
        digit = *num - '0'; 
        for (int i = 0; i < power; i++)
        {
            digit *= 10;
        }
        res += digit;
        power++;
        num--;
    }

    return res;
}
    
volatile uint32_t led_delay;

// When loaded via the loader, argc and argv (if set during load) will be available. 
void timer_main(int argc, char **argv, void *atags)
{
    (void) atags;

    if (argc > 1)
    {
        led_delay = str_to_uint(argv[1]);
    }

    led_init();
    interrupts_init();
    system_timer_init();
    system_timer_set(led_delay);
    interrupts_enable();
}
