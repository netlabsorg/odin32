/* $Id: k32KillProcessEx.cpp,v 1.1 2001-07-10 05:20:59 bird Exp $
 *
 * k32KillProcessEx - DosKillProcessEx extention.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/*******************************************************************************
*   Defined Constants And Macros                                               *
*******************************************************************************/
#define INCL_DOSPROCESS
#define INCL_DOSERRORS
#define INCL_DOSEXCEPTIONS
#define INCL_OS2KRNL_TK

#define NO_WIN32K_LIB_FUNCTIONS



/*******************************************************************************
*   Header Files                                                               *
*******************************************************************************/
#include <os2.h>
#include "devSegDf.h"                   /* Win32k segment definitions. */
#include "OS2Krnl.h"
#include "win32k.h"
#include "k32.h"
#include "options.h"
#include "dev32.h"
#include "log.h"
#include "macros.h"



/**
 * DosKillProcess extention.
 * @returns NO_ERROR on success.
 *          Appropriate error code on failure.
 * @param   flAction    Action flags.
 *                      DKP_PROCESSTREE and DKP_PROCESS is supposed to be supported (currently not implemented).
 *                      This parameter is extended with the flowing flags:
 *                      DKP_FLAG_KILL9
 * @param   pid         The identity of the process or root in process tree to be killed.
 * @sketch
 * @status  Paritially implemented.
 * @author  knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 */
APIRET k32KillProcessEx(ULONG flAction, PID pid)
{
    APIRET  rc;

    /*
     * Validate input.
     */
    if (    (flAction & DKP_ACTION_MASK) > DKP_PROCESS
        ||  flAction & ~(DKP_ACTION_MASK | DKP_FLAG_MASK))
    {
        kprintf(("k32KillProcessEx(flAction=0x%08x, pid=0x%04x): flags are invalid\n", flAction, pid));
        return ERROR_INVALID_PARAMETER;
    }

    if (    (ULONG)pid == 0UL
        ||  (ULONG)pid >= 0x10000)
    {
        kprintf(("k32KillProcessEx(flAction=0x%08x, pid=0x%04x): pid is out of range\n", flAction, pid));
        return ERROR_INVALID_PROCID;
    }


    /*
     * Do the work:
     *
     * Currently we'll try do things the nice way first, if that failes
     * we'll do the brute force kill. The reason for this is that I don't
     * think APTERM do proper cleanup, like calling exceptionhandlers...
     */
    rc = POST_SIGNAL32((USHORT)XCPT_SIGNAL_KILLPROC,
                       (USHORT)(flAction & DKP_ACTION_MASK),
                       (USHORT)(flAction & DKP_FLAG_KILL9 ? ~0 : 0),
                       (USHORT)pid);

    if (rc == ERROR_SIGNAL_PENDING && flAction & DKP_FLAG_KILL9)
    {
        rc = POST_SIGNAL32((USHORT)XCPT_SIGNAL_APTERM,
                           (USHORT)(flAction & DKP_ACTION_MASK),
                           (USHORT)~0,
                           (USHORT)pid);
    }

    return rc;
}

