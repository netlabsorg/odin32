/* $Id: vmutex.h,v 1.2 1999-05-27 15:17:59 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __VMUTEX_H__
#define __VMUTEX_H__

#define VMUTEX_RETURN_IMM	 0
#define VMUTEX_WAIT_FOREVER	-1

#ifdef _OS2WIN_H
#define HMTX DWORD
#endif

class VMutex
{
public:
	VMutex (void);
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