/* $Id: map.cpp,v 1.2 1999-06-10 20:48:00 phaller Exp $ */

/* Copyright (C) 1995 by Holger Veit (Holger.Veit@gmd.de)
 * Use at your own risk! No Warranty! The author is not responsible for
 * any damage or loss of data caused by proper or improper use of this
 * device driver.
 */

#define INCL_DOS
#define INCL_DOSDEVICES
#define INCL_DOSDEVIOCTL
#include <os2.h>

#include "map.h"
#include "mapos2.h"

int mpioctl(HFILE fd, int func,struct map_ioctl* data)
{
    struct xf86_pmap_param par;
    struct xf86_pmap_data dta;
    ULONG len;
    APIRET rc;

    switch (func) {
    case IOCTL_MAP:
        par.u.physaddr = data->a.phys;
        par.size = data->size;
        rc = DosDevIOCtl(fd, XFREE86_PMAP, PMAP_MAP,
            (ULONG*)&par, sizeof(par), &len,
            (ULONG*)&dta, sizeof(dta), &len);
        if (!rc) data->a.user = (void*)dta.addr;
        return rc ? -1 : 0;
    case IOCTL_UMAP:
        par.u.physaddr = (ULONG)data->a.user;
        par.size = 0;
        rc = DosDevIOCtl(fd, XFREE86_PMAP, PMAP_UNMAP,
            (ULONG*)&par, sizeof(par), &len,
            NULL, 0, NULL);
        return rc ? -1 : 0;
    default:
        return -1;
    }
}
