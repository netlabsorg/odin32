/* $Id: vsemaphore.h,v 1.1 2002-07-22 09:53:48 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __VSEMAPHORE_H__
#define __VSEMAPHORE_H__

#define VSEMAPHORE_RETURN_IMM	 	0
#define VSEMAPHORE_WAIT_FOREVER		-1

#define VSEMAPHORE_NONSHARED         	0
#define VSEMAPHORE_SHARED		1

#ifdef _OS2WIN_H
#define HEV DWORD
#endif

class VSemaphore
{
public:
	VSemaphore (int fShared = VSEMAPHORE_NONSHARED, HEV *phSem = NULL);
       ~VSemaphore();

public:
	VOID wait(ULONG timeout = VSEMAPHORE_WAIT_FOREVER, HEV *phSem = NULL);
	VOID post(HEV *phSem = NULL);
	VOID reset(HEV *phSem = NULL);

protected:
	HEV  sem_handle;
	BOOL fShared;
};

#endif //__VSEMAPHORE_H__
