/* $Id: tweak.h,v 1.1 1999-11-28 12:33:11 sandervl Exp $ */
/******************************************************************************
 *
 *   Wine Windows 95 interface tweaks
 *
 *   Copyright (c) 1997 Dave Cuthbert (dacut@ece.cmu.edu)
 *
 *****************************************************************************/

#ifndef __WINE_TWEAK_H
#define __WINE_TWEAK_H

#include "windef.h"

typedef enum
{
    WIN31_LOOK,
    WIN95_LOOK,
    WIN98_LOOK
} WINE_LOOK;


int  TWEAK_Init(void);
int  TWEAK_CheckConfiguration(void);

extern WINE_LOOK TWEAK_WineLook;

#endif /* __WINE_TWEAK_H */
