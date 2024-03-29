// To keep this in the first portion of the binary.
.section ".text.boot"

// Make _start global.
.globl  _start

// Entry point for the kernel.
// r15 -> should begin execution at 0x8000.
// r0 -> 0x00000000
// r1 -> 0x00000C42
// r2 -> 0x00000100 - start of ATAGS
// preserve these registers as argument for kernel_main
_start:
        // Setup the stack.  Stack will work down from the start of the kernel.
        ldr     sp, =__start

	// Move exception vectors to low memory
	push	{r0-r2}
	ldr     r0, =exception_vector
	mov     r1, #0x0000
	ldmia	r0!, {r2-r9}
	stmia   r1!, {r2-r9}
	ldmia   r0!, {r2-r8}
	stmia   r1!, {r2-r8}
	pop	{r0-r2}

        // Clear out bss.
        ldr     r4, =__bss_start
        ldr     r9, =__bss_end
        mov     r5, #0
        mov     r6, #0
        mov     r7, #0
        mov     r8, #0
        b       2f

1:
        // store multiple at r4.
        stmia   r4!, {r5-r8}

        // If we are still below bss_end, loop.
2:
        cmp     r4, r9
        blo     1b

        // Call main
	mov     r2, #0x100
        bl      timer_main
halt:
        wfe
        b       halt
