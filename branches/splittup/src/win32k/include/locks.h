/* $Id: locks.h,v 1.1.2.1 2002-04-01 10:01:03 bird Exp $
 *
 * R/W spinlocks.
 *
 * Copyright (c) 2001-2002 knut st. osmundsen (bird@anduin.net)
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

