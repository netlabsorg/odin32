/* $Id: winproc.cpp,v 1.1 2001-05-11 08:36:31 sandervl Exp $ */
/*
 * Window procedure callbacks
 *
 * Copyright 1999 Sander van Leeuwen (OS/2 Port)
 *
 * Based on Wine Windows code (windows\winproc.c) 991114
 *
 *
 * Copyright 1995 Martin von Loewis
 * Copyright 1996 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include <string.h>
#include <win\winproc.h>
#include <win\debugtools.h>
#include <heapcode.h>
#include "win32wbase.h"

#define DBG_LOCALLOG	DBG_winproc
#include "dbglocal.h"

DECLARE_DEBUG_CHANNEL(relay)


/* Simple jmp to call 32-bit procedure directly */
#pragma pack(1)
typedef struct
{
    BYTE       jmp;                  /* jmp  proc (relative jump) */
    WNDPROC  proc WINE_PACKED;
} WINPROC_JUMP;
#pragma pack()

typedef struct tagWINDOWPROC
{
    WINPROC_JUMP          jmp;      /* Jump */
    struct tagWINDOWPROC *next;     /* Next window proc */
    UINT                magic;    /* Magic number */
    WINDOWPROCTYPE        type;     /* Function type */
    WINDOWPROCUSER        user;     /* Function user */
} WINDOWPROC;

#define WINPROC_MAGIC  ('W' | ('P' << 8) | ('R' << 16) | ('C' << 24))

#define WINPROC_THUNKPROC(pproc) \
            (WNDPROC)((LONG)(pproc)->jmp.proc + (LONG)(&(pproc)->jmp.proc+1))

/**********************************************************************
 *           WINPROC_GetPtr
 *
 * Return a pointer to the win proc.
 */
static WINDOWPROC *WINPROC_GetPtr( WNDPROC handle )
{
    BYTE *ptr;
    WINDOWPROC *proc;

    ptr = (BYTE *)handle;
    if(ptr == NULL) {
	DebugInt3();
        return NULL;
    }

    /* First check if it is the jmp address */
//    if (*ptr == 0xe9 /* jmp */) ptr -= (int)&((WINDOWPROC *)0)->jmp -
//                                       (int)&((WINDOWPROC *)0)->thunk;

    /* Now we have a pointer to the WINDOWPROC struct */
    if (((WINDOWPROC *)ptr)->magic == WINPROC_MAGIC)
         return (WINDOWPROC *)ptr;

    return NULL;
}


/**********************************************************************
 *           WINPROC_AllocWinProc
 *
 * Allocate a new window procedure.
 */
static WINDOWPROC *WINPROC_AllocWinProc( WNDPROC func, WINDOWPROCTYPE type,
                                         WINDOWPROCUSER user )
{
    WINDOWPROC *proc, *oldproc;

    /* Allocate a window procedure */

    if (!(proc = (WINDOWPROC *)_cmalloc(sizeof(WINDOWPROC) ))) return 0;

    /* Check if the function is already a win proc */

    oldproc = WINPROC_GetPtr( func );
    if (oldproc)
    {
        *proc = *oldproc;
    }
    else
    {
        switch(type)
        {
        case WIN_PROC_32A:
        case WIN_PROC_32W:
            proc->jmp.jmp  = 0xe9;
            /* Fixup relative jump */
            proc->jmp.proc = (WNDPROC)((LONG)func - (LONG)(&proc->jmp.proc+1));
            break;
        default:
            /* Should not happen */
            break;
        }
        proc->magic = WINPROC_MAGIC;
        proc->type  = type;
        proc->user  = user;
    }
    proc->next  = NULL;
    return proc;
}


/**********************************************************************
 *           WINPROC_GetProc
 *
 * Get a window procedure pointer that can be passed to the Windows program.
 */
WNDPROC WINPROC_GetProc( HWINDOWPROC proc, WINDOWPROCTYPE type )
{
 WINDOWPROC *lpProc = (WINDOWPROC *)proc;

    if(!lpProc) {
	DebugInt3();
	return NULL;
    }

    if(type != lpProc->type) {
    	 /* Have to return the jmp address if types don't match */
         return (WNDPROC)&lpProc->jmp;
    }
    else return WINPROC_THUNKPROC(lpProc); //return original window proc address if types match
}


/**********************************************************************
 *           WINPROC_SetProc
 *
 * Set the window procedure for a window or class. There are
 * three tree classes of winproc callbacks:
 *
 * 1) class  -> wp                      -       not subclassed
 *    class  -> wp -> wp -> wp -> wp    -       SetClassLong()
 *             /           /
 * 2) window -'           /             -       not subclassed
 *    window -> wp -> wp '              -       SetWindowLong()
 *
 * 3) timer  -> wp                      -       SetTimer()
 *
 * Initially, winproc of the window points to the current winproc
 * thunk of its class. Subclassing prepends a new thunk to the
 * window winproc chain at the head of the list. Thus, window thunk
 * list includes class thunks and the latter are preserved when the
 * window is destroyed.
 *
 */
BOOL WINPROC_SetProc( HWINDOWPROC *pFirst, WNDPROC func,
                      WINDOWPROCTYPE type, WINDOWPROCUSER user )
{
    BOOL bRecycle = FALSE;
    WINDOWPROC *proc, **ppPrev;

    if(func == NULL) {
        *(WINDOWPROC **)pFirst = 0;
        return TRUE;
    }

    /* Check if function is already in the list */

    ppPrev = (WINDOWPROC **)pFirst;
    proc = WINPROC_GetPtr( func );
    while (*ppPrev)
    {
        if (proc)
        {
            if (*ppPrev == proc)
            {
                if ((*ppPrev)->user != user)
                {
                    /* terminal thunk is being restored */

                    WINPROC_FreeProc( *pFirst, (*ppPrev)->user );
                    *(WINDOWPROC **)pFirst = *ppPrev;
                    return TRUE;
                }
                bRecycle = TRUE;
                break;
            }
        }
        else
        {
            if (((*ppPrev)->type == type) &&
                (func == WINPROC_THUNKPROC(*ppPrev)))
            {
                bRecycle = TRUE;
                break;
            }
        }

        /* WPF_CLASS thunk terminates window thunk list */
        if ((*ppPrev)->user != user) break;
        ppPrev = &(*ppPrev)->next;
    }

    if (bRecycle)
    {
        /* Extract this thunk from the list */
        proc = *ppPrev;
        *ppPrev = proc->next;
    }
    else  /* Allocate a new one */
    {
        if (proc)  /* Was already a win proc */
        {
            type = proc->type;
            func = WINPROC_THUNKPROC(proc);
        }
        proc = WINPROC_AllocWinProc( func, type, user );
        if (!proc) return FALSE;
    }

    /* Add the win proc at the head of the list */

    proc->next  = *(WINDOWPROC **)pFirst;
    *(WINDOWPROC **)pFirst = proc;
    return TRUE;
}


/**********************************************************************
 *           WINPROC_FreeProc
 *
 * Free a list of win procs.
 */
void WINPROC_FreeProc( HWINDOWPROC proc, WINDOWPROCUSER user )
{
    while (proc)
    {
        WINDOWPROC *next = ((WINDOWPROC *)proc)->next;
        if (((WINDOWPROC *)proc)->user != user) break;
        free( proc );
        proc = next;
    }
}


/**********************************************************************
 *           WINPROC_GetProcType
 *
 * Return the window procedure type.
 */
WINDOWPROCTYPE WINPROC_GetProcType( HWINDOWPROC proc )
{
    if (!proc ||
        (((WINDOWPROC *)proc)->magic != WINPROC_MAGIC))
        return WIN_PROC_INVALID;
    return ((WINDOWPROC *)proc)->type;
}

/**********************************************************************
 *           CallWindowProc32A    (USER32.18)
 *
 * The CallWindowProc() function invokes the windows procedure _func_,
 * with _hwnd_ as the target window, the message specified by _msg_, and
 * the message parameters _wParam_ and _lParam_.
 *
 * Some kinds of argument conversion may be done, I'm not sure what.
 *
 * CallWindowProc() may be used for windows subclassing. Use
 * SetWindowLong() to set a new windows procedure for windows of the
 * subclass, and handle subclassed messages in the new windows
 * procedure. The new windows procedure may then use CallWindowProc()
 * with _func_ set to the parent class's windows procedure to dispatch
 * the message to the superclass.
 *
 * RETURNS
 *
 *    The return value is message dependent.
 *
 * CONFORMANCE
 *
 *   ECMA-234, Win32
 */
LRESULT WINAPI CallWindowProcA(
    WNDPROC func, /* window procedure */
    HWND hwnd, /* target window */
    UINT msg,  /* message */
    WPARAM wParam, /* message dependent parameter */
    LPARAM lParam    /* message dependent parameter */
) {
    WINDOWPROC *proc = WINPROC_GetPtr( func );

    if(proc) {
          dprintf2(("CallWindowProcA %x %x %x %x %x -> proc %x, type %d, org func %x", func, hwnd, msg, wParam, lParam, proc, proc->type, WINPROC_THUNKPROC(proc)));
    }
    else  dprintf2(("CallWindowProcA %x %x %x %x %x (unknown proc)", func, hwnd, msg, wParam, lParam));
   
    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("CallWindowProcA, window %x not found", hwnd));
        // return func( hwnd, msg, wParam, lParam );
        return 0;
    }

    if (!proc) return func(hwnd, msg, wParam, lParam );

    switch(proc->type)
    {
    case WIN_PROC_32A:
        return func(hwnd, msg, wParam, lParam );
    case WIN_PROC_32W:
        return WINPROC_CallProc32ATo32W( func, hwnd, msg, wParam, lParam );
    default:
        WARN_(relay)("Invalid proc %p\n", proc );
        return 0;
    }
}


/**********************************************************************
 *           CallWindowProc32W    (USER32.19)
 */
LRESULT WINAPI CallWindowProcW( WNDPROC func, HWND hwnd, UINT msg,
                                WPARAM wParam, LPARAM lParam )
{
    WINDOWPROC *proc = WINPROC_GetPtr( func );

    if(proc) {
          dprintf2(("CallWindowProcW %x %x %x %x %x -> proc %x, type %d, org func %x", func, hwnd, msg, wParam, lParam, proc, proc->type, WINPROC_THUNKPROC(proc)));
    }
    else  dprintf2(("CallWindowProcW %x %x %x %x %x (unknown proc)", func, hwnd, msg, wParam, lParam));

    Win32BaseWindow *window = Win32BaseWindow::GetWindowFromHandle(hwnd);
    if(!window) {
        dprintf(("CallWindowProcW, window %x not found", hwnd));
        // return func( hwnd, msg, wParam, lParam );
        return 0;
    }

    if (!proc) return func( hwnd, msg, wParam, lParam );

    switch(proc->type)
    {
    case WIN_PROC_32A:
        return WINPROC_CallProc32WTo32A( func, hwnd, msg, wParam, lParam );
    case WIN_PROC_32W:
        return func(hwnd, msg, wParam, lParam );
    default:
        WARN_(relay)("Invalid proc %p\n", proc );
        return 0;
    }
}

