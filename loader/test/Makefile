all: cmdline_test

cmdline_test: cmdline_test.c ../parser.c ../utils.c ../cmdline.c
	gcc -Werror -I.. -DTEST -std=c99 -g -Wall -Wextra -fsanitize=address,undefined -lbsd -o cmdline_test cmdline_test.c ../parser.c ../utils.c ../cmdline.c

clean:
	rm *.o cmdline_test
