#include "interrupts.h"

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


.globl interrupt_handler
.equ SVC_MODE, 0x13
        
        // Taken from the ARM Cortex-A Series Programmer's Guide 12.1.3 - Nested Interrupts
interrupt_handler:
        // The PC is 4 bytes ahead of the instruction that was interrupted
        sub lr, lr, #4
        // Save the LR and SPSR on the SVC stack and immediately switch to
        // SVC mode to handle the interrupt.
        srsfd   sp!, #(SVC_MODE)
        cps     #(SVC_MODE)
        // Store the caller-saved registers as per the ARM-AAPC as they
        // will get clobbered by the handler function.
        push    { r0-r3, r12 }
        // Ensure stack is double word aligned before calling a function.
        and     r1, sp, #4
        sub     sp, sp, r1
        
        push    { r1, lr }
        bl      handle_interrupt

        // Interrupts will be left disabled by irq_handler.
        // Restore SVC registers and stack.
        pop     { r1, lr }
        add     sp, sp, r1
        pop     { r0 - r3, r12 }
        // Resume excecution at the point of interruption.
        rfefd   sp!
