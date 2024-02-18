##define TEST_BLINK
#ifdef TEST_BLINK

.equ argc, 3
	
.section ".rodata"
argv:
        .word   name
        .word   delay
        .word   times

name:
        .asciz  "piblink"
delay:
        .asciz  "1048576"
times:
        .asciz  "10"

#endif
	
.section ".text"

        // argc is in r0, argv is in r1, and atags is in r2
        // r3 - delay
        // r4 - number of times to blink
        // r5 - str
        // default delay 1048576
        // default number 10
#ifdef TEST_BLINK
        mov     r0, #(argc)
        ldr     r1, =argv
        mov     r2, #0
#endif
        
        // Set defaults
        mov     r3, #1048576
        mov     r4, #10

        cmp     r0, #2
        blt     read_times    
        ldr     r5, [r1, #4]
        bl      atoi
        mov     r3, r7

read_times:
        cmp     r0, #3
        blt     start_blink
        ldr     r5, [r1, #8]
        bl      atoi
        mov     r4, r7

start_blink:
	cmp	r4, #0
	beq	done
	
        // Turn LED on
        // GPIO Output Clear 0
        ldr     r5, =0x20200028
        // On-board LED
	// Pin 16
        mov	r6, #0x10000
        str     r6, [r5]	

	// Delay
        mov     r7, r3
delay_on:  
        subs    r7, r7, #1
        bne     delay_on

	// Turn LED off
        // GPIO Output Set 0
        ldr     r5, =0x2020001C	
        str     r6, [r5]	

	// Delay
        mov     r7, r3
delay_off:  
        subs    r7, r7, #1
        bne     delay_off

	sub	r4, r4, #1
	b	start_blink

done:   
        wfe
        b       done

        
atoi:
        mov     r7, #0
        b       check_end
        // r5 str pointer
        // r6 char
        // r7 val

add_char:
        mov     r8, #10
        mul     r7, r8, r7
        sub     r6, r6, #48
        add     r7, r6, r7
        add     r5, r5, #1
        

check_end:
        ldrb    r6, [r5]
        cmp     r6, #0
        bne     add_char
        mov     pc, lr
