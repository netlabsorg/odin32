/* $Id: winproc.h,v 1.2 1999-12-09 09:49:41 sandervl Exp $ */

/*
 * Window procedure callbacks definitions
 *
 * Copyright 1996 Alexandre Julliard
 */

#ifndef __WINE_WINPROC_H
#define __WINE_WINPROC_H

#include "windef.h"

typedef enum
{
    WIN_PROC_INVALID,
    WIN_PROC_32A,
    WIN_PROC_32W
} WINDOWPROCTYPE;

typedef enum
{
    WIN_PROC_CLASS,
    WIN_PROC_WINDOW,
    WIN_PROC_TIMER
} WINDOWPROCUSER;

typedef void *HWINDOWPROC;  /* Really a pointer to a WINDOWPROC */

typedef struct
{
    WPARAM    wParam;
    LPARAM	lParam;
    LRESULT	lResult;
} MSGPARAM;

extern BOOL WINPROC_Init(void);
extern WNDPROC WINPROC_GetProc( HWINDOWPROC proc, WINDOWPROCTYPE type );
extern BOOL WINPROC_SetProc( HWINDOWPROC *pFirst, WNDPROC func,
                               WINDOWPROCTYPE type, WINDOWPROCUSER user );
extern void WINPROC_FreeProc( HWINDOWPROC proc, WINDOWPROCUSER user );
extern WINDOWPROCTYPE WINPROC_GetProcType( HWINDOWPROC proc );

LRESULT WINPROC_CallProc32ATo32W( WNDPROC func, HWND hwnd,
                                  UINT msg, WPARAM wParam,
                                  LPARAM lParam );

LRESULT WINPROC_CallProc32WTo32A( WNDPROC func, HWND hwnd,
                                  UINT msg, WPARAM wParam,
                                  LPARAM lParam );

#endif  /* __WINE_WINPROC_H */
