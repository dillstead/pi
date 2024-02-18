#define _POSIX_SOURCE
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <inttypes.h>
#include <bsd/string.h>
#include <limits.h>
#include "cmds.h"
#include "cmdline.h"

static uint32_t record_num = 0;

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

//    printf("Reading byte\n");
    while (read_len == 0)
    {
        read_len = read(fd, val, sizeof *val);
    }
    if (read_len < 0)
    {
        fprintf(stderr, "Error reading byte: %s\n",
                strerror(errno));
    }
//    else
//    {
//        printf("Read byte: %u %c\n", *val, *val);
//    }
    return read_len > 0;
}

static bool read_string(int fd, unsigned char *str)
{
    // Strings are at least one byte end with \0.
    do
    {
        if (!read_byte(fd, str))
        {
            return false;
        }
    } while (*str++ != '\0');
    return true;
}

static bool write_byte(int fd, uint8_t val)
{
    ssize_t write_len = 0;

//    printf("Writing byte\n");
    while (write_len == 0)
    {
        write_len = write(fd, &val, sizeof val);
    }
    if (write_len < 0)
    {
        fprintf(stderr, "Error writing byte: %s\n",
                strerror(errno));
    }
//    printf("Wrote byte: %u %c\n", val, val);
    return write_len > 0;
}

static bool write_record(int fd, const uint8_t *record)
{
    printf("Record %d: %s", ++record_num, record);
    // Records end with \r\n\0, it's not necessary to write the
    // trailing '\0' as each record contains a byte count.
    while (*record != '\0')
    {
        if (!write_byte(fd, *record))
        {
            return false;
        }
        record++;
    }
    return true;
}

static void get_key(void)
{
    struct termios save;
    struct termios tp;
    bool restore_term = false;
    
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

    puts("Press any key to run");
    getchar();

cleanup:
    tcflush(STDIN_FILENO, TCIFLUSH);
    if (restore_term)
    {
        tcsetattr(STDIN_FILENO, TCSANOW, &save);
    }
}

int main(int argc, char **argv)
{
    int serial_fd = -1;
    FILE *in_file = NULL;
    bool is_interactive;
    uint8_t cmd;
    int num_args;
    size_t cmdline_used;
    size_t cmdline_remain;
    size_t len;
    char cmdline[MAX_CMDLINE];
    uint8_t record[MAX_RECORD];
    unsigned char err_msg[MAX_ERRMSG];
    int rc = EXIT_FAILURE;

    if (argc < 3)
    {
        fprintf(stderr, "Usage: upload <dev> <ihex file> [-i] [command line]\n");
        goto cleanup;
    }
    
    serial_fd = serial_open(argv[1]);
    if (serial_fd == -1)
    {
        goto cleanup;
    }

    in_file = fopen(argv[2], "r+");
    if (!in_file)
    {
        fprintf(stderr, "Failed to open %s: %s\n", argv[2],
                strerror(errno));
        goto cleanup;
    }

    // Check for and remove the interactive argument if present.
    is_interactive = false;
    num_args = 0;
    for (int i = 3, j = 3; i < argc; i++)
    {
        if (strcmp(argv[i], "-i") == 0)
        {
            is_interactive = true;
        }
        else
        {
            argv[j] = argv[i];
            num_args++;
            j++;
        }
    }

    // Flatten command line array into a single buffer of NULL-terminated strings.
    cmdline_used = 0;
    cmdline_remain = flatten_cmdline(argv + 3, num_args, cmdline, sizeof cmdline);
    if (cmdline_remain > SSIZE_MAX)
    {
        goto cleanup;
    }

    write_byte(serial_fd, CMD_INIT);
    do
    {
        if (!read_byte(serial_fd, &cmd))
        {
            goto cleanup;
        }
        switch (cmd)
        {
        case CMD_START:
        {
            puts("Uploading...");
            break;
        }
        case CMD_RECORD:
        {
            // Send the command line before the contents of the file.
            if (cmdline_remain > 0)
            {
                len = cmdline_encode(cmdline + cmdline_used, cmdline_remain,
                                     record);
                cmdline_used += len;
                cmdline_remain -= len;
            }
            else
            {
                if (fgets((char *) record, sizeof record, in_file) == NULL)
                {
                    fprintf(stderr, "Error reading from file\n");
                    goto cleanup;
                }
            }
            if (!write_record(serial_fd, record))
            {
                goto cleanup;
            }
            break;
        }
        case CMD_DONE:
        {
            puts("Upload successful");
            break;
        }
        case CMD_ERROR:
        {
            if (!read_string(serial_fd, err_msg))
            {
                fprintf(stderr, "Error reading error message\n");
                goto cleanup;
            }
            fprintf(stderr, "Error uploading record %" PRIu32 ": %s\n", record_num,
                    err_msg);
            break;
        }
        default:
        {
            fprintf(stderr, "Unknown command %d\n", cmd);
            break;
        }
        }
    } while (cmd != CMD_DONE && cmd != CMD_ERROR);
    if (cmd == CMD_DONE)
    {
        if (is_interactive)
        {
            get_key();
        }
        write_byte(serial_fd, CMD_RUN);
        rc = EXIT_SUCCESS;
    }

cleanup:    
    if (in_file)
    {
        fclose(in_file);
    }
    if (serial_fd != -1)
    {
        close(serial_fd);
    }
    return rc;
}
