ARM_CC = $(ARM_TOOLCHAIN)-gcc
ARM_OBJDUMP = $(ARM_TOOLCHAIN)-objdump
ARM_OBJCOPY = $(ARM_TOOLCHAIN)-objcopy
CFLAGS =  -mcpu=arm1176jzf-s -ffreestanding -g -Wall -Wextra -Wno-unused-value -mgeneral-regs-only
LDFLAGS = -nostdlib -nostartfiles -lgcc
SRCS = $(filter-out send.c,$(wildcard *.c))
ASM_SRCS = $(wildcard *.S)
HDRS = $(wildcard *.h)
OBJS = $(patsubst %.c, %.o, $(SRCS))
OBJS += $(patsubst %.S, %.o, $(ASM_SRCS))
INCS = -I.
IMG_NAME=kernel7

all:  echo send

echo: $(OBJS) $(HDRS)
	$(ARM_CC) -T linker.ld -o $(IMG_NAME).elf $(OBJS) $(LDFLAGS)
	$(ARM_OBJCOPY) $(IMG_NAME).elf -O binary $(IMG_NAME).img
	$(ARM_OBJCOPY) $(IMG_NAME).elf -O ihex $(IMG_NAME).hex
	$(ARM_OBJDUMP) -d -S $(IMG_NAME).elf > $(IMG_NAME).asm

send: send.c
	$(CC) -std=c99 -Werror -std=c99 -Wall -Wextra -O2 -o send send.c

%.o: %.c
	$(ARM_CC) $(CFLAGS) $(INCS) -c $< -o $@ 

%.o: %.S
	$(ARM_CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm *.img *.elf *.asm *.hex *.o send
