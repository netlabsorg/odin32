/* $Id: vmutex.h,v 1.6 2002-04-07 14:34:30 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __VMUTEX_H__
#define __VMUTEX_H__

#include <odincrt.h>

class VMutex
{
public:
	VMutex();
       ~VMutex();

public:
	VOID enter()         { DosEnterCriticalSection(&critsect); };
	VOID leave()         { DosLeaveCriticalSection(&critsect); };

protected:
        CRITICAL_SECTION_OS2 critsect;
};

#endif