/* $Id: pen.h,v 1.1 1999-05-24 20:19:17 ktk Exp $ */

/*
 * GDI pen definitions
 *
 * Copyright 1994 Alexandre Julliard
 */

#ifndef __WINE_PEN_H
#define __WINE_PEN_H

#include "gdi.h"

  /* GDI logical pen object */
typedef struct
{
    GDIOBJHDR   header;
    LOGPEN    logpen;
} PENOBJ;

extern INT16 PEN_GetObject16( PENOBJ * pen, INT16 count, LPSTR buffer );
extern INT PEN_GetObject( PENOBJ * pen, INT count, LPSTR buffer );

#endif  /* __WINE_PEN_H */
