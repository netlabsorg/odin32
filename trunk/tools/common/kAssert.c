/* $Id: kAssert.c,v 1.1 2002-02-24 02:47:23 bird Exp $
 *
 * kAssert - Assertion helper functions.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * GPL
 *
 */
#ifndef NOFILEID
static const char szFileId[] = "$Id: kAssert.c,v 1.1 2002-02-24 02:47:23 bird Exp $";
#endif


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_BASE
#include <os2.h>

#include <stdio.h>
#include "kTypes.h"


/**
 * This function will display and log assertions which failes.
 * @returns TRUE:   issue breakpoint instruction
 *          FALSE:  continue execution.
 * @param   pszExpr
 * @param   pszFilename     Name of the file the kASSERT macro is called from.
 * @param   uLine           Line number (within pszFilename) of the kASSERT macro call.
 * @param   pszFunction     Name of the function the kASSERT macro is called from.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
KBOOL KLIBCALL kAssertMsg(const char *pszExpr, const char *pszFilename, unsigned uLine, const char *pszFunction)
{
    PTIB    ptib;
    PPIB    ppib;
    KBOOL   fRc = TRUE;

    DosGetInfoBlocks(&ptib, &ppib);
    if (ppib->pib_ultype != 3)          /* !PM */
    {
        fprintf(stderr,
                "\n"
                "Assertion Failed: %.640s\n"
                "File: %.260s\n"
                "Line: %d\n"
                "Function: %.128s\n",
                pszExpr, pszFilename, uLine, pszFunction);
    }
    else
    {
        char    szTitle[42];
        char    szMsg[1024];
        HAB     hab;
        HMQ     hmq;
        hab = WinInitialize(0);
        hmq = WinCreateMsgQueue(hab, 0);

        /*
         * Format a title.
         */
        if (DosQueryModuleName(ppib->pib_hmte, sizeof(szTitle), &szTitle[0]))
            sprintf(szTitle, "Process 0x%x(%d)", ppib->pib_ulpid, ppib->pib_ulpid);

        /*
         * Format the message.
         */
        sprintf(szMsg,
                "\n"
                "Assertion Failed: %.640s\n"
                "File: %.260s\n"
                "Line: %d\n"
                "Function: %.128s\n",
                pszExpr, pszFilename, uLine, pszFunction);

        fRc = WinMessageBox(HWND_DESKTOP, HWND_DESKTOP, szTitle, szMsg, 0,
                            MB_ABORTRETRYIGNORE | MB_MOVEABLE | MB_APPLMODAL) != MBID_IGNORE;

        WinDestroyMsgQueue(hmq);
        WinTerminate(hab);
    }
    return fRc;
}


