/* $Id: icongrp.h,v 1.1 1999-09-06 02:20:05 bird Exp $ */

/*
 * PE2LX icon groups
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 * Copyright 1998 knut st. osmundsen (knut.stange.osmundsen@pmsc.no)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __ICONGRP_H__
#define __ICONGRP_H__

BOOL  ShowGroupIcon(LXHeaderSuper &OS2Exe, int id, IconHeader *ihdr, int size);
void *ConvertGroupIcon(LXHeaderSuper &OS2Exe, int id, IconHeader *ihdr, int size, PULONG pulSize);
ULONG QuerySizeGroupIcon(IconHeader *ihdr, int size);

#endif