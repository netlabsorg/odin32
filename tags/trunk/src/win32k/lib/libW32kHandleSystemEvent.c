/* $Id: libW32kHandleSystemEvent.c,v 1.1 2001-02-18 14:46:31 bird Exp $
 *
 * libW32kHandleSystemEvent - Override system events like Ctrl-Alt-Delete
 *          and Ctrl-Alt-2xNumLock.
 *
 * Copyright (c) 2000-2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#define INCL_DOSERRORS
#define INCL_DOSFILEMGR
#define INCL_DOSDEVICES


/*******************************************************************************
*   Internal Functions                                                         *
*******************************************************************************/
#include <os2.h>
#include "win32k.h"


/*******************************************************************************
*   Global Variables                                                           *
*******************************************************************************/
extern BOOL     fInited;
extern HFILE    hWin32k;


/**
 * Override a system event.
 * @returns NO_ERROR on success.
 *          ERROR_INVALID_PARAMETER     If incorrect parameter specified.
 *          ERROR_ACCESS_DENIED         If you tried to unset (fHandle=FALSE) an event specifying the wrong
 *                                      hev. Or you tried to handle (fHandle=TRUE) and event which is allready
 *                                      handled by someone else (which is alive and kicking - ie. !fBad).
 *          ERROR_INIT_ROUTINE_FAILED   If the Win32k library isn't inited successfully.
 *          Any errorcode returned by DosDevIOCtl.
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
APIRET APIENTRY  W32kHandleSystemEvent(ULONG ulEvent, HEV hev, BOOL fHandle)
{
    APIRET rc;

    if (fInited)
    {
        K32HANDLESYSTEMEVENT    Param;
        ULONG                   cbParam = sizeof(Param);
        ULONG                   cbData = 0UL;

        Param.ulEvent = ulEvent;
        Param.hev = hev;
        Param.fHandle = fHandle;
        Param.rc = ERROR_INVALID_PARAMETER;

        rc = DosDevIOCtl(hWin32k,
                         IOCTL_W32K_K32,
                         K32_HANDLESYSTEMEVENT,
                         &Param, sizeof(Param), &cbParam,
                         "", 1, &cbData);

        if (rc == NO_ERROR)
            rc = Param.rc;
    }
    else
        rc = ERROR_INIT_ROUTINE_FAILED;

    return rc;
}

