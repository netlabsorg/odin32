/* $Id: PerTaskW32kData.h,v 1.1 2001-07-08 02:58:36 bird Exp $
 *
 * Per Task (Win32k) Data.
 *
 * Copyright (c) 2001 knut st. osmundsen (kosmunds@csc.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#ifndef _PreTaskW32kData_h_
#define _PreTaskW32kData_h_


/*******************************************************************************
*   Structures and Typedefs                                                    *
*******************************************************************************/
typedef struct _PerTaskData
{
    AVLNODECORE     core;
    ULONG           cUsage;             /* Usage counter. */
    PID             pid;                /* Also in core.Key */
    PSZ             pszzOdin32Env;      /* Pointer to environment block. */
    LOCKHANDLE      lockOdin32Env;      /* VMLock handle for the environment block. */
} PTD, *PPTD;


PPTD _Optlink   GetTaskData(PID pid);
void _Optlink   RemoveTaskData(PID pid);

#endif

