/* $Id: k32HandleSystemEvent.cpp,v 1.1.2.1 2001-09-27 03:08:22 bird Exp $
 *
 * k32HandleSystemEvent - Override system events like Ctrl-Alt-Delete
 *          and Ctrl-Alt-2xNumLock.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSMEMMGR
#define INCL_DOSERRORS

#define INCL_OS2KRNL_TK
#define INCL_OS2KRNL_SEM
#define NO_WIN32K_LIB_FUNCTIONS
#define INCL_KKL_LOG

/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include <OS2Krnl.h>
#include <kKrnlLib.h>

#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
/**
 * This global array will have an entry for each event. If the entry is
 * different from NULLHANDLE we should post the the semaphore and not
 * let the OS handle the event.
 */
struct
{
    HEV     hev;                        /* NULLHANDLE if the event isn't overridden. */
                                        /* Handle to post if overridden.             */
    BOOL    fBad;                       /* Flag which is set if a posting of the hev */
                                        /* returned and error code.                  */
} aSysEventsOverrides[] =
{
    {NULLHANDLE, FALSE},                /* dh SendEvent - Session Manager (mouse - both buttons) */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Ctrl-Break                             */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Ctrl-C                                 */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Ctrl-ScrollLock                        */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Ctrl-PrtSc                             */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Shift-PrtSc                            */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Session Manager (keyboard - Ctrl-Esc)  */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Ctrl-Alt-Del                           */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Keyboard Hot Plug/Reset                */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Power suspend event                    */
    {NULLHANDLE, FALSE},                /* dh SendEvent - Power off event                        */
    {NULLHANDLE, FALSE}                 /* VectorSDF    - System Dump                            */
};


/**
 * Override a system event.
 * @returns NO_ERROR on success.
 *          ERROR_INVALID_PARAMETER     If incorrect parameter specified.
 *          ERROR_ACCESS_DENIED         If you tried to unset (fHandle=FALSE) an event specifying the wrong
 *                                      hev. Or you tried to handle (fHandle=TRUE) and event which is allready
 *                                      handled by someone else (which is alive and kicking - ie. !fBad).
 * @param   ulEvent     Event to override.
 *                      In win32k.h the valid events are defined.
 * @param   hev         Handle of shared event semaphore which is posted when
 *                      the specified system event occures.
 *                      If the value 0xFFFFFFFF is specified the system will handle the event.
 * @param   fHandle     Action flag. <br>
 *                      TRUE:  Take control of the event.<br>
 *                      FALSE: Give control back to the OS of this event. (hev must match the current handle!)
 * @status  partially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 * @remark  Not all events are implemented yet.
 */
APIRET k32HandleSystemEvent(ULONG ulEvent, HEV hev, BOOL fHandle)
{
    /*
     * Validate parameters.
     *  Event identifier range.
     *  The event semaphore handle seems ok.
     */
    if (    ulEvent > K32_SYSEVENT_LAST
        ||  ((ULONG)hev & 0xFFFF0000UL) != 0x80010000UL /* 0x80010000 seems to be the shared event semaphore handle bits. */
        )
        return ERROR_INVALID_PARAMETER;


    /*
     * Check if we're trying to override an allready taken event.
     */
    if (    (fHandle  && aSysEventsOverrides[ulEvent].hev && !aSysEventsOverrides[ulEvent].fBad)
        ||  (!fHandle && aSysEventsOverrides[ulEvent].hev != hev && !aSysEventsOverrides[ulEvent].fBad)
        )
        return ERROR_ACCESS_DENIED;


    /*
     * Handle action.
     */
    if (fHandle)
    {   /* Take controll of an event. */
        aSysEventsOverrides[ulEvent].fBad = FALSE;
        aSysEventsOverrides[ulEvent].hev = hev;
    }
    else
    {   /* Giveback control. */
        aSysEventsOverrides[ulEvent].hev = NULLHANDLE;
    }

    return NO_ERROR;
}

