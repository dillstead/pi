#include <stdbool.h>

bool isdigit(char c)
{
    return c >= '0' && c <= '9';
}

int atoi(const char *str)
{
    int num = 0;
    bool neg = false;

    if (*str == '-')
    {
        str++;
        neg = true;
    }
    while (isdigit(*str))
    {
        num *= 10;
        num += *str - '0';
        str++;
    }
    return neg ? -num : num;
}

char *itoa (int num, char *str)
{
    char *start = str;
    int tmp;
    bool neg = false;

    if (num < 0)
    {
        neg = true;
        num = -num;
        start++;
    }
    tmp = num;
    do
    {
        tmp /= 10;
        start++;
    } while (tmp > 0);
    *start-- = '\0';
    do
    {
        *start-- = num % 10 + '0';
        num /= 10;
    } while (num > 0);
    if (neg)
    {
        *start = '-';
    }
    return str;
}
