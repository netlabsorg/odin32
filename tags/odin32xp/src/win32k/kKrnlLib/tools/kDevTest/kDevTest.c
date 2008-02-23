/* $Id: kDevTest.c,v 1.1 2002-09-30 23:53:52 bird Exp $
 *
 * Main Program for Ring-3 Device Testing.
 *
 * Copyright (c) 2000-2002 knut st. osmundsen <bird@anduin.net>
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


/** @design Win32k Ring-3 Testing
 * I'll try to make it possible to test parts or all the win32k code in ring-3.
 * To archive this the interface against the kernel has to be faked/emulated.
 * More precisely:
 *      - DevHelps.
 *      - Worker routines for imported kernel functions. (calltab.asm jumps to them.)
 *      - 16-bit stack 1Kb.
 *      - Strategy calls.
 *      - Fake module loadings (= testcases).
 *      - ?
 *
 * Some of the initstuff has to be omitted, at least in the start. The first
 * goal is to be able to test _ldrOpenPath and _ldrOpen.
 *
 *
 * @subsection  Device Helper Routines
 *
 * These I think we'll implemented by providing the kernel interface, a far 16:16
 * pointer to a dh-router. Our router will in most cases thunk back to 32-bit
 * code and implementet the required devhelp routines in pure C code.
 *
 * These are the needed routines:
 *      - DevHelp_VirtToLin   - ok
 *      - DevHelp_VMAlloc     - ok
 *      - DevHelp_VMFree      - ok
 *      - DevHelp_GetDOSVar   - ok
 *      - DevHelp_VMLock
 *      - DevHelp_VMUnLock  ?
 *      - DevHelp_VMSetMem  ?
 *      - DevHelp_Yield     ?
 *
 *
 * @subsection  Worker routines for imported kernel functions
 *
 * Create worker routines for the imported kernel functions. Calltab will be
 * set up to jump to them. This is done in d32init.c, in stead of using
 * the importtab.
 *
 * Some of these workers will be parts of testcases. For example g_tkExecPgm
 * and _LDRQAppType.
 *
 * Only the imported functions are implemented on demand. Initially these
 * functions will be provided:
 *      - ldrOpen
 *      - ldrRead
 *      - ldrClose
 *      - ldrOpenPath
 *      - SftFileSize
 *
 *
 * @subsection  16-bit stack
 *
 * To make this test real the stack has to be 16-bit and _very_ small (1KB).
 * TKSSBase have to be implemented by the DevHelp_GetDOSVar DosTable2 stuff.
 * The stack will be thunked to 16-bit by a thunking procedure called from
 * main. This procedure thunks the stack (quite easy, we're in tiled memory!),
 * set the value of TKSSBase, calls a C function which does all the rest of
 * the testing. When taht function returns, the stack will be thunked back
 * to 32-bit, TKSSBase will be zeroed, and the procedure returns to main.
 *
 *
 * @subsection  Straegy routine calls (not implemented)
 *
 * We'll call the strategy entry points with init request packets. The initiation
 * will require a replacement for DosDevIOCtl (16-bit) which calls the
 * $elf strategy routine. We'll also have to provide fakes for kernel probing,
 * verifing and overloading in d32init.c.
 *
 *
 * @subsection  Order of events
 *
 * This is the order this testing environment currently works:
 *      1) init devhelp subsystem
 *      2) init workers
 *      3) thunk stack
 *      4) Fake 16-bit init. Set TKSSBase, FlatDS, FlatCS, DevHelp pointer....
 *      5) Call R0Init32().
 *         (d32init.c is modified a bit to setup the calltab correctly.)
 *      6) Start testing...
 *      7) Testing finished - thunk stack back to 32-bit.
 */



/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#include <os2.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "kDevTest.h"


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
BOOL init(void);
BOOL LoadAndInitDriver(const char *pszDriver, const char *pszArgs, BOOL fBaseDev);





/**
 * Initiates all 'subsystems'
 * @returns Success indicator.
 */
BOOL init(void)
{
    kdtDHInit();
    return TRUE;
}


/**
 * Loads and initiates an device driver.
 * @returns Success indicator.
 * @param   pszDriver   Name of the driver module file.
 * @param   pszArgs     Arguments to pass along.
 * @param   fBaseDev    Set this if base device.
 */
BOOL LoadAndInitDriver(const char *pszDriver, const char *pszArgs, BOOL fBaseDev)
{
    PMODINFO    pModInfo;

    /*
     * Load it.
     */
    pModInfo = kdtLoadDriver(pszDriver);
    if (pModInfo)
    {
        if (kdtLoadValidateDriver(pModInfo))
        {
            ULONG rc;

            if (kdtInitDriver(pModInfo, pszArgs, fBaseDev))
            {
                return TRUE;
            }
            else
                printf("kDevTest: Init failed with rc=%d\n", rc);
        }
        else
            printf("kDevTest: Invalid driver module %s.\n", pszDriver);
        free(pModInfo);
    }
    else
        printf("kDevTest: failed to load %s.\n", pszDriver);

    return FALSE;
}


/**
 * Prints syntax information.
 */
void syntax(void)
{
    printf(
           "kDevTest v0.0.0 - Ring 3 Device Testing!\n"
           "\n"
           "syntax: kDevTest.exe [options] <module> [args]\n"
           "\n"
           "options:\n"
           "    -b      basedevice. default is normal device.\n"
           "\n"
           "  module    Full name of the device module to test.\n"
           "  args      Arguments to present to the device driver.\n");
}


/**
 * Main function. Arguments _currently_ unused.
 */
int main(int argc, char **argv)
{
    int         rc;
    int         argi;
    BOOL        fOptions = TRUE;

    /* parsed arguments */
    static char szDriver[CCHMAXPATH];
    static char szArguments[4096];
    BOOL        fBaseDev = FALSE;


    /*
     * Parse input.
     */
    for (szDriver[0] = '\0', argi = 1; argi < argc; argi++)
    {
        if (!szDriver[0] && (argv[argi][0] == '-' || argv[argi][0] == '/'))
        {
            switch (argv[argi][1])
            {
                case 'b':
                case 'B':
                    fBaseDev = TRUE;
                    break;

                default:
                    printf("kDevTest: syntax error! invalid argument %s\n", argv[argi]);
                case '?':
                case '-':
                case 'h':
                case 'H':
                    syntax();
                    return 12;
            }
        }
        else if (!szDriver[0])
            strcpy(szDriver, argv[argi]);
        else
        {   /* TODO: quoting etc */
            strcat(strcat(szArguments, " "), argv[argi]);
        }
    }

    if (!szDriver[0])
    {
        printf("kDevTest: syntax error! missing driver name.\n");
        syntax();
        return 12;
    }


    /*
     * Load and init the driver
     */
    if (LoadAndInitDriver(&szDriver[0], &szArguments[0], fBaseDev))
    {
        rc = 0;
    }
    else
    {
        rc = 8;
        printf("kDevTest: LoadAndInitDriver failed\n");
    }

    return rc;
}

