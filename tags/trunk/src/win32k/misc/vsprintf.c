/* $Id: vsprintf.c,v 1.5 2000-12-11 06:20:49 bird Exp $
 *
 * vsprintf and sprintf
 *
 * Copyright (c) 1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define ISDIGIT(c) ((c) >= '0' && (c) <= '9')
#define MAX(a, b)  ((a) >= (b) ? (a) : (b))
#define MIN(a, b)  ((a) < (b) ? (a) : (b))

#define NTSF_CAPITAL    0x0001
#define NTSF_LEFT       0x0002
#define NTSF_ZEROPAD    0x0004
#define NTSF_SPECIAL    0x0008
#define NTSF_VALSIGNED  0x0010
#define NTSF_PLUS       0x0020
#define NTSF_BLANK      0x0040

#define INCL_NOAPI

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <stdarg.h>

#include "dev32.h"
#include "sprintf.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static unsigned  _strnlen(const char *psz, unsigned cchMax);
static int       _atoi_skip(const char **ppsz);


/**
 * Read an integer (decimal) and update the string pointer while reading.
 * Do not support sign values.
 * @returns   Integer value.
 * @param     ppsz  Pointer to pointer to string. The string to be converted.
 */
static int _atoi_skip(const char **ppsz)
{
    int iValue = 0;

    while (ISDIGIT(**ppsz))
    {
        iValue *= 10;
        iValue += **ppsz - '0';
        (*ppsz)++;
    }

    return iValue;
}


/**
 * Finds the length of a string up to cchMax.
 * @returns   Length.
 * @param     psz     Pointer to string.
 * @param     cchMax  Max length.
 */
static unsigned _strnlen(const char *psz, unsigned cchMax)
{
    const char *pszC = psz;

    while (cchMax-- > 0 &&  *psz != '\0')
        psz++;

    return psz - pszC;
}


/**
 * Formats a number according to the parameters.
 * @returns   Pointer to next char. (after number)
 * @param     psz            Pointer to output string.
 * @param     lValue         Value
 * @param     uiBase         Number representation base.
 * @param     cchWidth       Width
 * @param     cchPrecision   Precision.
 * @param     fFlags         Flags (NTFS_*).
 */
static char * numtostr(char *psz, long lValue, unsigned int uiBase,
                       signed int cchWidth, signed int cchPrecision,
                       unsigned int fFlags
                       )
{
    char *          achDigits = "0123456789abcdef";
    int             cchValue;
    unsigned long   ul;
    int             i;
    int             j;

#ifdef DEBUG
    if (uiBase < 2 || uiBase > 16)
        return NULL;
#endif
    if (fFlags & NTSF_CAPITAL)
        achDigits = "0123456789ABCDEF";
    if (fFlags & NTSF_LEFT)
        fFlags &= ~NTSF_ZEROPAD;

    /* determin value length */
    cchValue = 0;
    ul = (unsigned long)((fFlags & NTSF_VALSIGNED) && lValue < 0L ? -lValue : lValue);
    do
    {
        cchValue++;
        ul /= uiBase;
    } while (ul > 0);

    i = 0;
    if (fFlags & NTSF_VALSIGNED)
    {
        if (lValue < 0)
        {
            lValue = -lValue;
            psz[i++] = '-';
        }
        else if (fFlags & (NTSF_PLUS | NTSF_BLANK))
            psz[i++] = (char)(fFlags & NTSF_PLUS ? '+' : ' ');
    }

    if (fFlags & NTSF_SPECIAL && (uiBase % 8) == 0)
    {
        psz[i++] = '0';
        if (uiBase == 16)
            psz[i++] = (char)(fFlags & NTSF_CAPITAL ? 'X' : 'x');
    }


    /* width - only if ZEROPAD */
    cchWidth -= i + cchValue;
    if (fFlags & NTSF_ZEROPAD)
        while (--cchWidth >= 0)
        {
            psz[i++] = '0';
            cchPrecision--;
        }
    else if (!(fFlags & NTSF_LEFT) && cchWidth > 0)
    {
        for (j = i-1; j >= 0; j--)
            psz[cchWidth + j] = psz[j];
        for (j = 0; j < cchWidth; j++)
            psz[j] = ' ';
        i += cchWidth;
    }

    psz += i;

    /* percision */
    while (--cchPrecision >= cchValue)
        *psz++ = '0';

    /* write number - not good enough but it works */
    i = -1;
    psz += cchValue;
    do
    {
        psz[i--] = achDigits[lValue % uiBase];
        lValue /= uiBase;
    } while (lValue > 0);

    /* width if NTSF_LEFT */
    if (fFlags & NTSF_LEFT)
        while (--cchWidth >= 0)
            *psz++ = ' ';


    return psz;
}


#pragma info(notrd)
/**
 * Partial vsprintf implementation.
 * @returns   number of
 * @param     pszBuffer   Output buffer.
 * @param     pszFormat   Format string.
 * @param     args        Argument list.
 */
int vsprintf(char *pszBuffer, const char *pszFormat, va_list args)
{
    char *psz = pszBuffer;

    while (*pszFormat != '\0')
    {
        if (*pszFormat == '%')
        {
            pszFormat++;  /* skip '%' */
            if (*pszFormat == '%')    /* '%%'-> '%' */
                *psz++ = *pszFormat++;
            else
            {
                long         lValue;
                unsigned int fFlags = 0;
                int          cchWidth = -1;
                int          cchPrecision = -1;
                unsigned int uiBase = 10;
                char         chArgSize;

                /* flags */
                #pragma info(none)
                while (1)
                #pragma info(restore)
                {
                    switch (*pszFormat++)
                    {
                        case '#':   fFlags |= NTSF_SPECIAL; continue;
                        case '-':   fFlags |= NTSF_LEFT; continue;
                        case '+':   fFlags |= NTSF_PLUS; continue;
                        case ' ':   fFlags |= NTSF_BLANK; continue;
                        case '0':   fFlags |= NTSF_ZEROPAD; continue;
                    }
                    pszFormat--;
                    break;
                }
                /* width */
                if (ISDIGIT(*pszFormat))
                    cchWidth = _atoi_skip(SSToDS(&pszFormat));
                else if (*pszFormat == '*')
                {
                    pszFormat++;
                    cchWidth = va_arg(args, int);
                    if (cchWidth < 0)
                    {
                        cchWidth = -cchWidth;
                        fFlags |= NTSF_LEFT;
                    }
                }

                /* precision */
                if (*pszFormat == '.')
                {
                    pszFormat++;
                    if (ISDIGIT(*pszFormat))
                        cchPrecision = _atoi_skip(SSToDS(&pszFormat));
                    else if (*pszFormat == '*')
                    {
                        pszFormat++;
                        cchPrecision = va_arg(args, int);
                    }
                    if (cchPrecision < 0)
                        cchPrecision = 0;
                }

                /* argsize */
                chArgSize = *pszFormat;
                if (chArgSize != 'l' && chArgSize != 'L' && chArgSize != 'H')
                    chArgSize = 0;
                else
                    pszFormat++;

                /* type */
                switch (*pszFormat++)
                {
                    /* char */
                    case 'c':
                        if (!(fFlags & NTSF_LEFT))
                            while (--cchWidth > 0)
                                *psz++ = ' ';

                        *psz++ = va_arg(args, int);

                        while (--cchWidth > 0)
                            *psz++ = ' ';
                        continue;

                    case 'd': /* signed decimal integer */
                    case 'i':
                        fFlags |= NTSF_VALSIGNED;
                        break;

                    case 'o':
                        uiBase = 8;
                        break;

                    case 'p':
                        fFlags |= NTSF_SPECIAL; /* Note not standard behaviour (but I like it this way!) */
                        uiBase = 16;
                        break;

                    case 's':   /* string */
                    {
                        int   i;
                        int   cchStr;
                        char *pszStr = va_arg(args, char*);

                        if (pszStr < (char*)0x10000)
                            pszStr = "<NULL>";
                        cchStr = _strnlen(pszStr, (unsigned)cchPrecision);
                        if (!(fFlags & NTSF_LEFT))
                            while (--cchWidth >= cchStr)
                                *psz++ = ' ';
                        for (i = cchStr; i > 0; i--)
                            *psz++ = *pszStr++;

                        while (--cchWidth >= cchStr)
                            *psz++ = ' ';
                        continue;
                    }

                    case 'u':
                        uiBase = 10;
                        break;

                    case 'x':
                    case 'X':
                        uiBase = 16;
                        break;

                    default:
                        continue;
                }
                /*-----------------*/
                /* integer/pointer */
                /*-----------------*/
                /* get value */
                if (pszFormat[-1] == 'p')
                    lValue = (signed long)va_arg(args, char *);
                else if (chArgSize == 'l')
                    lValue = va_arg(args, signed long);
                else if (chArgSize == 'h')
                    lValue = va_arg(args, signed short);
                else
                    lValue = va_arg(args, signed int);
                psz = numtostr(psz, lValue, uiBase, cchWidth,
                               cchPrecision, fFlags);
            }
        }
        else
            *psz++ = *pszFormat++;
    }
    *psz = '\0';

    return psz - pszBuffer;
}


/**
 * sprintf - wraps arguments into a vsprintf call.
 * @returns   number of bytes written.
 * @param     pszBuffer  Pointer to output buffer.
 * @param     pszFormat  Pointer to format string.
 * @param     ...        Optional parameters.
 */
int sprintf(char *pszBuffer, const char *pszFormat, ...)
{
    int     cch;
    va_list arguments;

    va_start(arguments, pszFormat);
    cch = vsprintf(pszBuffer, pszFormat, arguments);
    va_end(arguments);

    return cch;
}

