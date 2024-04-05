.section ".text"

.globl exception_vector
	
exception_vector:
        ldr 	pc, reset_handler_addr
	ldr 	pc, undefined_instruction_handler_addr
        ldr 	pc, software_interrupt_handler_addr
        ldr 	pc, prefetch_abort_handler_addr
        ldr 	pc, data_abort_handler_addr
        nop                                         // This one is reserved
        ldr 	pc, interrupt_handler_addr
        ldr 	pc, fast_interrupt_handler_addr

reset_handler_addr:                     .word reset_handler
undefined_instruction_handler_addr:     .word undefined_instruction_handler
software_interrupt_handler_addr:        .word software_interrupt_handler
prefetch_abort_handler_addr:            .word prefetch_abort_handler
data_abort_handler_addr:                .word data_abort_handler
interrupt_handler_addr:                 .word interrupt_handler
fast_interrupt_handler_addr:            .word fast_interrupt_handler
