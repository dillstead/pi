#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <inttypes.h>

static int serial_open(const char *devname)
{
    int fd;
    bool success = false;
    struct termios options;

    fd = open(devname, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd == -1)
    {
        fprintf(stderr, "Error opening %s: %s\n", devname,
                strerror(errno));
        goto cleanup;
    }
    
    memset(&options, 0, sizeof options);
    // 115200 bps, 8-bit characters, ignore modem control lines, enable receiver.
    options.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
    // Ignore framing and parity errors.
    options.c_iflag = IGNPAR;
    // Flush data received but not read, written but not transmitted.
    if (tcflush(fd, TCIOFLUSH) == -1)
    {
        fprintf(stderr, "Error flushing terminal: %s\n", strerror(errno));
        goto cleanup;
    }
    if (tcsetattr(fd, TCSANOW, &options) == -1)
    {
        fprintf(stderr, "Error setting terminal options: %s\n", strerror(errno));
        goto cleanup;
    }
    success = true;

cleanup:
    if (!success && fd != -1)
    {
        close(fd);
        fd = -1;
    }
    return fd;
}

static bool read_byte(int fd, uint8_t *val)
{
    ssize_t read_len = 0;

    while (read_len == 0)
    {
        read_len = read(fd, val, sizeof *val);
    }
    if (read_len < 0)
    {
        fprintf(stderr, "Error reading byte: %s\n",
                strerror(errno));
    }
    return read_len > 0;
}

static bool write_byte(int fd, uint8_t val)
{
    ssize_t write_len = 0;
    
    while (write_len == 0)
    {
        write_len = write(fd, &val, sizeof val);
    }
    if (write_len < 0)
    {
        fprintf(stderr, "Error writing byte: %s\n",
                strerror(errno));
    }
    return write_len > 0;
}

int main(int argc, char **argv)
{
    struct termios save;
    struct termios tp;
    bool restore_term;
    uint8_t lc; // local char
    uint8_t rc; // remote char
    int serial_fd = -1;
    int ret = EXIT_FAILURE;
    
    if (argc != 2)
    {
        fprintf(stderr, "Usage: send <dev>\n");
        return EXIT_FAILURE;
    }

    serial_fd = serial_open(argv[1]);
    if (serial_fd == -1)
    {
        goto cleanup;
    }

    if (tcgetattr(STDIN_FILENO, &tp) == -1)
    {
        fprintf(stderr, "Error getting terminal attributes: %s\n",
                strerror(errno));
        goto cleanup;
    }
    save = tp;
    tp.c_lflag &= ~(ECHO | ICANON);
    tp.c_cc[VMIN] = 1;
    tp.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &tp) == -1)
    {
        fprintf(stderr, "Error setting terminal attributes: %s\n",
                strerror(errno));
        goto cleanup;
    }
    restore_term = true;

    while (true)
    {
        rc = 0;
        if (!read_byte(STDIN_FILENO, &lc)
            || !write_byte(serial_fd, lc)
            || !read_byte(serial_fd, &rc))
        {
            goto cleanup;
        }
        if (rc == 'q')
        {
            break;
        }
        if (putchar(rc) == EOF)
        {

            fprintf(stderr, "Error printing char: %s\n",
                    strerror(errno));
            goto cleanup;
        }
        fflush(stdout);
    }
    
    ret = EXIT_SUCCESS;
    
cleanup:
    if (tcflush(STDIN_FILENO, TCIFLUSH) == -1)
    {
        perror(NULL);
        return EXIT_FAILURE;
    }
    if (restore_term
        && tcsetattr(STDIN_FILENO, TCSANOW, &save) == -1)
    {
        perror(NULL);
        return EXIT_FAILURE;
    }
    if (serial_fd != -1)
    {
        close(serial_fd);
    }
    return ret;
}
