/* $Id: vmutex.h,v 1.5 2000-03-23 19:25:09 sandervl Exp $ */

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
	VMutex (int fShared = VMUTEX_NONSHARED, HMTX *phMutex = NULL);
       ~VMutex();

public:
	VOID enter(ULONG timeout = VMUTEX_WAIT_FOREVER, HMTX *phMutex = NULL);
	VOID leave(HMTX *phMutex = NULL);
	int  getNrBlocked() { return waiting; };

protected:
	HMTX sem_handle;
	int  waiting;
	BOOL fShared;
};

#endif