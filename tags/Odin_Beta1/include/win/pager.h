/* $Id: pager.h,v 1.3 2001-03-31 13:23:33 sandervl Exp $ */

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


#endif  /* __WINE_PAGER_H */
