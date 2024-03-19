#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "interrupt.h"

uint32_t INT_IRQ0_PEND;
uint32_t INT_IRQ1_PEND;
uint32_t INT_IRQ2_PEND;

extern uint32_t get_interrupt_num(void);

int main(void)
{
    // No bits set
    assert(get_interrupt_num() == MAX_INTERRUPTS);
    // Set bit in INT_IRQ0_PEND
    INT_IRQ0_PEND = 0x04;
    assert(get_interrupt_num() == 66);
    // Set bit in INT_IRQ1_PEND
    INT_IRQ0_PEND = 0x100;
    INT_IRQ1_PEND = 0x04;
    assert(get_interrupt_num() == 2);
    // Set bit in INT_IRQ2_PEND
    INT_IRQ0_PEND = 0x200;
    INT_IRQ1_PEND = 0;
    INT_IRQ2_PEND = 0x04;
    assert(get_interrupt_num() == 34);
    // Set bits in INT_IRQ1_PEND and INT_IRQ2_PEND
    INT_IRQ0_PEND = 0x300;
    INT_IRQ1_PEND = 0x04;
    INT_IRQ2_PEND = 0x04;
    assert(get_interrupt_num() == 2);
    // Set bits in INT_IRQ0_PEND, INT_IRQ1_PEND and INT_IRQ2_PEND
    INT_IRQ0_PEND = 0x304;
    INT_IRQ1_PEND = 0x04;
    INT_IRQ2_PEND = 0x04;
    assert(get_interrupt_num() == 66);
    return EXIT_SUCCESS;
}
