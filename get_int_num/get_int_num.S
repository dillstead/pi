#include "interrupt.h"

.section ".text"

.globl get_interrupt_num
        
// Taken from BCM2835 ARM Peripherals Example 7.4 Interrupt priority      
get_interrupt_num:
        push    { r4-r10, fp, lr }
        mov     r0, #(INT_IRQ0_BASE + 31)

        ldr     r1, =INT_IRQ0_PEND
        ldr     r2, [r1]
        // Save bits 8 and 9, which, if set indicate that there is
        // one or more pending bits set in IRQ1_PEND and IRQ2_PEND
        // respectively.
        and     r3, r2, #0x300
        // Clear bits 8 and 9 and if the result is non-zero it indicates
        // at least one pending interrupt in IRQ0_PEND, extract and return it.
        bics    r2, r2, #0x300
        bne     get_bit

        // No pending interrupts in IRQ0_PEND.
        tst     r3, #0x100
        // If bit 8 is set, there's at least one pending bit in IRQ1_PEND,
        // extract and return it.
        movne   r0, #(INT_IRQ1_BASE + 31)
        ldrne   r1, =INT_IRQ1_PEND
        ldrne   r2, [r1]
        // Mask out the interrupts that are also present in IRQ0_PEND.
        // If one of those interrupts is processed here, it results in the
        // wrong interrupt number being returned.  
        bicne   r2, #((1 << 7) | (1 << 9) | (1 << 10))
        bicne   r2, #((1 << 18) | (1 << 19))
        bne     get_bit

        // No pending interrupts in IRQ1_PEND.
        tst     r3, #0x200
        // If bit 9 is set, there's at least one pending bit in IRQ2_PEND,
        // extract and return it.
        movne   r0, #(INT_IRQ2_BASE + 31)
        ldrne   r1, =INT_IRQ2_PEND
        ldrne   r2, [r1]
        // Mask out the interrupts that are also present in IRQ0_PEND
        bicne   r2, #((1 << 21) | (1 << 22) | (1 << 23) | (1 << 24) | (1 << 25))
        bicne   r2, #(1 << 30)
        beq     no_bit
        
get_bit:
        // Extract the index of the least significant pending interrupt bit.
        // LSB(r2) = 31 - CLZ(r2 ^ r2 - 1)
        sub     r3, r2, #1
        eor     r2, r2, r3
        clz     r3, r2
        sub     r0, r3
        b       done

no_bit:
        mov     r0, #MAX_INTERRUPTS

done:
        pop     { r4-r10, fp, pc }
	