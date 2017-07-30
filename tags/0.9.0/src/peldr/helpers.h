/** @file
 *
 * Helper functions for the PE loader.
 *
 * These are needed since we don't link to LIBC.
 *
 * Project Odin Software License can be found in LICENSE.TXT
 */

#ifndef HELPERS_H
#define HELPERS_H

#include <stdarg.h>

void *memcpy(void *p1, const void *p2, int n);
char *strcpy(char *s1, const char *s2);
int strlen(const char *s);
char *strcat(char *s1, const char *s2);
int str_starts_with(const char *s1, const char *s2);
const char *str_skip_char(const char *s, int c);
const char *str_find_char(const char *s, int c);
const char *str_find_last_char(const char *s, int c);
int vsnprintf (char *buf, int n, const char *fmt, va_list args);
int snprintf(char *buf, int n, const char *fmt, ...);

#endif // HELPERS_H
