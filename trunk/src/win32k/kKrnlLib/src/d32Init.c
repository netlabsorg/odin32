/* $Id: d32Init.c,v 1.3 2002-12-16 01:36:46 bird Exp $
 *
 * d32init.c - 32bit init routins.
 *
 * Copyright (c) 2002 knut st. osmundsen <bird@anduin.net>
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
#ifndef NOFILEID
static const char szFileId[] = "$Id: d32Init.c,v 1.3 2002-12-16 01:36:46 bird Exp $";
#endif


/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#if  0
    #define kprintf2(a) kprintf
#else
    #define kprintf2(a) (void)0
#endif

#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY
#define INCL_OS2KRNL_ALL
#define INCL_OS2KRNL_LDR
#define INCL_KKL_HEAP
#define INCL_KKL_MISC

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <kLib/kLib.h>
#include <kLib/kDevHlp.h>

#include <os2.h>
#include <string.h>

#include "kKrnlLib.h"

#include "options.h"
#include "dev1632.h"
#include "dev32.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
ULONG          readnum(const char *pszNum);

/* Note! these two have to be visible to some assembly programs. */
ULONG _System R0Addr32bit(ULONG ulParam, PFN pfn);
ULONG _System R0Init(RP32INIT *pRpInit);


/**
 * Forwarder for ASMR0Addr32bit which only purpose is to
 * initate a logging sequence.
 * @returns what ever the function returned.
 * @param   ulParam     User argument.
 * @param   pfn         Address (32-bit FLAT) of the function to call.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
ULONG R0Addr32bit(ULONG ulParam, PFN pfn)
{
    KLOGSTART2("ULONG", "ULONG ulParam, PFN pfn", ulParam, pfn);
    ULONG rc;

    rc = pfn(ulParam);

    KLOGSTOP(rc);
    return rc;
}


/**
 * Ring-0, 32-bit, init function.
 * @returns   Status word.
 * @param     pRpInit  Pointer init request packet.
 * @sketch    Set default parameters.
 *            Parse command line options.
 *            Show (kprint) configuration.
 *            Init heap.
 *            Init ldr.
 *            Init procs. (overloaded ldr procedures)
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG R0Init(RP32INIT *pRpInit)
{
    KLOGENTRY1("USHORT","RP32INIT * pRpInit", pRpInit);
    char *      pszTmp2;
    char *      pszTmp;
    ULONG       ul;
    APIRET      rc;
    KDHVMLOCK   lockhandle;

    /*
     * Commandline options
     */
    kprintf(("Options start\n"));
    pszTmp = strpbrk(pRpInit->InitArgs, "-/");
    while (pszTmp != NULL)
    {
        /*
         * Skip [-/] and look for argument.
         */
        pszTmp++; //skip [-/]
        pszTmp2 = strpbrk(pszTmp, ":=/- ");
        if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
            pszTmp2++;
        else
            pszTmp2 = NULL;

        /*
         * The option switch.
         */
        switch (*pszTmp)
        {
            case 'c':
            case 'C': /* -C[1|2|3|4] or -Com:[1|2|3|4]  -  com-port no, def:-C2 */
                if (!pszTmp2)
                    pszTmp2 = pszTmp + 1;
                ul = readnum(pszTmp2);
                switch (ul)
                {
                    case 1: options.usCom = OUTPUT_COM1; break;
                    case 2: options.usCom = OUTPUT_COM2; break;
                    case 3: options.usCom = OUTPUT_COM3; break;
                    case 4: options.usCom = OUTPUT_COM4; break;
                }
                break;

            case 'h':
            case 'H': /* Heap options */
                if (pszTmp2)
                {
                    ul = readnum(pszTmp2 + 1);
                    if (ul > 0x1000UL && ul < 0x2000000UL) /* 4KB < ul < 32MB */
                    {
                        if (strnicmp(pszTmp, "heapm", 5) == 0)
                            options.cbSwpHeapMax = ul;
                        else
                            options.cbSwpHeapInit = ul;
                    }
                }
                break;

            case 'l':
            case 'L': /* -L[..]<:|=| >[<Y..|E..| > | <N..|D..>] */
                options.fLogging = (!pszTmp2
                                    &&  (   *pszTmp2 == 'Y'
                                         || *pszTmp2 == 'y'
                                         || *pszTmp2 == 'E'
                                         || *pszTmp2 == 'e'));
                break;

            case 'q':
            case 'Q': /* quiet initialization */
                options.fQuiet = TRUE;
                break;

            case 'r':
            case 'R': /* ResHeap options */
                if (!pszTmp2)
                {
                    ul = readnum(pszTmp2 + 1);
                    if (ul > 0x1000UL && ul < 0x700000UL) /* 4KB < ul < 7MB */
                    {
                        if (strnicmp(pszTmp, "resheapm", 8) == 0)
                            options.cbResHeapMax = ul;
                        else
                            options.cbResHeapInit = ul;
                    }
                }
                break;

            case 's':
            case 'S': /* Symbol file */
                szSymbolFile[0] = '\0';
                if (!pszTmp2)
                    strncat(szSymbolFile, pszTmp2, sizeof(szSymbolFile));
                break;


            case 'v':
            case 'V': /* verbose initialization */
                options.fQuiet = FALSE;
                break;
        }

        /* next option */
        pszTmp = strpbrk(pszTmp, "-/");
    }

    /* heap min/max corrections */
    if (options.cbSwpHeapInit > options.cbSwpHeapMax)
        options.cbSwpHeapMax = options.cbSwpHeapInit;
    if (options.cbResHeapInit > options.cbResHeapMax)
        options.cbResHeapMax = options.cbResHeapInit;

    /* Log option summary */
    #ifdef DEBUG
    kprintf(("Options - Summary - Start\n"));
    kprintf(("\tszSymbolFile=%s\n", szSymbolFile));
    if (options.fQuiet)
        kprintf(("\tQuiet init\n"));
    else
        kprintf(("\tVerbose init\n"));

    if (options.fLogging)
        kprintf(("\tlogging enabled\n"));
    else
        kprintf(("\tlogging disabled\n"));
    kprintf(("\tCom port no.%03xh\n", options.usCom));
    kprintf(("\tcbSwpHeapInit=0x%08x  cbSwpHeapMax=0x%08x\n",
             options.cbSwpHeapInit, options.cbSwpHeapMax));
    kprintf(("\tcbResHeapInit=0x%08x  cbResHeapMax=0x%08x\n",
             options.cbResHeapInit, options.cbResHeapMax));
    kprintf(("Options - Summary - End\n"));
    #endif /* debug */
    /* end option summary */


    /*
     * init sub-parts
     */
    /* 32-bit devhelpers */
    kDHInit(Device_Help);

    /* functionimports. */
    if ((rc = krnlInit()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /* kLib */
    if ((rc = kLibInit()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /* logging for kKrnlLib */
    if ((rc = InitkKrnlLibLog()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }
#if 0
    /* heap */
    if (heapInit(options.cbResHeapInit, options.cbResHeapMax,
                 options.cbSwpHeapInit, options.cbSwpHeapMax) != NO_ERROR)
    {
        KLOGEXIT(ERROR_D32_HEAPINIT_FAILED);
        return ERROR_D32_HEAPINIT_FAILED;
    }
#endif

    /* ... */

    /* fake MTE. */
    if ((rc = krnlInit2()) != NO_ERROR)
    {
        KLOGEXIT(rc);
        return rc;
    }

    /*
     * Lock the 32-bit objects/segments and 16-bit datasegment in memory
     */
    /* 32-bit code segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = kDH_VMLock2(&CODE32START,
                     ((unsigned)&CODE32END & ~0xFFF) - (unsigned)&CODE32START, /* Round down so we don't overlap with the next request. */
                     VMDHL_LONG,
                     SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("code segment lock failed with with rc=%d\n", rc));

    /* 32-bit data segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = kDH_VMLock2(&DATA32START,
                     &EH_DATAEND - &DATA32START,
                     VMDHL_LONG | VMDHL_WRITE,
                     SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("data segment lock failed with with rc=%d\n", rc));

    KLOGEXIT(NO_ERROR);
    return NO_ERROR;
}


/**
 * Reads a number (unsigned long integer) for a string.
 * @returns   number read, ~0UL on error / no number read.
 * @param     pszNum  Pointer to the string containing the number.
 * @status    competely implemented.
 * @author    knut st. osmundsen
 */
ULONG    readnum(const char *pszNum)
{
    KLOGENTRY1("ULONG","const char * pszNum", pszNum);
    ULONG ulRet = 0;
    ULONG ulBase = 10;
    int   i = 0;

    /* determin ulBase */
    if (*pszNum == '0')
        if (pszNum[1] == 'x' || pszNum[1] == 'X')
        {
            ulBase = 16;
            pszNum += 2;
        }
        else
        {
            ulBase = 8;
            i = 1;
        }

    /* read digits */
    while (ulBase == 16 ? (pszNum[i] >= '0' && pszNum[i] <= '9') || (pszNum[i] >= 'a' && pszNum[i] <= 'f') || (pszNum[i] >= 'A' && pszNum[i] <= 'F')
           : (pszNum[i] >= '0' && pszNum[i] <= (ulBase == 10 ? '9' : '7'))
           )
    {
        ulRet *= ulBase;
        if (ulBase <= 10)
            ulRet += pszNum[i] - '0';
        else
            ulRet += pszNum[i] - (pszNum[i] >= 'A' ? 'A' - 10 : (pszNum[i] >= 'a' ? 'a' + 9 : '0'));

        i++;
    }

    KLOGEXIT(i > 0 ? ulRet : ~0UL);
    return i > 0 ? ulRet : ~0UL;
}

