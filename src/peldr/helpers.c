/** @file
 *
 * Helper functions for the PE loader.
 *
 * These are needed since we don't link to LIBC.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#include "helpers.h"

#ifndef NULL
#define NULL 0
#endif

void *memcpy(void *p1, const void *p2, int n)
{
    char *p = (char *)p1;
    while (n)
    {
        *p++ = *(char *)(p2++);
        n--;
    }
    return p1;
}

char *strcpy(char *s1, const char *s2)
{
    char *s = s1;
    while (*s2)
        *s++ = *s2++;
    *s = '\0';
    return s1;
}

int strlen(const char *s)
{
    const char *s2 = s;
    while (*s2)
        s2++;
    return s2 - s;
}

char *strcat(char *s1, const char *s2)
{
    char *s = s1;
    while (*s)
        s++;
    strcpy(s, s2);
    return s1;
}

int str_starts_with(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return !*s2;
}

const char *str_skip_char(const char *s, int c)
{
    while (*s && *s == c)
        s++;
    return s;
}

const char *str_find_char(const char *s, int c)
{
    while (*s && *s != c)
        s++;
    return s;
}

const char *str_find_last_char(const char *s, int c)
{
    int len = strlen (s);
    const char *e = s + len;
    while (e >= s && *e != c)
        e--;
    return e < s ? s + len : e;
}

//
// BEGIN: taken from _ras.cpp
//

void ulong2string (unsigned long number, char *string, int n, int base)
{
    static const char *digits = "0123456789ABCDEF";

    unsigned long tmp = number;
    char *s = string;
    int len = 0;
    int l = 0;
    int i;

    if (n <= 0)
    {
        return;
    }

    if (tmp == 0)
    {
        s[l++] = digits[0];
    }

    while (tmp != 0)
    {
        if (l >= n)
        {
            break;
        }
        s[l++] = digits[tmp%base];
        len++;
        tmp /= base;
    }
    if (l < n)
    {
        s[l++] = '\0';
    }

    s = string;

    for (i = 0; i < len/2; i++)
    {
        tmp = s[i];
        s[i] = s[len - i - 1];
        s[len - i - 1] = tmp;
    }

    return;
}

void long2string (long number, char *string, int n, int base)
{
    if (n <= 0)
    {
        return;
    }

    if (number < 0)
    {
        *string++ = '-';
        number = -number;
        n--;
    }

    ulong2string (number, string, n, base);
}

int string2ulong (const char *string, char **pstring2, unsigned long *pvalue, int base)
{
    unsigned long value = 0;
    int sign = 1;

    const char *p = string;

    if (p[0] == '-')
    {
        sign = -1;
        p++;
    }

    if (base == 0)
    {
        if (p[0] == 0 && (p[1] == 'x' || p[1] == 'X'))
        {
            base = 16;
            p += 2;
        }
        else if (p[0] == 0)
        {
            base = 8;
            p += 1;
        }
        else
        {
            base = 10;
        }
    }

    while (*p)
    {
        int digit = 0;

        if ('0' <= *p && *p <= '9')
        {
            digit = *p - '0';
        }
        else if ('a' <= *p && *p <= 'f')
        {
            digit = *p - 'a' + 0xa;
        }
        else if ('A' <= *p && *p <= 'F')
        {
            digit = *p - 'A' + 0xa;
        }
        else
        {
            break;
        }

        if (digit >= base)
        {
            break;
        }

        value = value*base + digit;

        p++;
    }

    if (pstring2)
    {
        *pstring2 = (char *)p;
    }

    *pvalue = sign*value;

    return 1;
}

int vsnprintf(char *buf, int n, const char *fmt, va_list args)
{
    int count = 0;
    char *s = (char *)fmt;
    char *d = buf;

    if (n <= 0)
    {
        return 0;
    }

    n--;

    while (*s && count < n)
    {
        char tmpstr[16];

        char *str = NULL;

        int width = 0;
        int precision = 0;

        if (*s == '%')
        {
            s++;

            if ('0' <= *s && *s <= '9' || *s == '-')
            {
                char setprec = (*s == '0');
                string2ulong (s, &s, (unsigned long *)&width, 10);
                if (setprec)
                {
                    precision = width;
                }
            }

            if (*s == '.')
            {
                s++;
                string2ulong (s, &s, (unsigned long *)&precision, 10);
            }

            if (*s == 's')
            {
                str = va_arg(args, char *);
                s++;
                precision = 0;
            }
            else if (*s == 'c')
            {
                tmpstr[0] = va_arg(args, int);
                tmpstr[1] = 0;
                str = &tmpstr[0];
                s++;
                precision = 0;
            }
            else if (*s == 'p' || *s == 'P')
            {
                int num = va_arg(args, int);

                ulong2string (num, tmpstr, sizeof (tmpstr), 16);

                str = &tmpstr[0];
                s++;
                width = 8;
                precision = 8;
            }
            else
            {
                if (*s == 'l')
                {
                    s++;
                }

                if (*s == 'd' || *s == 'i')
                {
                    int num = va_arg(args, int);

                    long2string (num, tmpstr, sizeof (tmpstr), 10);

                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'u')
                {
                    int num = va_arg(args, int);

                    ulong2string (num, tmpstr, sizeof (tmpstr), 10);

                    str = &tmpstr[0];
                    s++;
                }
                else if (*s == 'x' || *s == 'X')
                {
                    int num = va_arg(args, int);

                    ulong2string (num, tmpstr, sizeof (tmpstr), 16);

                    str = &tmpstr[0];
                    s++;
                }
            }
        }

        if (str != NULL)
        {
            int i;
            char numstr[16];
            int len = strlen (str);
            int leftalign = 0;

            if (width < 0)
            {
                width = -width;
                leftalign = 1;
            }

            if (precision)
            {
                i = 0;
                while (precision > len)
                {
                    numstr[i++] = '0';
                    precision--;
                }

                memcpy (&numstr[i], str, len);

                str = &numstr[0];
                len += i;
            }

            if (len < width && !leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }

                if (count >= n)
                {
                    break;
                }
            }

            i = 0;
            while (i < len && count < n)
            {
                *d++ = str[i++];
                count++;
            }

            if (count >= n)
            {
                break;
            }

            if (len < width && leftalign)
            {
                while (len < width && count < n)
                {
                    *d++ = ' ';
                    width--;
                    count++;
                }

                if (count >= n)
                {
                    break;
                }
            }
        }
        else
        {
            *d++ = *s++;
            count++;
        }
    }

    *d = '\0';

    return count + 1;
}

int snprintf(char *buf, int n, const char *fmt, ...)
{
    va_list args;

    int rc = 0;

    va_start (args, fmt);

    rc = vsnprintf (buf, n, fmt, args);

    va_end (args);

    return rc;
}

//
// END: taken from _ras.cpp
//
