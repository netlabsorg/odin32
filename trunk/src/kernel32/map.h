/* $Id: map.h,v 1.3 1999-06-10 19:11:30 phaller Exp $ */

/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de)
 * Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */
#ifndef _MAP_H_
#define _MAP_H_

/* must include <os2.h> first */

#define IOCTL_MAP   1
#define IOCTL_UMAP  2

struct map_ioctl {
    union {
        ULONG   phys;
        void*   user;
    } a;
    ULONG   size;
};

extern int mpioctl(HFILE fd, int func, struct map_ioctl* data);

#endif
