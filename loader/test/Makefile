all: cmdline_test

cmdline_test: cmdline_test.c ../cmdline.c
	gcc -Werror -I.. -std=c99 -g -Wall -Wextra -fsanitize=address,undefined -lbsd -o cmdline_test cmdline_test.c ../cmdline.c

clean:
	rm *.o cmdline_test
