/* $Id: d16crt.c,v 1.2 2002-12-16 02:24:28 bird Exp $
 *
 * Moved all the crt replacements used by the 16-bit part into one file.
 *
 * Copyright (c) 1998-2003 knut st. osmundsen <bird@anduin.net>
 *
 *
 * This file is part of kKrnlLib.
 *
 * kKrnlLib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * kKrnlLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with kKrnlLib; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#include <os2.h>
#include "d16crt.h"


/**
 * Quick implementation of fopen.
 * @param    pszFilename   name of file to open (sz)
 * @param    pszIgnored    whatever - it is ignored
 * @return   Handle to file. (not pointer to a FILE stream as in C-library)
 * @remark   binary and readonly is assumed!
 */
HFILE fopen(const char * pszFilename, const char * pszIgnored)
{
    HFILE   hFile = (HFILE)0xffff;
    USHORT  rc;
    unsigned short Action = 0;

    rc = DosOpen(
        (char*)pszFilename,
        &hFile,
        &Action,
        0,
        FILE_NORMAL,
        OPEN_ACTION_FAIL_IF_NEW | OPEN_ACTION_OPEN_IF_EXISTS,
        OPEN_SHARE_DENYNONE | OPEN_ACCESS_READONLY | OPEN_FLAGS_NOINHERIT,
        NULL);

    pszIgnored = pszIgnored;
    if (rc != NO_ERROR)
       hFile = 0;
    return hFile;
}


/**
 * fread emulation
 * @returns   Number of blocks read.
 * @param     pvBuffer  Buffer to read into
 * @param     cbBlock   Blocksize
 * @param     cBlock    Block count
 * @param     hFile     Handle to file (HFILE)
 */
int fread(void * pvBuffer, USHORT cbBlock, USHORT cBlock,  HFILE hFile)
{
    USHORT  ulRead;
    USHORT  rc;

    rc = DosRead(hFile, pvBuffer, (USHORT)(cbBlock*cBlock), &ulRead);
    if (rc == 0)
        rc = (USHORT)cBlock;
    else
        rc = 0;

    return rc;
}


/**
 * fseek emulation
 * @returns   Same as DosChgFilePtr
 * @param     hFile   Filehandle
 * @param     off     offset into file from origin
 * @param     org     origin
 */
int fseek(HFILE hFile, signed long off, int iOrg)
{
    ULONG  ul;
    return  (int)DosChgFilePtr(hFile, off, iOrg, &ul);
}


/**
 * Get filesize in bytes.
 * @returns   Filesize.
 * @param     hFile   Filehandle
 * @remark    This function sets the file position to end of file.
 */
unsigned long fsize(HFILE hFile)
{
    USHORT rc;
    ULONG  cb;

    rc = DosChgFilePtr(hFile, 0, FILE_END, &cb);

    return cb;
}


/**
 * feof emulation
 * @returns   EOF (-1) if end-of-file reached, otherwise returns 0
 * @param     hFile   Filehandle
 */
int feof(HFILE hFile)
{
    char     achBuffer[1];
    ULONG    ulActual = 0;
    USHORT   usActual = 0;
    USHORT   rc;

    rc = DosRead(hFile, (PVOID)&achBuffer[0], 1, &usActual);
    if (rc == ERROR_NO_DATA || usActual == 0)
        return -1;

    rc = DosChgFilePtr(hFile, -1, FILE_CURRENT, &ulActual);
    return 0;
}


/**
 * fgets emulation - slow!
 * @returns   pointer to the read string, or NULL if failed
 * @param     pszBuf  Pointer to the string buffer to read line into.
 * @param     cchBuf  String buffer size.
 * @param     hFile   Filehandle.
 */
char * fgets(char * pszBuf, int cchBuf, HFILE hFile)
{
    char *  psz = pszBuf;
    USHORT  usActual;

    if (cchBuf <= 1 || feof(hFile))
        return NULL;

    cchBuf--;                           /* terminator */
    do
    {
        if (DosRead(hFile, psz, 1, &usActual))
        {
            if (psz == pszBuf)
                return NULL;
            *psz = '\0';
            break;
        }
    } while (--cchBuf && *psz != '\n' && ++psz);

    /*  "\r\n" -> "\n" */
    if (pszBuf < psz && *psz == '\n' && psz[-1] == '\r')
        *(--psz) = '\n';
    psz[1] = '\0';

    return pszBuf;
}

#if 0 /* not in use */
/**
 * kmemcpy - memory copy - slow!
 * @param     psz1  target
 * @param     psz2  source
 * @param     cch length
 */
void *   kmemcpy(char *psz1, const char *psz2, int cch)
{
    while (cch-- != 0)
        *psz1++ = *psz2++;

    return psz1;
}
#endif


/**
 * Finds psz2 in psz1.
 * @returns   Pointer to occurence of psz2 in psz1.
 * @param     psz1  String to be search.
 * @param     psz2  Substring to search for.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
char *kstrstr(const char *psz1, const char *psz2)
{
    while (*psz1 != '\0')
    {
        int i = 0;
        while (psz2[i] != '\0' && psz1[i] == psz2[i])
            i++;

        /* found it? */
        if (psz2[i] == '\0')
            return (char*)psz1;
        if (psz1[i] == '\0' )
            break;
        psz1++;
    }

    return NULL;
}


#if 0 /* not in use */
/**
 * kstrcmp - String compare
 * @returns   0 - equal else !0
 * @param     psz1  String 1
 * @param     psz2  String 2
 */
int      kstrcmp(const char *psz1, const char *psz2);
{
    while (*psz1 == *psz2 && *psz1 != '\0' && *psz2 != '\0')
    {
        psz1++;
        psz2++;
    }
    return *psz1 - *psz2;
}
#endif


/**
 * kstrncmp - String 'n' compare.
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 * @param     len length
 */
int      kstrncmp(const char *psz1, const char *psz2, int cch)
{
    int i = 0;
    while (i < cch && *psz1 == *psz2 && *psz1 != '\0' && *psz2 != '\0')
    {
        psz1++;
        psz2++;
        i++;
    }

    return i - cch;
}


#if 0 /* not in use */
/**
 * kstrnicmp - String 'n' compare, case-insensitive.
 * @returns   0 - equal else !0
 * @param     p1  String 1
 * @param     p2  String 2
 * @param     len length
 */
int      kstrnicmp(const char *psz1, const char *psz2, int cch)
{
    char ch1, ch2;

    do
    {
        ch1 = *psz1++;
        if (ch1 >= 'A' && ch1 <= 'Z')
            ch1 += 'a' - 'A';           /* to lower case */
        ch2 = *psz2++;
        if (ch2 >= 'A' && ch2 <= 'Z')
            ch2 += 'a' - 'A';           /* to lower case */
    } while (--cch > 0 && ch1 == ch2 && ch1 != '\0' && ch2 != '\0');

    return ch1 - ch2;
}
#endif


/**
 * kstrlen - String length.
 * @returns   Length of the string.
 * @param     psz  Pointer to string.
 * @status    completely implemented and tested.
 * @author    knut st. osmundsen
 */
int kstrlen(const char * psz)
{
    int cch = 0;
    while (*psz++ != '\0')
        cch++;
    return cch;
}


/**
 * String copy (strcpy).
 * @returns   Pointer to target string.
 * @param     pszTarget  Target string.
 * @param     pszSource  Source string.
 * @author    knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
char * kstrcpy(char * pszTarget, const char * pszSource)
{
    char *psz = pszTarget;

    while (*pszSource != '\0')
        *psz++ = *pszSource++;
    *psz = '\0';
    return pszTarget;
}


/**
 * String 'n' copy
 * @returns   Pointer to target string.
 * @param     pszTarget   Target string.
 * @param     pszSource   Source string.
 * @param     cch         Number of bytes to copy.
 */
char * kstrncpy(char * pszTarget, const char * pszSource, int cch)
{
    char *  psz = pszTarget;

    while (cch-- && *pszSource != '\0')
        *psz++ = *pszSource++;

    if (cch)
        *psz++ = '\0';

    return pszTarget;
}


/**
 * String concatenation
 * @returns   Pointer to target string.
 * @param     pszTarget   Target string.
 * @param     pszSource   String to be appended.
 */
char * kstrcat(char * pszTarget, const char * pszSource)
{
    char   * psz = pszTarget;

    while (*psz != '\0')
        psz++;
    while (*pszSource != '\0')
        *psz++ = *pszSource++;
    *psz = '\0';
    return pszTarget;
}


/**
 * String tokenizer
 * @returns   Pointer to found token or NULL if failed.
 * @param     pszTarget   String to be tokenized.
 * @param     pszToken    Token delimiters string.
 */
char * kstrtok(char * pszTarget, const char * pszToken)
{
    static char * pszPos = NULL;
    char        * pszRet = pszTarget;

    if (pszTarget)
        pszPos = pszTarget;
    else if (!pszPos || *pszPos == '\0')
        return NULL;
    pszRet = pszPos;

    while (*pszPos != '\0')
    {
        const char *pszTokenI;
        for (pszTokenI = pszToken; *pszTokenI; pszTokenI++)
            if (*pszPos == *pszTokenI)
            {
                *pszPos++ = '\0';
                return pszRet;
            }

        pszPos++;
    }
    pszPos = NULL;
    return pszRet;
}


/**
 * Copy an argument to a buffer. Ie. "-K[=|:]c:\os2krnl ....". Supports quotes
 * @returns   Number of chars of pszArg that has been processed.
 * @param     pszTarget  -  pointer to target buffer.
 * @param     pszArg     -  pointer to source argument string.
 * @param     cchMaxlen  -  maximum chars to copy.
 */
int kargncpy(char * pszTarget, const char * pszArg, unsigned cchMaxlen)
{
    int i = 0;
    int fQuote = FALSE;

    /* skip option word/letter */
    while (*pszArg != '\0' && *pszArg != ' ' && *pszArg != ':' &&
           *pszArg != '='  && *pszArg != '-' && *pszArg != '/')
    {
        pszArg++;
        i++;
    }

    if (*pszArg == ' ' || *pszArg == '-' || *pszArg == '/' || *pszArg == '\0')
        return 0;


    do
    {
        pszArg++;
        i++;
    } while (*pszArg != '\0' && *pszArg == ' ');

    /* copy maxlen or less */
    /* check for quotes */
    if (*pszArg == '"')
    {
        fQuote = TRUE;
        pszArg++;
        i++;
    }
    /* copy loop */
    while (cchMaxlen > 1 && (fQuote ? *pszArg != '"' : *pszArg != ' ') && *pszArg != '\0')
    {
        *pszTarget++ = *pszArg++;
        i++;
        cchMaxlen--;
    }

    /* terminate pszTarget */
    pszTarget = '\0';

    return i;
}


/**
 * strchr implementation.
 * @returns Pointer to first occurence of ch in psz.
 * @param   psz     String to search.
 * @param   ch      Char to search for.
 */
const char *kstrchr(const char *psz, int ch)
{
    while (*psz != '\0' && *psz != (char)ch)
        psz++;
    return *psz == (char)ch ? psz : NULL;
}

