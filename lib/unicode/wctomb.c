/*
 * WideCharToMultiByte implementation
 *
 * Copyright 2000 Alexandre Julliard
 */

#include <string.h>

#include "winnls.h"
#include "wine/unicode.h"

/* search for a character in the unicode_compose_table; helper for compose() */
static inline int binary_search( WCHAR ch, int low, int high )
{
    extern const WCHAR unicode_compose_table[];
    while (low <= high)
    {
        int pos = (low + high) / 2;
        if (unicode_compose_table[2*pos] < ch)
        {
            low = pos + 1;
            continue;
        }
        if (unicode_compose_table[2*pos] > ch)
        {
            high = pos - 1;
            continue;
        }
        return pos;
    }
    return -1;
}

/* return the result of the composition of two Unicode chars, or 0 if none */
static WCHAR compose( const WCHAR *str )
{
    extern const WCHAR unicode_compose_table[];
    extern const unsigned int unicode_compose_table_size;

    int idx = 1, low = 0, high = unicode_compose_table_size - 1;
    for (;;)
    {
        int pos = binary_search( str[idx], low, high );
        if (pos == -1) return 0;
        if (!idx--) return unicode_compose_table[2*pos+1];
        low = unicode_compose_table[2*pos+1];
        high = unicode_compose_table[2*pos+3] - 1;
    }
}


/****************************************************************/
/* sbcs support */

/* check if 'ch' is an acceptable sbcs mapping for 'wch' */
static inline int is_valid_sbcs_mapping( const struct sbcs_table *table, int flags,
                                         WCHAR wch, unsigned char ch )
{
    if (flags & WC_NO_BEST_FIT_CHARS) return (table->cp2uni[ch] == wch);
    if (ch != (unsigned char)table->info.def_char) return 1;
    return (wch == table->info.def_unicode_char);
}

/* query necessary dst length for src string */
static inline int get_length_sbcs( const struct sbcs_table *table, int flags,
                                   const WCHAR *src, unsigned int srclen )
{
    unsigned int ret = srclen;

    if (flags & WC_COMPOSITECHECK)
    {
        const unsigned char  * const uni2cp_low = table->uni2cp_low;
        const unsigned short * const uni2cp_high = table->uni2cp_high;
        WCHAR composed;

        for (ret = 0; srclen > 1; ret++, srclen--, src++)
        {
            if (!(composed = compose(src))) continue;
            /* check if we should skip the next char */

            /* in WC_DEFAULTCHAR and WC_DISCARDNS mode, we always skip */
            /* the next char no matter if the composition is valid or not */
            if (!(flags & (WC_DEFAULTCHAR|WC_DISCARDNS)))
            {
                unsigned char ch = uni2cp_low[uni2cp_high[composed >> 8] + (composed & 0xff)];
                if (!is_valid_sbcs_mapping( table, flags, composed, ch )) continue;
            }
            src++;
            srclen--;
        }
        if (srclen) ret++;  /* last char */
    }
    return ret;
}

/* wcstombs for single-byte code page */
static inline int wcstombs_sbcs( const struct sbcs_table *table,
                                 const WCHAR *src, unsigned int srclen,
                                 char *dst, unsigned int dstlen )
{
    const unsigned char  * const uni2cp_low = table->uni2cp_low;
    const unsigned short * const uni2cp_high = table->uni2cp_high;
    int ret = srclen;

    if (dstlen < srclen)
    {
        /* buffer too small: fill it up to dstlen and return error */
        srclen = dstlen;
        ret = -1;
    }

    for (;;)
    {
        switch(srclen)
        {
        default:
        case 16: dst[15] = uni2cp_low[uni2cp_high[src[15] >> 8] + (src[15] & 0xff)];
        case 15: dst[14] = uni2cp_low[uni2cp_high[src[14] >> 8] + (src[14] & 0xff)];
        case 14: dst[13] = uni2cp_low[uni2cp_high[src[13] >> 8] + (src[13] & 0xff)];
        case 13: dst[12] = uni2cp_low[uni2cp_high[src[12] >> 8] + (src[12] & 0xff)];
        case 12: dst[11] = uni2cp_low[uni2cp_high[src[11] >> 8] + (src[11] & 0xff)];
        case 11: dst[10] = uni2cp_low[uni2cp_high[src[10] >> 8] + (src[10] & 0xff)];
        case 10: dst[9]  = uni2cp_low[uni2cp_high[src[9]  >> 8] + (src[9]  & 0xff)];
        case 9:  dst[8]  = uni2cp_low[uni2cp_high[src[8]  >> 8] + (src[8]  & 0xff)];
        case 8:  dst[7]  = uni2cp_low[uni2cp_high[src[7]  >> 8] + (src[7]  & 0xff)];
        case 7:  dst[6]  = uni2cp_low[uni2cp_high[src[6]  >> 8] + (src[6]  & 0xff)];
        case 6:  dst[5]  = uni2cp_low[uni2cp_high[src[5]  >> 8] + (src[5]  & 0xff)];
        case 5:  dst[4]  = uni2cp_low[uni2cp_high[src[4]  >> 8] + (src[4]  & 0xff)];
        case 4:  dst[3]  = uni2cp_low[uni2cp_high[src[3]  >> 8] + (src[3]  & 0xff)];
        case 3:  dst[2]  = uni2cp_low[uni2cp_high[src[2]  >> 8] + (src[2]  & 0xff)];
        case 2:  dst[1]  = uni2cp_low[uni2cp_high[src[1]  >> 8] + (src[1]  & 0xff)];
        case 1:  dst[0]  = uni2cp_low[uni2cp_high[src[0]  >> 8] + (src[0]  & 0xff)];
        case 0: break;
        }
        if (srclen < 16) return ret;
        dst += 16;
        src += 16;
        srclen -= 16;
    }
}

/* slow version of wcstombs_sbcs that handles the various flags */
static int wcstombs_sbcs_slow( const struct sbcs_table *table, int flags,
                               const WCHAR *src, unsigned int srclen,
                               char *dst, unsigned int dstlen,
                               const char *defchar, int *used )
{
    const unsigned char  * const uni2cp_low = table->uni2cp_low;
    const unsigned short * const uni2cp_high = table->uni2cp_high;
    const unsigned char table_default = table->info.def_char & 0xff;
    unsigned int len;
    int tmp;
    WCHAR composed;

    if (!defchar) defchar = (const char *)&table_default;
    if (!used) used = &tmp;  /* avoid checking on every char */

    for (len = dstlen; srclen && len; dst++, len--, src++, srclen--)
    {
        WCHAR wch = *src;

        if ((flags & WC_COMPOSITECHECK) && (srclen > 1) && (composed = compose(src)))
        {
            /* now check if we can use the composed char */
            *dst = uni2cp_low[uni2cp_high[composed >> 8] + (composed & 0xff)];
            if (is_valid_sbcs_mapping( table, flags, composed, *dst ))
            {
                /* we have a good mapping, use it */
                src++;
                srclen--;
                continue;
            }
            /* no mapping for the composed char, check the other flags */
            if (flags & WC_DEFAULTCHAR) /* use the default char instead */
            {
                *dst = *defchar;
                *used = 1;
                src++;  /* skip the non-spacing char */
                srclen--;
                continue;
            }
            if (flags & WC_DISCARDNS) /* skip the second char of the composition */
            {
                src++;
                srclen--;
            }
            /* WC_SEPCHARS is the default */
        }

        *dst = uni2cp_low[uni2cp_high[wch >> 8] + (wch & 0xff)];
        if (!is_valid_sbcs_mapping( table, flags, wch, *dst ))
        {
            *dst = *defchar;
            *used = 1;
        }
    }
    if (srclen) return -1;  /* overflow */
    return dstlen - len;
}


/****************************************************************/
/* dbcs support */

/* check if 'ch' is an acceptable dbcs mapping for 'wch' */
static inline int is_valid_dbcs_mapping( const struct dbcs_table *table, int flags,
                                         WCHAR wch, unsigned short ch )
{
    if (ch == table->info.def_char && wch != table->info.def_unicode_char) return 0;
    if (flags & WC_NO_BEST_FIT_CHARS)
    {
        /* check if char maps back to the same Unicode value */
        if (ch & 0xff00)
        {
            unsigned char off = table->cp2uni_leadbytes[ch >> 8];
            return (table->cp2uni[(off << 8) + (ch & 0xff)] == wch);
        }
        return (table->cp2uni[ch & 0xff] == wch);
    }
    return 1;
}

/* query necessary dst length for src string */
static int get_length_dbcs( const struct dbcs_table *table, int flags,
                            const WCHAR *src, unsigned int srclen,
                            const char *defchar )
{
    const unsigned short * const uni2cp_low = table->uni2cp_low;
    const unsigned short * const uni2cp_high = table->uni2cp_high;
    WCHAR defchar_value = table->info.def_char;
    WCHAR composed;
    int len;

    if (!defchar && !(flags & WC_COMPOSITECHECK))
    {
        for (len = 0; srclen; srclen--, src++, len++)
        {
            if (uni2cp_low[uni2cp_high[*src >> 8] + (*src & 0xff)] & 0xff00) len++;
        }
        return len;
    }

    if (defchar) defchar_value = defchar[1] ? ((defchar[0] << 8) | defchar[1]) : defchar[0];
    for (len = 0; srclen; len++, srclen--, src++)
    {
        unsigned short res;
        WCHAR wch = *src;

        if ((flags & WC_COMPOSITECHECK) && (srclen > 1) && (composed = compose(src)))
        {
            /* now check if we can use the composed char */
            res = uni2cp_low[uni2cp_high[composed >> 8] + (composed & 0xff)];

            if (is_valid_dbcs_mapping( table, flags, composed, res ))
            {
                /* we have a good mapping for the composed char, use it */
                if (res & 0xff00) len++;
                src++;
                srclen--;
                continue;
            }
            /* no mapping for the composed char, check the other flags */
            if (flags & WC_DEFAULTCHAR) /* use the default char instead */
            {
                if (defchar_value & 0xff00) len++;
                src++;  /* skip the non-spacing char */
                srclen--;
                continue;
            }
            if (flags & WC_DISCARDNS) /* skip the second char of the composition */
            {
                src++;
                srclen--;
            }
            /* WC_SEPCHARS is the default */
        }

        res = uni2cp_low[uni2cp_high[wch >> 8] + (wch & 0xff)];
        if (!is_valid_dbcs_mapping( table, flags, wch, res )) res = defchar_value;
        if (res & 0xff00) len++;
    }
    return len;
}

/* wcstombs for double-byte code page */
static inline int wcstombs_dbcs( const struct dbcs_table *table,
                                 const WCHAR *src, unsigned int srclen,
                                 char *dst, unsigned int dstlen )
{
    const unsigned short * const uni2cp_low = table->uni2cp_low;
    const unsigned short * const uni2cp_high = table->uni2cp_high;
    int len;

    for (len = dstlen; srclen && len; len--, srclen--, src++)
    {
        unsigned short res = uni2cp_low[uni2cp_high[*src >> 8] + (*src & 0xff)];
        if (res & 0xff00)
        {
            if (len == 1) break;  /* do not output a partial char */
            len--;
            *dst++ = res >> 8;
        }
        *dst++ = (char)res;
    }
    if (srclen) return -1;  /* overflow */
    return dstlen - len;
}

/* slow version of wcstombs_dbcs that handles the various flags */
static int wcstombs_dbcs_slow( const struct dbcs_table *table, int flags,
                               const WCHAR *src, unsigned int srclen,
                               char *dst, unsigned int dstlen,
                               const char *defchar, int *used )
{
    const unsigned short * const uni2cp_low = table->uni2cp_low;
    const unsigned short * const uni2cp_high = table->uni2cp_high;
    WCHAR defchar_value = table->info.def_char;
    WCHAR composed;
    int len, tmp;

    if (defchar) defchar_value = defchar[1] ? ((defchar[0] << 8) | defchar[1]) : defchar[0];
    if (!used) used = &tmp;  /* avoid checking on every char */

    for (len = dstlen; srclen && len; len--, srclen--, src++)
    {
        unsigned short res;
        WCHAR wch = *src;

        if ((flags & WC_COMPOSITECHECK) && (srclen > 1) && (composed = compose(src)))
        {
            /* now check if we can use the composed char */
            res = uni2cp_low[uni2cp_high[composed >> 8] + (composed & 0xff)];

            if (is_valid_dbcs_mapping( table, flags, composed, res ))
            {
                /* we have a good mapping for the composed char, use it */
                src++;
                srclen--;
                goto output_char;
            }
            /* no mapping for the composed char, check the other flags */
            if (flags & WC_DEFAULTCHAR) /* use the default char instead */
            {
                res = defchar_value;
                *used = 1;
                src++;  /* skip the non-spacing char */
                srclen--;
                goto output_char;
            }
            if (flags & WC_DISCARDNS) /* skip the second char of the composition */
            {
                src++;
                srclen--;
            }
            /* WC_SEPCHARS is the default */
        }

        res = uni2cp_low[uni2cp_high[wch >> 8] + (wch & 0xff)];
        if (!is_valid_dbcs_mapping( table, flags, wch, res ))
        {
            res = defchar_value;
            *used = 1;
        }

    output_char:
        if (res & 0xff00)
        {
            if (len == 1) break;  /* do not output a partial char */
            len--;
            *dst++ = res >> 8;
        }
        *dst++ = (char)res;
    }
    if (srclen) return -1;  /* overflow */
    return dstlen - len;
}

/* wide char to multi byte string conversion */
/* return -1 on dst buffer overflow */
_K32CONV int cp_wcstombs( const union cptable *table, int flags,
                 const WCHAR *src, int srclen,
                 char *dst, int dstlen, const char *defchar, int *used )
{
    if (table->info.char_size == 1)
    {
        if (!dstlen) return get_length_sbcs( &table->sbcs, flags, src, srclen );
        if (flags || defchar || used)
            return wcstombs_sbcs_slow( &table->sbcs, flags, src, srclen,
                                       dst, dstlen, defchar, used );
        return wcstombs_sbcs( &table->sbcs, src, srclen, dst, dstlen );
    }
    else /* mbcs */
    {
        if (!dstlen) return get_length_dbcs( &table->dbcs, flags, src, srclen, defchar );
        if (flags || defchar || used)
            return wcstombs_dbcs_slow( &table->dbcs, flags, src, srclen,
                                       dst, dstlen, defchar, used );
        return wcstombs_dbcs( &table->dbcs, src, srclen, dst, dstlen );
    }
}
