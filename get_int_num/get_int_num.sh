#!/bin/bash
#gcc -Werror -std=c99 -Wall -Wextra -Wno-error=unused-parameter -Wno-error=unused-function -Wno-error=unused-variable -Wconversion -Wno-error=sign-conversion -fsanitize=address,undefined -g3 -o get_int_num main.c get_int_num.S
gcc -Werror -std=c99 -Wall -Wextra -O2 -o get_int_num main.c get_int_num.S

#PRELOAD_LIB="/lib/arm-linux-gnueabihf/libasan.so.6"
PRELOAD_LIB=""

# Set the LD_PRELOAD variable to preload your library
LD_PRELOAD="$PRELOAD_LIB" ~/Projects/pi/get_int_num/get_int_num
