/* $Id: vmutex.h,v 1.4 1999-10-24 22:48:17 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __VMUTEX_H__
#define __VMUTEX_H__

#define VMUTEX_RETURN_IMM	 0
#define VMUTEX_WAIT_FOREVER	-1

#define VMUTEX_NONSHARED         0
#define VMUTEX_SHARED		 1

#ifdef _OS2WIN_H
#define HMTX DWORD
#endif

class VMutex
{
public:
	VMutex (int fShared = VMUTEX_NONSHARED);
       ~VMutex();

public:
	VOID enter(ULONG timeout = VMUTEX_WAIT_FOREVER);
	VOID leave();
	int  getNrBlocked() { return waiting; };

protected:
	HMTX sem_handle;
	int  waiting;
};

#endif