/* $Id: d32init.c,v 1.41.2.1 2001-09-27 03:08:14 bird Exp $
 *
 * d32init.c - 32-bits init routines.
 *
 * Copyright (c) 1998-1999 knut st. osmundsen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

/*******************************************************************************
*   Defined Constants                                                          *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_NOPMAPI
#define LDR_INCL_INITONLY
#define INCL_OS2KRNL_ALL
#define INCL_OS2KRNL_LDR
#define INCL_KKL_LOG

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include <string.h>

#include "devSegDf.h"
#include "options.h"
#include "dev1632.h"
#include "dev32.h"
#include "dev32hlp.h"
#include "ldr.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
#ifdef DEBUG
static char * apszPE[] = {"FLAGS_PE_NOT", "FLAGS_PE_PE2LX", "FLAGS_PE_PE", "FLAGS_PE_MIXED", "!invalid!"};
static char * apszPEOneObject[] = {"FLAGS_PEOO_DISABLED", "FLAGS_PEOO_ENABLED", "FLAGS_PEOO_FORCED", "!invalid!"};
static char * apszInfoLevel[] = {"INFOLEVEL_QUIET", "INFOLEVEL_ERROR", "INFOLEVEL_WARNING", "INFOLEVEL_INFO", "INFOLEVEL_INFOALL", "!invalid!"};
#endif


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
 ULONG          readnum(const char *pszNum);


/*
 * Ring-0, 32-bit, init function.
 * @returns   Status word.
 * @param     pRpInit  Pointer init request packet.
 * @sketch    Set TKSSBase32.
 *            Set default parameters.
 *            Parse command line options.
 *            Show (kprint) configuration.
 *            Init heap.
 *            Init ldr.
 *            Init procs. (overloaded ldr procedures)
 * @status    completely implemented.
 * @author    knut st. osmundsen
 */
ULONG _System R0Init(RP32INIT *pRpInit)
{
    char *      pszTmp2;
    char *      pszTmp;
    APIRET      rc;
    LOCKHANDLE  lockhandle;

    /*---------------------*/
    /* commandline options */
    /*---------------------*/
    kprintf(("Options start\n"));
    pszTmp = strpbrk(pRpInit->InitArgs, "-/");
    while (pszTmp != NULL)
    {
        int cch;
        pszTmp++; //skip [-/]
        cch = strlen(pszTmp);
        switch (*pszTmp)
        {
            case '1':   /* All-In-One-Object fix - temporary...- -1<-|+|*> */
                if (pszTmp[1] == '-')
                    options.fPEOneObject = FLAGS_PEOO_DISABLED;
                else if (pszTmp[1] == '+')
                    options.fPEOneObject = FLAGS_PEOO_ENABLED;
                else
                    options.fPEOneObject = FLAGS_PEOO_FORCED;
                break;

            case 'd':
            case 'D':
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL
                    && (pszTmp2[1] == 'N' ||pszTmp2[1] == 'n' || pszTmp2[1] == 'D' || pszTmp2[1] == 'd')
                    )
                    options.fDllFixes = FALSE;
                else
                    options.fDllFixes = TRUE;
                break;

            case 'e':
            case 'E':/* Elf or EXe */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp[1] != 'x' && pszTmp[1] != 'X')
                {
                    options.fElf = !(pszTmp2 != NULL
                                     && (   pszTmp2[1] == 'N' || pszTmp2[1] == 'n'
                                         || pszTmp2[1] == 'D' || pszTmp2[1] == 'd'));
                }
                else
                {
                    options.fExeFixes = !(pszTmp2 != NULL
                                          && (   pszTmp2[1] == 'N' || pszTmp2[1] == 'n'
                                              || pszTmp2[1] == 'D' || pszTmp2[1] == 'd'));
                }
                break;

            case 'f':
            case 'F': /* -F[..]<:|=| >[<Y..|E..| > | <N..|D..>]  - force preload */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 == NULL
                    || (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y' || pszTmp2[1] == 'E' || pszTmp2[1] == 'e')
                    )
                    options.fForcePreload = TRUE;
                else
                    options.fForcePreload = FALSE;
                break;

            case 'j':
            case 'J': /* -Java:<Yes|No> */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                options.fJava =
                    pszTmp2 != NULL
                    && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '=')
                    && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                break;

            case 'l':
            case 'L': /* -L[..]<:|=| >[<Y..|E..| > | <N..|D..>] */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL
                    && (pszTmp2[1] == 'Y' ||pszTmp2[1] == 'y' || pszTmp2[1] == 'E' || pszTmp2[1] == 'e')
                    )
                    options.fLogging = TRUE;
                else
                    options.fLogging = FALSE;
                break;

            case 'n':
            case 'N': /* NoLoader */
                options.fNoLoader = TRUE;
                break;

            case 'p':
            case 'P': /* PE */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                {
                    pszTmp2++;
                    if (strnicmp(pszTmp2, "pe2lx", 5) == 0)
                        options.fPE = FLAGS_PE_PE2LX;
                    else if (strnicmp(pszTmp2, "pe", 2) == 0)
                        options.fPE = FLAGS_PE_PE;
                    else if (strnicmp(pszTmp2, "mixed", 2) == 0)
                        options.fPE = FLAGS_PE_MIXED;
                    else if (strnicmp(pszTmp2, "not", 2) == 0)
                        options.fPE = FLAGS_PE_NOT;
                    else
                        kprintf(("R0Init32: invalid parameter -PE:...\n"));
                }
                else
                    kprintf(("R0Init32: invalid parameter -PE...\n"));
                break;

            case 'q':
            case 'Q': /* quiet initialization */
                options.fQuiet = TRUE;
                break;

            case 'r':
            case 'R': /* ResHeap options or REXX option */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                if (   (pszTmp[1] == 'E' || pszTmp[1] == 'e')
                    && (pszTmp[2] == 'X' || pszTmp[2] == 'x'))
                {   /* REXX */
                    options.fREXXScript =
                        pszTmp2 != NULL
                        && (int)(pszTmp2-pszTmp) < cch-1
                        && (*pszTmp2 == ':' || *pszTmp2 == '=')
                        && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                }
                break;

            case 's':
            case 'S': /* Script:<Yes|No> */
                pszTmp2 = strpbrk(pszTmp, ":=/- ");
                options.fUNIXScript =
                    pszTmp2 != NULL
                    && (int)(pszTmp2-pszTmp) < cch-1
                    && (*pszTmp2 == ':' || *pszTmp2 == '=')
                    && (pszTmp2[1] == 'Y' || pszTmp2[1] == 'y');
                break;

            case 'v':
            case 'V': /* verbose initialization */
                options.fQuiet = FALSE;
                break;

            case 'w':
            case 'W': /* ModuleBase info level; -W<n> or -Warning:<n> */
                if (pszTmp[1] >= '0' && pszTmp[1] <= '4')
                    options.ulInfoLevel = pszTmp[1] - '0';
                else
                {
                    pszTmp2 = strpbrk(pszTmp, ":=/- ");
                    if (pszTmp2 != NULL && (*pszTmp2 == ':' || *pszTmp2 == '='))
                        pszTmp2++;
                    else
                        pszTmp2 = pszTmp + 1;

                    if (*pszTmp2 >= '0' && *pszTmp2 <= '4')
                        options.ulInfoLevel = *pszTmp2 - '0';
                }
                break;

        }
        pszTmp = strpbrk(pszTmp, "-/");
    }

    /* Log option summary */
    #ifdef DEBUG
    kprintf(("Options - Summary - Start\n"));
    if (options.fQuiet)
        kprintf(("\tQuiet init\n"));
    else
        kprintf(("\tVerbose init\n"));

    if (options.fLogging)
        kprintf(("\tlogging enabled\n"));
    else
        kprintf(("\tlogging disabled\n"));

    kprintf(("\tfPE=%d (%s)\n",     options.fPE, apszPE[KMIN(options.fPE, 5)]));
    kprintf(("\tfPEOneObject=%d (%s)\n",     options.fPEOneObject, apszPEOneObject[KMIN(options.fPEOneObject, 3)]));
    kprintf(("\tulInfoLevel=%d (%s)\n", options.ulInfoLevel, apszInfoLevel[KMIN(options.ulInfoLevel, 5)]));
    kprintf(("\tfElf=%d\n",         options.fElf));
    kprintf(("\tfUNIXScript=%d\n",  options.fUNIXScript));
    kprintf(("\tfREXXScript=%d\n",  options.fREXXScript));
    kprintf(("\tfJAVA=%d\n",        options.fJava));
    kprintf(("\tfNoLoader=%d\n",    options.fNoLoader));
    kprintf(("Options - Summary - End\n"));
    #endif /* debug */
    /* end option summary */


    /*
     * init sub-parts
     */
    /* loader */
    if ((rc = ldrInit()) != NO_ERROR)
        return rc;

    /* apis */
    #if 0
    if ((rc = APIInit()) != NO_ERROR)
        return rc;
    #endif

    /* callgate */
    #ifndef R3TST
    if ((rc = InitCallGate()) != NO_ERROR)
    {
        kprintf(("R0Init32: InitCallGate failed with rc=%d\n", rc));
        return rc;
    }
    #endif


    /*
     * Lock the 32-bit objects/segments and 16-bit datasegment in memory
     */
    /* 32-bit code segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = D32Hlp_VMLock2(&CODE32START,
                        ((unsigned)&CODE32END & ~0xFFF) - (unsigned)&CODE32START, /* Round down so we don't overlap with the next request. */
                        VMDHL_LONG,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("code segment lock failed with with rc=%d\n", rc));

    /* 32-bit data segment */
    memset(SSToDS(&lockhandle), 0, sizeof(lockhandle));
    rc = D32Hlp_VMLock2(&DATA32START,
                        &CONST32_ROEND - &DATA32START,
                        VMDHL_LONG | VMDHL_WRITE,
                        SSToDS(&lockhandle));
    if (rc != NO_ERROR)
        kprintf(("data segment lock failed with with rc=%d\n", rc));

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

    return i > 0 ? ulRet : ~0UL;
}

