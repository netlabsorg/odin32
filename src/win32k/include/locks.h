/* $Id: locks.h,v 1.1 2001-02-10 11:15:41 bird Exp $
 *
 * R/W spinlocks.
 *
 * Copyright (c) 2001 knut st. osmundsen (knut.stange.osmundsen@mynd.no)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*NOINC*/
#ifndef _LOCKS_H_
#define _LOCKS_H_
/*INC*/

typedef unsigned long RWLOCK;
typedef RWLOCK * PRWLOCK;

APIRET _Optlink RWLockAcquireRead(PRWLOCK pLock);
APIRET _Optlink RWLockReleaseRead(PRWLOCK pLock);
APIRET _Optlink RWLockAcquireWrite(PRWLOCK pLock);
APIRET _Optlink RWLockReleaseWrite(PRWLOCK pLock);

/*NOINC*/
#endif
/*INC*/

