#include "gpio.h"
#include "mmio.h"
#include "led.h"

void led_init(void)
{
    uint32_t selector;

    selector = mmio_read(GPFSEL1);
    selector &= ~(7 << 18);
    selector |= 1 << 18;
    mmio_write(GPFSEL1, selector);
}


void led_blink(int times, int32_t interval)
{
    for (int i = 0; i < times; i++)
    {
        led_on();
        delay(interval);
        led_off();
        delay(interval);
    }
}

void led_on(void)
{
    mmio_write(GPCLR0, 1 << 16);
}

void led_off(void)
{
    mmio_write(GPSET0, 1 << 16);
}
