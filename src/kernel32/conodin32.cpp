/* $Id: conodin32.cpp,v 1.1 2002-06-26 07:13:30 sandervl Exp $
 *
 * Part of consol which require Odin32 headers not compatible with os2wrap.h.
 *
 * Copyright (c) 2002 knut st. osmundsen (bird@anduin.net)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>

#include <os2win.h>
#include <winnt.h>
#include <winnls.h>
#include <wincon.h>
#include <winprocess.h>

#include <stdlib.h>
#include <string.h>

#include <misc.h>
#include <wprocess.h>

#include "console.h"

#define DBG_LOCALLOG	DBG_conodin32
#include "dbglocal.h"






/**
 * Internal worker for generating console control events.
 * This worker doesn't validate the event.
 *
 * @returns TRUE/FALSE.
 * @param   dwCtrlEvent         The event number to generate.
 *                              CTRL_C_EVENT or CTRL_BREAK_EVENT.
 * @param   dwProcessGroupId    The target process group
 *                              Odin32: Currently only supports our selfs.
 * @status  partially implemented and tested.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @remark  NT is doing this asynchronus.
 * @remark  We don't handle all removal/add situation too well..
 */
BOOL InternalGenerateConsoleCtrlEvent(DWORD dwCtrlEvent, DWORD dwProcessGroupId)
{
    ENVDB *         pEDB;
    BOOL            fHandled = FALSE;
    PDB *           pdb;
    PCONCTRL        pCur;
    PCONCTRLDATA    pData;

    /*
     * Get process data.
     */
    pdb = PROCESS_IdToPDB(0);
    if (pdb == NULL || pdb->env_db == NULL)
    {
        dprintf(("KERNEL32:CONSOLE: GenerateConsoleCtrlEvent(%08x,%08x) - pdb == NULL || pdb->env_db == NULL. To early?\n",
                 dwCtrlEvent, dwProcessGroupId));
        DebugInt3();
        return FALSE;
    }
    pEDB = pdb->env_db;


    /*
     * Do the work.
     * I'm lazy, so I'm using critical section for this.
     * (Should perhaps use the break_sem.. But NT doesn't use that apparently.)
     */
    EnterCriticalSection(&pEDB->section);
    pData = ((PCONCTRLDATA)pEDB->break_handlers);
    pCur = pData->pHead;
    while (pCur)
    {
        void *pvFree = NULL;
        pCur->flFlags |= ODIN32_CONCTRL_FLAGS_USED;
        fHandled = ((PHANDLER_ROUTINE)pCur->pfnHandler)(dwCtrlEvent);
        pCur->flFlags &= ~ODIN32_CONCTRL_FLAGS_USED;

        if (pCur->flFlags & ODIN32_CONCTRL_FLAGS_USED)
        {
            pvFree = pCur;
            if (pCur->pNext)
                pCur->pNext->pPrev = pCur->pPrev;
            else
                pData->pTail = pCur->pPrev;
            if (pCur->pPrev)
                pCur->pPrev->pNext = pCur->pNext;
            else
                pData->pHead = pCur->pNext;
        }

        /* next */
        pCur = pCur->pNext;
        free(pvFree);
        if (fHandled)
            break;
    }
    LeaveCriticalSection(&pEDB->section);
    return fHandled;
}


/**
 * Generatea Ctrl event to for a console. Ie. Ctrl-C or Ctrl-Break.
 * @returns Success indicator.
 * @param   dwCtrlEvent         The event number to generate.
 *                              CTRL_C_EVENT or CTRL_BREAK_EVENT.
 * @param   dwProcessGroupId    The target process group
 *                              Odin32: Currently only supports our selfs.
 * @status  partially implemented and tested.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @todo    Respect the fIgnoreCtrlC flag.
 */
BOOL WIN32API GenerateConsoleCtrlEvent(DWORD dwCtrlEvent,
                                       DWORD dwProcessGroupId)
{
    ENVDB * pEDB;
    BOOL    fHandled = FALSE;

    dprintf(("KERNEL32:CONSOLE: GenerateConsoleCtrlEvent(%08x,%08x) - partially implemented(?).\n",
             dwCtrlEvent, dwProcessGroupId));

    /*
     * Get process data.
     */
    //PDB *pdb = PROCESS_IdToPDB(0);
    struct _PDB *pdb = PROCESS_IdToPDB(0);
    if (pdb == NULL || pdb->env_db == NULL)
    {
        dprintf(("KERNEL32:CONSOLE: GenerateConsoleCtrlEvent(%08x,%08x) - pdb == NULL || pdb->env_db == NULL. To early?\n",
                 dwCtrlEvent, dwProcessGroupId));
        DebugInt3();
        return FALSE;
    }

    /*
     * Validate input.
     */
    if (    dwCtrlEvent != CTRL_C_EVENT
        &&  dwCtrlEvent != CTRL_BREAK_EVENT)
    {   /* bad event */
        SetLastError(ERROR_MR_MID_NOT_FOUND);
        dprintf(("KERNEL32:CONSOLE: GenerateConsoleCtrlEvent(%08x,%08x) - Event. Returns FALSE.\n",
                 dwCtrlEvent, dwProcessGroupId));
        return FALSE;
    }

    if (   dwProcessGroupId != 0
        && (   dwProcessGroupId != (DWORD)pdb->server_pid
            && (pdb->group == NULL || dwProcessGroupId != (DWORD)pdb->group->server_pid))
           )
    {   /* bad pid group */
        dprintf(("KERNEL32:CONSOLE: GenerateConsoleCtrlEvent(%08x,%08x) - Unsupported PID no action taken. Returns TRUE.\n",
                 dwCtrlEvent, dwProcessGroupId));
        return TRUE;
    }


    /*
     * Do the work.
     */
    return InternalGenerateConsoleCtrlEvent(dwCtrlEvent, dwProcessGroupId);
}


/**
 * Default ctrl handler.
 *
 * @returns TRUE if handled (allway).
 *          FALSE if not.
 * @param   dwEvent     The event which is to be handled.
 * @status  partially implemented and tested
 * @author  knut st. osmundsen (bird@anduin.net)
 * @todo    Make sure the exit code is right.
 *          Needs *working* testcases!
 */
BOOL WIN32API DefaultConsoleCtrlHandler(DWORD dwEvent)
{
    dprintf(("KERNEL32:CONSOLE: DefaultConsoleCtrlHandler(%08x)\n", dwEvent));
    ExitProcess(0);                    /* TODO: Make sure this is the right exit value */
    return TRUE;
}


/**
 * Adds or removes a control handler of a certain type.
 *
 * @returns Success indicator.
 * @param   pfnHandler  Pointer to the handler routine.
 *                      NULL is allowed.
 * @param   fAdd        TRUE: Add handler.
 *                      FALSE: Removed handler.
 *                      If pfnHandler is NULL the call will
 *                      disable / enable handling of the Ctrl-C event.
 *                      (Not completely implemented, see todo on GenerateConsoleCtrlEvent.)
 * @status  partially implemented tested.
 * @author  knut st. osmundsen (bird@anduin.net)
 * @todo    Testcase 24 doesn't work. NT does restart a few times apparently.
 *          Must check Win2k and WinXP!
 */
BOOL WIN32API SetConsoleCtrlHandler(PHANDLER_ROUTINE pfnHandler, BOOL fAdd)
{
    BOOL    fRc = FALSE;
    PDB *   pdb;
    dprintf(("KERNEL32:CONSOLE: SetConsoleCtrlHandler(%08x,%08x)\n", pfnHandler, fAdd));

    pdb = PROCESS_Current();
    if (pdb)
    {
        if (pdb->env_db)
        {
            ENVDB *         pEDB = pdb->env_db;
            PCONCTRLDATA    pData = (PCONCTRLDATA)pEDB->break_handlers;

            /*
             * Do the work.
             * I'm lazy, so I'm using critical section for this.
             * I probably should use the break_sem...
             */
            if (pfnHandler)
            {
                if (fAdd)
                {
                    /*
                     * Add operation.
                     */
                    PCONCTRL    pNew = (PCONCTRL)malloc(sizeof(CONCTRL));
                    if (pNew)
                    {
                        pNew->pfnHandler = (void *)pfnHandler;
                        pNew->pPrev = NULL;

                        EnterCriticalSection(&pEDB->section);
                        pNew->pNext = pData->pHead;
                        if (pData->pHead)
                        {
                            pData->pHead->pPrev = pNew;
                            pData->pHead = pNew;
                        }
                        else
                            pData->pTail = pData->pHead = pNew;
                        LeaveCriticalSection(&pEDB->section);

                        fRc = TRUE;
                    }
                    else
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                }
                else
                {
                    PCONCTRL pCur;

                    /*
                     * Removal of a routine is requested.
                     *  Walk the list looking for it.
                     */
                    EnterCriticalSection(&pEDB->section);
                    for (pCur = pData->pTail;
                         pCur;
                         pCur = pCur->pPrev)
                    {
                        if (pCur->pfnHandler == pfnHandler)
                        {
                            /*
                             * Found it.
                             * Unlink it if not marked as active.
                             * Else mark it for deleletion.
                             */
                            if (pCur->flFlags & ODIN32_CONCTRL_FLAGS_USED)
                                pCur->flFlags = ODIN32_CONCTRL_FLAGS_REMOVED;
                            else
                            {
                                if (pCur->pNext)
                                    pCur->pNext->pPrev = pCur->pPrev;
                                else
                                    pData->pTail = pCur->pPrev;
                                if (pCur->pPrev)
                                    pCur->pPrev->pNext = pCur->pNext;
                                else
                                    pData->pHead = pCur->pNext;
                                free(pCur);
                            }

                            fRc = TRUE;
                            break;
                        }
                    }
                    LeaveCriticalSection(&pEDB->section);

                    if (!fRc)
                        SetLastError(ERROR_INVALID_PARAMETER);
                }
            }
            else
            {   /*
                 * No handler routine - which means setting some flag.
                 */
                dprintf(("KERNEL32:CONSOLE: SetConsoleCtrlHandler(0,%d) Turning on and off Ctrl-C isn't implemented yet\n",
                         fAdd));
                EnterCriticalSection(&pEDB->section);
                pData->fIgnoreCtrlC = fAdd != 0; /* Just make this a 1 or 0 for convenience. */
                LeaveCriticalSection(&pEDB->section);
                fRc = TRUE;

            }
        }
        else
        {
            dprintf(("KERNEL32:CONSOLE: SetConsoleCtrlHandler - pdb->env_db is NULL. This is really bad!\n"));
            DebugInt3();
        }
    }
    else
    {
        dprintf(("KERNEL32:CONSOLE: SetConsoleCtrlHandler - PROCESS_Current(): failed! \n"));
        DebugInt3();
    }

    return fRc;
}
