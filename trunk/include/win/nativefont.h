/* $Id: nativefont.h,v 1.2 2000-03-17 17:10:52 cbratschi Exp $ */

/*
 * Native font class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_NATIVEFONT_H
#define __WINE_NATIVEFONT_H

typedef struct tagNATIVEFONT_INFO
{
    COMCTL32_HEADER header;
} NATIVEFONT_INFO;


extern VOID NATIVEFONT_Register (VOID);
extern VOID NATIVEFONT_Unregister (VOID);

#endif  /* __WINE_NATIVEFONT_H */
