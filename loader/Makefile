CC = $(ARM_TOOLCHAIN)-gcc
GDB = $(ARM_TOOLCHAIN)-gdb
OBJDUMP = $(ARM_TOOLCHAIN)-objdump
OBJCOPY = $(ARM_TOOLCHAIN)-objcopy
CFLAGS = -mcpu=arm1176jzf-s -ffreestanding -g -Wall -Wextra -Wno-unused-value -mgeneral-regs-only
LDFLAGS = -nostdlib -nostartfiles -lgcc
SRCS = $(filter-out upload.c cmdline.c, $(wildcard *.c))
ASM_SRCS = $(wildcard *.S)
HDRS = $(wildcard *.h)
OBJS = $(patsubst %.c, %.o, $(SRCS))
OBJS += $(patsubst %.S, %.o, $(ASM_SRCS))
LOADER_OBJS = 
INCS = -I.
IMG_NAME=kernel7

all: upload loader blink

loader: $(OBJS) $(HDRS)
	$(CC) -T loader.ld -o $(IMG_NAME).elf $(filter-out blink.o, $(OBJS)) $(LDFLAGS)
	$(OBJCOPY) $(IMG_NAME).elf -O binary $(IMG_NAME).img
	$(OBJDUMP) -D -S $(IMG_NAME).elf > $(IMG_NAME).asm

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@ 

%.o: %.S
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm *.img *.elf *.asm *.hex *.o upload

upload: upload.c cmdline.c
	gcc -Werror -std=c99 -O2 -Wall -Wextra -o upload upload.c cmdline.c -lbsd

blink: blink.S
	$(CC) -T blink.ld -o blink.elf blink.o $(LDFLAGS)
	$(OBJCOPY) blink.elf -O binary blink.img
	$(OBJCOPY) blink.elf -O ihex blink.hex
