/* $Id: cursorgrp.h,v 1.1 1999-10-06 18:50:51 bird Exp $ */

/*
 * PE2LX cursor group
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __CURSORGRP_H__
#define __CURSORGRP_H__

BOOL ShowGroupCursor(LXHeaderSuper &OS2Exe, int id, CursorHeader *chdr, int size);
void *ConvertGroupCursor(CursorHeader *chdr, int size, PULONG ulSize);
ULONG QuerySizeGroupCursor(CursorHeader *chdr, int size);

#endif