/* $Id: nativefont.h,v 1.1 1999-05-24 20:19:15 ktk Exp $ */

/*
 * Native font class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_NATIVEFONT_H
#define __WINE_NATIVEFONT_H

typedef struct tagNATIVEFONT_INFO
{
    DWORD  dwDummy;   /* just to keep the compiler happy ;-) */

} NATIVEFONT_INFO;


extern VOID NATIVEFONT_Register (VOID);
extern VOID NATIVEFONT_Unregister (VOID);

#endif  /* __WINE_NATIVEFONT_H */
