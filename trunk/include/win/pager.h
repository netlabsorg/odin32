/* $Id: pager.h,v 1.2 2000-03-17 17:10:52 cbratschi Exp $ */

/*
 * Pager class extra info
 *
 * Copyright 1998 Eric Kohl
 */

#ifndef __WINE_PAGER_H
#define __WINE_PAGER_H


typedef struct tagPAGER_INFO
{
    COMCTL32_HEADER header;

    HWND   hwndChild;
    COLORREF clrBk;
    INT    nBorder;
    INT    nButtonSize;
    INT    nPos;
    BOOL   bForward;

    INT    nChildSize;

} PAGER_INFO;


extern VOID PAGER_Register (VOID);
extern VOID PAGER_Unregister (VOID);

#endif  /* __WINE_PAGER_H */
