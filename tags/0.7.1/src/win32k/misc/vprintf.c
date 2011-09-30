/* $Id: vprintf.c,v 1.10 2001-03-11 16:47:45 bird Exp $
 *
 * vprintf and printf
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



#if defined(RING0) && !defined(R3TST)
    #define COMOUTPUT
#endif
#if !defined(RING0) || defined(R3TST)
    #define DOSWRITEOUTPUT
#endif

#ifndef DOSWRITEOUTPUT
    #define INCL_NOAPI
#else
    #define INCL_DOSPROCESS
    #define INCL_DOSERRORS
#endif

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>

#include <stdarg.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "dev32.h"
#include "vprintf.h"
#ifdef COMOUTPUT
    #include <builtin.h>
#endif
#ifdef RING0
    #include "options.h"
#endif
#include "Yield.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
static char chNewLine = '\n';
static char chReturn  = '\r';

/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
static int       _atoi_skip(const char **ppsz);
static unsigned  _strnlen(const char *psz, unsigned cchMax);
static void      chout(int ch);
static char *    strout(char *psz, signed cchMax);


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
static char * numtostr(long lValue, unsigned int uiBase,
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
            chout('-');
            i++;
        }
        else if (fFlags & (NTSF_PLUS | NTSF_BLANK))
        {
            chout(fFlags & NTSF_PLUS ? '+' : ' ');
            i++;
        }
    }

    if (fFlags & NTSF_SPECIAL && (uiBase % 8) == 0)
    {
        chout('0');
        i++;
        if (uiBase == 16)
        {
            chout(fFlags & NTSF_CAPITAL ? 'X' : 'x');
            i++;
        }
    }


    /* width - only if ZEROPAD */
    cchWidth -= i + cchValue;
    if (fFlags & NTSF_ZEROPAD)
        while (--cchWidth >= 0)
        {
            chout('0');
            cchPrecision--;
        }
    #if 0
    else if (!(fFlags & NTSF_LEFT) && cchWidth > 0)
    {   /* not yet supported! */
        /*
        for (j = i-1; j >= 0; j--)
            psz[cchWidth + j] = psz[j];
        for (j = 0; j < cchWidth; j++)
            psz[j] = ' ';
        i += cchWidth;
        */
    }
    #endif

    /* percision */
    while (--cchPrecision >= cchValue)
        chout('0');

    ul = 1;
    for (i = 1; i < cchValue; i++)
        ul *= uiBase;
    for (i = 0; i < cchValue; i++)
    {
        chout(achDigits[lValue / ul]);
        lValue %= ul;
        ul /= uiBase;
    }

    /* width if NTSF_LEFT */
    if (fFlags & NTSF_LEFT)
        while (--cchWidth >= 0)
            chout(' ');


    return NULL;
}


/**
 * Partial vprintf implementation.
 * This function checks the fLogging flag, and will therefore only write if logging is enabled.
 * @returns   number of
 * @param     pszFormat   Format string.
 * @param     args        Argument list.
 */
int vprintf(const char *pszFormat, va_list args)
{
    #if defined(RING0) && !defined(R3TST)
    if (!options.fLogging)
        return 0;
    #endif
    return vprintf2(pszFormat, args);
}

#pragma info(notrd)
/**
 * Partial vprintf implementation.
 * This function don't check the fLogging flag, and will therefore allways write.
 * @returns   number of
 * @param     pszFormat   Format string.
 * @param     args        Argument list.
 */
int vprintf2(const char *pszFormat, va_list args)
{
    int cch = 0;

    while (*pszFormat != '\0')
    {
        if (*pszFormat == '%')
        {
            if (cch > 0)
            {
                strout((char*)(pszFormat - cch), cch);
                cch = 0;
            }

            pszFormat++;  /* skip '%' */
            if (*pszFormat == '%')    /* '%%'-> '%' */
                chout(*pszFormat++);
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
                                chout(' ');

                        chout(va_arg(args, int));

                        while (--cchWidth > 0)
                            chout(' ');
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
                        int   cchStr;
                        char *pszStr = va_arg(args, char*);

                        if (pszStr < (char*)0x10000)
                            pszStr = "<NULL>";
                        cchStr = _strnlen(pszStr, (unsigned)cchPrecision);
                        if (!(fFlags & NTSF_LEFT))
                            while (--cchWidth >= cchStr)
                                 chout(' ');

                        pszStr = strout(pszStr, cchStr);

                        while (--cchWidth >= cchStr)
                            chout(' ');
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
                numtostr(lValue, uiBase, cchWidth, cchPrecision, fFlags);
            }
        }
        else
        {
            cch++;
            pszFormat++;
        }
    }

    if (cch > 0)
    {
        strout((char*)(pszFormat - cch), cch);
        cch = 0;
    }

    return 0UL;
}

/* stub */
int _vprintfieee(const char *pszFormat, va_list args)
{
    return vprintf(pszFormat, args);
}


/**
 * printf - wraps arguments into a vprintf call.
 * @returns   number of bytes written.
 * @param     pszFormat  Pointer to format string.
 * @param     ...        Optional parameters.
 */
int printf(const char *pszFormat, ...)
{
    int     cch;
    va_list arguments;

    va_start(arguments, pszFormat);
    cch = vprintf(pszFormat, arguments);
    va_end(arguments);

    return cch;
}


/* stub */
int _printfieee(const char *pszFormat, ...)
{
    int     cch;
    va_list arguments;

    va_start(arguments, pszFormat);
    cch = vprintf(pszFormat, arguments);
    va_end(arguments);

    return cch;
}

/* stub */
int _printf_ansi(const char *pszFormat, ...)
{
    int     cch;
    va_list arguments;

    va_start(arguments, pszFormat);
    cch = vprintf(pszFormat, arguments);
    va_end(arguments);

    return cch;
}




/**
 * Writes a char to output device.
 * @param     ch  Char to write.
 * @status    completely
 * @author    knut st. osmundsen
 */
static void chout(int ch)
{
#ifdef DOSWRITEOUTPUT
    ULONG ulWrote;
#endif

    if (ch != '\r')
    {
        if (ch == '\n')
        {
        #ifdef COMOUTPUT
            #pragma info(noeff)
            while (!(_inp(options.usCom + 5) & 0x20));  /* Waits for the port to be ready. */
            _outp(options.usCom, chReturn);             /* Put the char. */
            #pragma info(restore)
        #endif
        #ifdef DOSWRITEOUTPUT
            DosWrite(1, (void*)&chReturn, 1, SSToDS(&ulWrote));
        #endif
        }
    #ifdef COMOUTPUT
        #pragma info(noeff)
        while (!(_inp(options.usCom + 5) & 0x20));  /* Waits for the port to be ready. */
        _outp(options.usCom, ch);                   /* Put the char. */
        #pragma info(restore)
        Yield();
    #endif
    #ifdef DOSWRITEOUTPUT
        DosWrite(1, SSToDS(&ch), 1, SSToDS(&ulWrote));
    #endif
    }
}


/**
 * Write a string to the output device.
 * @returns   pointer end of string.
 * @param     psz     Pointer to the string to write.
 * @param     cchMax  Max count of chars to write. (or until '\0')
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
static char *strout(char *psz, signed cchMax)
{
    int cchYield = 0;
    while (cchMax > 0 && *psz != '\0')
    {
        ULONG cch = 0;
        ULONG ul;

        while (cchMax - cch > 0 && psz[cch] != '\0' && psz[cch] != '\r' && psz[cch] != '\n')
            cch++;

        /* write string part */
    #ifdef COMOUTPUT
        for (ul = 0; ul < cch; ul++)
        {
            #pragma info(noeff)
            while (!(_inp(options.usCom + 5) & 0x20));  /* Waits for the port to be ready. */
            _outp(options.usCom, psz[ul]);              /* Put the char. */
            #pragma info(restore)
        }
    #endif
    #ifdef DOSWRITEOUTPUT
        DosWrite(1, psz, cch, SSToDS(&ul));
    #endif

        /* cr and lf check + skip */
        if (cch < cchMax && (psz[cch] == '\n' || psz[cch] == '\r'))
        {
            if (psz[cch] == '\n')
            {
            #ifdef COMOUTPUT
                #pragma info(noeff)
                while (!(_inp(options.usCom + 5) & 0x20));  /* Waits for the port to be ready. */
                _outp(options.usCom, chReturn);             /* Put the char. */
                while (!(_inp(options.usCom + 5) & 0x20));  /* Waits for the port to be ready. */
                _outp(options.usCom, chNewLine);            /* Put the char. */
                cchYield++;
                #pragma info(restore)
            #endif
            #ifdef DOSWRITEOUTPUT
                DosWrite(1, (void*)&chReturn, 1, SSToDS(&ul));
                DosWrite(1, (void*)&chNewLine, 1, SSToDS(&ul));
            #endif
            }
            cch++;
        }

        /* next */
        psz += cch;
        cchMax -= cch;
        cchYield += cch;
        if (cchYield > 3)
            if (Yield())
                cchYield = 0;
    }
    return psz;
}

