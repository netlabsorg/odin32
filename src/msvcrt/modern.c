/* Odin32 MSVCRT - exports added by later (VC2005+) C runtimes.
 *
 * Bounds-checked "_s" functions, 64-bit time_t variants, aligned allocation,
 * 64-bit integer conversions, errno accessors.  Built on kLIBC so the
 * semantics are the standard C ones; only the MSVCRT specific return
 * conventions are implemented here.
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>

#ifndef EINVAL
#define EINVAL 22
#endif
#define MRT_ERANGE 34
#define MRT_STRUNCATE 80

extern int *MSVCRT__errno(void);
extern unsigned long *MSVCRT_doserrno(void);
extern char MSVCRT__iob[];

typedef long long mrt_i64;
typedef unsigned long long mrt_u64;
typedef unsigned short mrt_wchar;

static int mrt_err(int e)
{
    *MSVCRT__errno() = e;
    return e;
}

/* ------------------------------------------------------------------ */
/* errno accessors                                                     */

int mrt_get_errno(int *p)
{
    if (!p) return mrt_err(EINVAL);
    *p = *MSVCRT__errno();
    return 0;
}

int mrt_set_errno(int v)
{
    *MSVCRT__errno() = v;
    return 0;
}

int mrt_get_doserrno(unsigned long *p)
{
    if (!p) return mrt_err(EINVAL);
    *p = *MSVCRT_doserrno();
    return 0;
}

int mrt_set_doserrno(unsigned long v)
{
    *MSVCRT_doserrno() = v;
    return 0;
}

void *mrt_iob_func(void)
{
    return MSVCRT__iob;
}

/* ------------------------------------------------------------------ */
/* string.h secure variants                                            */

size_t mrt_strnlen(const char *s, size_t max)
{
    size_t n = 0;
    if (!s) return 0;
    while (n < max && s[n]) n++;
    return n;
}

size_t mrt_wcsnlen(const mrt_wchar *s, size_t max)
{
    size_t n = 0;
    if (!s) return 0;
    while (n < max && s[n]) n++;
    return n;
}

int mrt_strcpy_s(char *dst, size_t size, const char *src)
{
    size_t i;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size; i++)
        if (!(dst[i] = src[i])) return 0;
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_strcat_s(char *dst, size_t size, const char *src)
{
    size_t i, j;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size && dst[i]; i++) ;
    if (i == size) { *dst = 0; return mrt_err(EINVAL); }
    for (j = 0; i + j < size; j++)
        if (!(dst[i + j] = src[j])) return 0;
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_strncpy_s(char *dst, size_t size, const char *src, size_t count)
{
    size_t i;
    if (!count && !dst && !size) return 0;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!count) { *dst = 0; return 0; }
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    if (count == (size_t)-1) {            /* _TRUNCATE */
        for (i = 0; i < size - 1 && src[i]; i++) dst[i] = src[i];
        dst[i] = 0;
        return src[i] ? mrt_err(MRT_STRUNCATE) : 0;
    }
    for (i = 0; i < size && i < count; i++)
        if (!(dst[i] = src[i])) return 0;
    if (i == count && i < size) { dst[i] = 0; return 0; }
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_strncat_s(char *dst, size_t size, const char *src, size_t count)
{
    size_t i, j;
    if (!count && !dst && !size) return 0;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!count) return 0;
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size && dst[i]; i++) ;
    if (i == size) { *dst = 0; return mrt_err(EINVAL); }
    if (count == (size_t)-1) {
        for (j = 0; i + j < size - 1 && src[j]; j++) dst[i + j] = src[j];
        dst[i + j] = 0;
        return src[j] ? mrt_err(MRT_STRUNCATE) : 0;
    }
    for (j = 0; i + j < size && j < count; j++)
        if (!(dst[i + j] = src[j])) return 0;
    if (j == count && i + j < size) { dst[i + j] = 0; return 0; }
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_memcpy_s(void *dst, size_t size, const void *src, size_t count)
{
    if (!count) return 0;
    if (!dst) return mrt_err(EINVAL);
    if (!src) { memset(dst, 0, size); return mrt_err(EINVAL); }
    if (size < count) { memset(dst, 0, size); return mrt_err(MRT_ERANGE); }
    memcpy(dst, src, count);
    return 0;
}

int mrt_memmove_s(void *dst, size_t size, const void *src, size_t count)
{
    if (!count) return 0;
    if (!dst || !src) return mrt_err(EINVAL);
    if (size < count) return mrt_err(MRT_ERANGE);
    memmove(dst, src, count);
    return 0;
}

int mrt_wcscpy_s(mrt_wchar *dst, size_t size, const mrt_wchar *src)
{
    size_t i;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size; i++)
        if (!(dst[i] = src[i])) return 0;
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_wcscat_s(mrt_wchar *dst, size_t size, const mrt_wchar *src)
{
    size_t i, j;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size && dst[i]; i++) ;
    if (i == size) { *dst = 0; return mrt_err(EINVAL); }
    for (j = 0; i + j < size; j++)
        if (!(dst[i + j] = src[j])) return 0;
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_wcsncpy_s(mrt_wchar *dst, size_t size, const mrt_wchar *src, size_t count)
{
    size_t i;
    if (!count && !dst && !size) return 0;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!count) { *dst = 0; return 0; }
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    if (count == (size_t)-1) {
        for (i = 0; i < size - 1 && src[i]; i++) dst[i] = src[i];
        dst[i] = 0;
        return src[i] ? mrt_err(MRT_STRUNCATE) : 0;
    }
    for (i = 0; i < size && i < count; i++)
        if (!(dst[i] = src[i])) return 0;
    if (i == count && i < size) { dst[i] = 0; return 0; }
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

int mrt_wcsncat_s(mrt_wchar *dst, size_t size, const mrt_wchar *src, size_t count)
{
    size_t i, j;
    if (!count && !dst && !size) return 0;
    if (!dst || !size) return mrt_err(EINVAL);
    if (!count) return 0;
    if (!src) { *dst = 0; return mrt_err(EINVAL); }
    for (i = 0; i < size && dst[i]; i++) ;
    if (i == size) { *dst = 0; return mrt_err(EINVAL); }
    if (count == (size_t)-1) {
        for (j = 0; i + j < size - 1 && src[j]; j++) dst[i + j] = src[j];
        dst[i + j] = 0;
        return src[j] ? mrt_err(MRT_STRUNCATE) : 0;
    }
    for (j = 0; i + j < size && j < count; j++)
        if (!(dst[i + j] = src[j])) return 0;
    if (j == count && i + j < size) { dst[i + j] = 0; return 0; }
    *dst = 0;
    return mrt_err(MRT_ERANGE);
}

char *mrt_strtok_s(char *str, const char *delim, char **ctx)
{
    char *tok;
    if (!delim || !ctx || (!str && !*ctx)) { mrt_err(EINVAL); return NULL; }
    if (!str) str = *ctx;
    str += strspn(str, delim);
    if (!*str) { *ctx = str; return NULL; }
    tok = str;
    str += strcspn(str, delim);
    if (*str) *str++ = 0;
    *ctx = str;
    return tok;
}

int mrt_strlwr_s(char *s, size_t size)
{
    size_t i;
    if (!s || !size) return mrt_err(EINVAL);
    for (i = 0; i < size && s[i]; i++) s[i] = (char)tolower((unsigned char)s[i]);
    if (i == size) { *s = 0; return mrt_err(EINVAL); }
    return 0;
}

int mrt_strupr_s(char *s, size_t size)
{
    size_t i;
    if (!s || !size) return mrt_err(EINVAL);
    for (i = 0; i < size && s[i]; i++) s[i] = (char)toupper((unsigned char)s[i]);
    if (i == size) { *s = 0; return mrt_err(EINVAL); }
    return 0;
}

/* ------------------------------------------------------------------ */
/* integer <-> string                                                  */

static int fmt_u64(mrt_u64 v, int neg, char *buf, size_t size, int radix)
{
    char tmp[72];
    int n = 0;
    size_t i;
    if (!buf || !size) return mrt_err(EINVAL);
    if (radix < 2 || radix > 36) { *buf = 0; return mrt_err(EINVAL); }
    do { int d = (int)(v % radix); tmp[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10); v /= radix; } while (v);
    if (neg) tmp[n++] = '-';
    if ((size_t)n + 1 > size) { *buf = 0; return mrt_err(MRT_ERANGE); }
    for (i = 0; i < (size_t)n; i++) buf[i] = tmp[n - 1 - i];
    buf[n] = 0;
    return 0;
}

static int fmt_u64w(mrt_u64 v, int neg, mrt_wchar *buf, size_t size, int radix)
{
    char tmp[72];
    size_t i, n;
    int r = fmt_u64(v, neg, tmp, sizeof(tmp), radix);
    if (r) { if (buf && size) *buf = 0; return r; }
    n = strlen(tmp);
    if (!buf || n + 1 > size) { if (buf && size) *buf = 0; return mrt_err(MRT_ERANGE); }
    for (i = 0; i <= n; i++) buf[i] = (unsigned char)tmp[i];
    return 0;
}

int mrt_itoa_s(int v, char *b, size_t size, int radix)
{ return fmt_u64(radix == 10 && v < 0 ? (mrt_u64)(-(mrt_i64)v) : (mrt_u64)(unsigned)v, radix == 10 && v < 0, b, size, radix); }

int mrt_ltoa_s(long v, char *b, size_t size, int radix)
{ return fmt_u64(radix == 10 && v < 0 ? (mrt_u64)(-(mrt_i64)v) : (mrt_u64)(unsigned long)v, radix == 10 && v < 0, b, size, radix); }

int mrt_ultoa_s(unsigned long v, char *b, size_t size, int radix)
{ return fmt_u64(v, 0, b, size, radix); }

int mrt_i64toa_s(mrt_i64 v, char *b, size_t size, int radix)
{ return fmt_u64(radix == 10 && v < 0 ? (mrt_u64)(-v) : (mrt_u64)v, radix == 10 && v < 0, b, size, radix); }

int mrt_ui64toa_s(mrt_u64 v, char *b, size_t size, int radix)
{ return fmt_u64(v, 0, b, size, radix); }

char *mrt_i64toa(mrt_i64 v, char *b, int radix)
{ mrt_i64toa_s(v, b, 65, radix); return b; }

char *mrt_ui64toa(mrt_u64 v, char *b, int radix)
{ mrt_ui64toa_s(v, b, 65, radix); return b; }

int mrt_itow_s(int v, mrt_wchar *b, size_t size, int radix)
{ return fmt_u64w(radix == 10 && v < 0 ? (mrt_u64)(-(mrt_i64)v) : (mrt_u64)(unsigned)v, radix == 10 && v < 0, b, size, radix); }

int mrt_i64tow_s(mrt_i64 v, mrt_wchar *b, size_t size, int radix)
{ return fmt_u64w(radix == 10 && v < 0 ? (mrt_u64)(-v) : (mrt_u64)v, radix == 10 && v < 0, b, size, radix); }

int mrt_ui64tow_s(mrt_u64 v, mrt_wchar *b, size_t size, int radix)
{ return fmt_u64w(v, 0, b, size, radix); }

mrt_wchar *mrt_i64tow(mrt_i64 v, mrt_wchar *b, int radix)
{ mrt_i64tow_s(v, b, 65, radix); return b; }

mrt_wchar *mrt_ui64tow(mrt_u64 v, mrt_wchar *b, int radix)
{ mrt_ui64tow_s(v, b, 65, radix); return b; }

mrt_i64 mrt_atoi64(const char *s)
{ return strtoll(s, NULL, 10); }

mrt_i64 mrt_strtoi64(const char *s, char **end, int radix)
{ return strtoll(s, end, radix); }

mrt_u64 mrt_strtoui64(const char *s, char **end, int radix)
{ return strtoull(s, end, radix); }

mrt_i64 mrt_wtoi64(const mrt_wchar *w)
{
    char tmp[80];
    size_t i;
    for (i = 0; w && w[i] && i < sizeof(tmp) - 1; i++) tmp[i] = w[i] < 128 ? (char)w[i] : '?';
    tmp[i] = 0;
    return strtoll(tmp, NULL, 10);
}

/* ------------------------------------------------------------------ */
/* printf helpers                                                      */

int mrt_vscprintf(const char *fmt, va_list ap)
{
    if (!fmt) { mrt_err(EINVAL); return -1; }
    return vsnprintf(NULL, 0, fmt, ap);
}

int mrt_scprintf(const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = mrt_vscprintf(fmt, ap);
    va_end(ap);
    return r;
}

int mrt_vsprintf_s(char *buf, size_t size, const char *fmt, va_list ap)
{
    int r;
    if (!buf || !size || !fmt) { mrt_err(EINVAL); return -1; }
    r = vsnprintf(buf, size, fmt, ap);
    if (r < 0 || (size_t)r >= size) { *buf = 0; mrt_err(MRT_ERANGE); return -1; }
    return r;
}

int mrt_sprintf_s(char *buf, size_t size, const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = mrt_vsprintf_s(buf, size, fmt, ap);
    va_end(ap);
    return r;
}

/* _snprintf_s(buf, size, count, fmt, ...): count == _TRUNCATE (-1) truncates */
int mrt_vsnprintf_s(char *buf, size_t size, size_t count, const char *fmt, va_list ap)
{
    int r;
    size_t lim;
    if (!buf || !size || !fmt) { mrt_err(EINVAL); return -1; }
    lim = (count == (size_t)-1 || count >= size) ? size : count + 1;
    if (lim > size) lim = size;
    r = vsnprintf(buf, lim, fmt, ap);
    if (r < 0) { *buf = 0; mrt_err(EINVAL); return -1; }
    if ((size_t)r >= lim) {
        if (count == (size_t)-1) return -1;           /* truncated */
        if (count >= size) { *buf = 0; mrt_err(MRT_ERANGE); return -1; }
        return -1;
    }
    return r;
}

int mrt_snprintf_s(char *buf, size_t size, size_t count, const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = mrt_vsnprintf_s(buf, size, count, fmt, ap);
    va_end(ap);
    return r;
}

/* ------------------------------------------------------------------ */
/* aligned allocation: block pointer stored just below the aligned one */

void *mrt_aligned_offset_malloc(size_t size, size_t align, size_t off)
{
    char *raw, *p;
    if (!align || (align & (align - 1))) { mrt_err(EINVAL); return NULL; }
    if (align < sizeof(void *)) align = sizeof(void *);
    raw = malloc(size + align + 2 * sizeof(void *) + off);
    if (!raw) return NULL;
    p = (char *)(((size_t)raw + 2 * sizeof(void *) + off + align - 1) & ~(align - 1)) - off;
    ((void **)p)[-1] = raw;
    ((size_t *)p)[-2] = size;
    return p;
}

void *mrt_aligned_malloc(size_t size, size_t align)
{ return mrt_aligned_offset_malloc(size, align, 0); }

void mrt_aligned_free(void *p)
{
    if (p) free(((void **)p)[-1]);
}

void *mrt_aligned_offset_realloc(void *p, size_t size, size_t align, size_t off)
{
    void *n;
    size_t old;
    if (!p) return mrt_aligned_offset_malloc(size, align, off);
    if (!size) { mrt_aligned_free(p); return NULL; }
    n = mrt_aligned_offset_malloc(size, align, off);
    if (!n) return NULL;
    old = ((size_t *)p)[-2];
    memcpy(n, p, old < size ? old : size);
    mrt_aligned_free(p);
    return n;
}

void *mrt_aligned_realloc(void *p, size_t size, size_t align)
{ return mrt_aligned_offset_realloc(p, size, align, 0); }

/* ------------------------------------------------------------------ */
/* 64-bit time_t variants (kLIBC time_t is 32 bit)                     */

mrt_i64 mrt_time64(mrt_i64 *t)
{
    mrt_i64 r = (mrt_i64)time(NULL);
    if (t) *t = r;
    return r;
}

long mrt_time32(long *t)
{
    long r = (long)time(NULL);
    if (t) *t = r;
    return r;
}

static int to_time_t(const mrt_i64 *t, time_t *out)
{
    if (!t || *t < 0 || *t > 0x7fffffffLL) return 0;
    *out = (time_t)*t;
    return 1;
}

struct tm *mrt_localtime64(const mrt_i64 *t)
{
    time_t x;
    if (!to_time_t(t, &x)) { mrt_err(EINVAL); return NULL; }
    return localtime(&x);
}

struct tm *mrt_gmtime64(const mrt_i64 *t)
{
    time_t x;
    if (!to_time_t(t, &x)) { mrt_err(EINVAL); return NULL; }
    return gmtime(&x);
}

char *mrt_ctime64(const mrt_i64 *t)
{
    time_t x;
    if (!to_time_t(t, &x)) { mrt_err(EINVAL); return NULL; }
    return ctime(&x);
}

mrt_i64 mrt_mktime64(struct tm *tm)
{
    time_t r = mktime(tm);
    return r == (time_t)-1 ? -1LL : (mrt_i64)r;
}

double mrt_difftime64(mrt_i64 a, mrt_i64 b)
{ return (double)(a - b); }

int mrt_localtime64_s(struct tm *out, const mrt_i64 *t)
{
    struct tm *r;
    if (!out || !t) return mrt_err(EINVAL);
    r = mrt_localtime64(t);
    if (!r) { memset(out, 0xff, sizeof(*out)); return EINVAL; }
    *out = *r;
    return 0;
}

int mrt_gmtime64_s(struct tm *out, const mrt_i64 *t)
{
    struct tm *r;
    if (!out || !t) return mrt_err(EINVAL);
    r = mrt_gmtime64(t);
    if (!r) { memset(out, 0xff, sizeof(*out)); return EINVAL; }
    *out = *r;
    return 0;
}

/* ------------------------------------------------------------------ */
/* _l (explicit locale) variants: the locale argument is ignored; the  */
/* runtime only supports the "C" locale.                               */

typedef void *mrt_locale_t;

mrt_locale_t mrt_create_locale(int cat, const char *loc)
{
    (void)cat;
    if (loc && *loc && strcmp(loc, "C") && strcmp(loc, "")) return NULL;
    return (mrt_locale_t)1;
}

void mrt_free_locale(mrt_locale_t l) { (void)l; }
mrt_locale_t mrt_get_current_locale(void) { return (mrt_locale_t)1; }

#define CT_L(name, fn) int mrt_##name(int c, mrt_locale_t l) { (void)l; return fn(c); }
CT_L(isalnum_l, isalnum)
CT_L(isalpha_l, isalpha)
CT_L(iscntrl_l, iscntrl)
CT_L(isdigit_l, isdigit)
CT_L(isgraph_l, isgraph)
CT_L(islower_l, islower)
CT_L(isprint_l, isprint)
CT_L(ispunct_l, ispunct)
CT_L(isspace_l, isspace)
CT_L(isupper_l, isupper)
CT_L(isxdigit_l, isxdigit)
CT_L(tolower_l, tolower)
CT_L(toupper_l, toupper)

int mrt_towlower_l(int c, mrt_locale_t l) { (void)l; return (c >= 'A' && c <= 'Z') ? c + 32 : c; }
int mrt_towupper_l(int c, mrt_locale_t l) { (void)l; return (c >= 'a' && c <= 'z') ? c - 32 : c; }

double mrt_strtod_l(const char *s, char **e, mrt_locale_t l) { (void)l; return strtod(s, e); }
long mrt_strtol_l(const char *s, char **e, int r, mrt_locale_t l) { (void)l; return strtol(s, e, r); }
unsigned long mrt_strtoul_l(const char *s, char **e, int r, mrt_locale_t l) { (void)l; return strtoul(s, e, r); }
mrt_i64 mrt_strtoi64_l(const char *s, char **e, int r, mrt_locale_t l) { (void)l; return strtoll(s, e, r); }
mrt_u64 mrt_strtoui64_l(const char *s, char **e, int r, mrt_locale_t l) { (void)l; return strtoull(s, e, r); }
int mrt_atoi_l(const char *s, mrt_locale_t l) { (void)l; return atoi(s); }
long mrt_atol_l(const char *s, mrt_locale_t l) { (void)l; return atol(s); }
double mrt_atof_l(const char *s, mrt_locale_t l) { (void)l; return atof(s); }
mrt_i64 mrt_atoi64_l(const char *s, mrt_locale_t l) { (void)l; return strtoll(s, NULL, 10); }

int mrt_stricmp_l(const char *a, const char *b, mrt_locale_t l)
{
    (void)l;
    for (;; a++, b++) {
        int x = tolower((unsigned char)*a), y = tolower((unsigned char)*b);
        if (x != y) return x < y ? -1 : 1;
        if (!x) return 0;
    }
}

int mrt_strnicmp_l(const char *a, const char *b, size_t n, mrt_locale_t l)
{
    (void)l;
    for (; n; n--, a++, b++) {
        int x = tolower((unsigned char)*a), y = tolower((unsigned char)*b);
        if (x != y) return x < y ? -1 : 1;
        if (!x) return 0;
    }
    return 0;
}

int mrt_strcoll_l(const char *a, const char *b, mrt_locale_t l) { (void)l; return strcmp(a, b); }

char *mrt_strlwr_l(char *s, mrt_locale_t l)
{
    char *p = s;
    (void)l;
    for (; *p; p++) *p = (char)tolower((unsigned char)*p);
    return s;
}

char *mrt_strupr_l(char *s, mrt_locale_t l)
{
    char *p = s;
    (void)l;
    for (; *p; p++) *p = (char)toupper((unsigned char)*p);
    return s;
}

int mrt_vsnprintf_l(char *buf, size_t n, const char *fmt, mrt_locale_t l, va_list ap)
{
    int r;
    (void)l;
    r = vsnprintf(buf, n, fmt, ap);
    if (r < 0 || (size_t)r >= n) { if (n) buf[n - 1] = 0; return -1; }
    return r;
}

int mrt_snprintf_l(char *buf, size_t n, const char *fmt, mrt_locale_t l, ...)
{
    int r;
    va_list ap;
    va_start(ap, l);
    r = mrt_vsnprintf_l(buf, n, fmt, l, ap);
    va_end(ap);
    return r;
}

int mrt_sprintf_l(char *buf, const char *fmt, mrt_locale_t l, ...)
{
    int r;
    va_list ap;
    (void)l;
    va_start(ap, l);
    r = vsprintf(buf, fmt, ap);
    va_end(ap);
    return r;
}
