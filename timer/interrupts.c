#include <stdio.h>
#include <assert.h>
#include "mmio.h"
#include "interrupts.h"

extern int get_interrupt_num(void);

static void (*process_interrupts[MAX_INTERRUPTS])(void);
static void (*clear_interrupts[MAX_INTERRUPTS])(void);

void interrupts_init(void)
{
    // Disable all interrupts.
    mmio_write(INT_IRQ0_DIS, 0xFFFFFFF);
    mmio_write(INT_IRQ1_DIS, 0xFFFFFFF);
    mmio_write(INT_IRQ2_DIS, 0xFFFFFFF);
}

void register_process_interrupt(int interrupt_num, void (*process_interrupt)(void))
{
    uint32_t enable;
    
    ASSERT(!process_interrupts[interrupt_num]);
    
    process_interrupts[interrupt_num] = process_interrupt;
    // Enable the particular interrupt.
    if (interrupt_num < INT_IRQ2_BASE)
    {
        enable = mmio_read(INT_IRQ1_ENB);
        enable |= 1 << interrupt_num;
        mmio_write(INT_IRQ1_ENB, enable);
    }
    else if (interrupt_num < INT_IRQ0_BASE)
    {
        enable = mmio_read(INT_IRQ2_ENB);
        enable |= 1 << (interrupt_num - INT_IRQ2_BASE);
        mmio_write(INT_IRQ2_ENB, enable);
    }
    else
    {
        enable = mmio_read(INT_IRQ0_ENB);
        enable |= 1 << (interrupt_num - INT_IRQ0_BASE);
        mmio_write(INT_IRQ0_ENB, enable);
    }
}

void register_clear_interrupt(int interrupt_num, void (*clear_interrupt)(void))
{
    ASSERT(!clear_interrupts[interrupt_num]);
    
    clear_interrupts[interrupt_num] = clear_interrupt;
}

void interrupt_handler(void)
{
    // At this point, interrupts are disabled.
    num = get_interrupt_num();
    if (num >= MAX_INTERRUPTS)
    {
        return;
    }
    // Clear the source of the interrupt or else it will fire again
    // as soon as interrupts are enabled.
    if (clear_interrupts[num])
    {
        clear_interrupts[num]();
    }
    // Enable interrupts during the processing of the handler.
    //enable_interrupts();
    if (process_interrupts[num])
    {
        process_interrupts[num]();
    }
    //disable_interrupts();
}
