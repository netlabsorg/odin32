/* $Id: propsheet.h,v 1.1 1999-05-24 20:19:17 ktk Exp $ */

/*
 * Property sheet class extra info
 *
 * Copyright 1998 Anders Carlsson
 */

#ifndef __WINE_PROPSHEET_H  
#define __WINE_PROPSHEET_H

typedef struct tagPROPSHEET_INFO
{
    DWORD dwDummy;  /* just to keep the compiler happy ;-) */

} PROPSHEET_INFO, *LPPROPSHEET_INFO;



extern VOID PROPSHEET_Register (VOID);
extern VOID PROPSHEET_UnRegister (VOID);

#endif  /* __WINE_PROPSHEET_H */
