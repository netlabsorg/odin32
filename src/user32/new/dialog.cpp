/*
 * Dialog functions
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 */

#include <os2win.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <heapstring.h>
#include <winuser.h>
//#include "windowsx.h"
//#include "wine/winuser16.h"
//#include "wine/winbase16.h"
#include "dialog.h"
//#include "drive.h"
#include "heap.h"
#include "win.h"
#include "ldt.h"
#include "user.h"
#include "winproc.h"
//#include "message.h"
//#include "debugtools.h"

//ODINDEBUGCHANNEL(USER32-DIALOG)


  /* Dialog control information */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    INT16      x;
    INT16      y;
    INT16      cx;
    INT16      cy;
    UINT     id;
    LPCSTR     className;
    LPCSTR     windowName;
    LPVOID     data;
} DLG_CONTROL_INFO;

  /* Dialog template */
typedef struct
{
    DWORD      style;
    DWORD      exStyle;
    DWORD      helpId;
    UINT16     nbItems;
    INT16      x;
    INT16      y;
    INT16      cx;
    INT16      cy;
    LPCSTR     menuName;
    LPCSTR     className;
    LPCSTR     caption;
    WORD       pointSize;
    WORD       weight;
    BOOL     italic;
    LPCSTR     faceName;
    BOOL     dialogEx;
} DLG_TEMPLATE;

  /* Radio button group */
typedef struct
{
    UINT firstID;
    UINT lastID;
    UINT checkID;
} RADIOGROUP;

  /* Dialog base units */
static WORD xBaseUnit = 0, yBaseUnit = 0;


/***********************************************************************
 *           DIALOG_GetCharSizeFromDC
 *
 *
 *  Calculates the *true* average size of English characters in the
 *  specified font as oppposed to the one returned by GetTextMetrics.
 */
static BOOL DIALOG_GetCharSizeFromDC( HDC hDC, HFONT hFont, SIZE * pSize )
{
    BOOL Success = FALSE;
    HFONT hFontPrev = 0;
    pSize->cx = xBaseUnit;
    pSize->cy = yBaseUnit;
    if ( hDC )
    {
        /* select the font */
        TEXTMETRICA tm;
        memset(&tm,0,sizeof(tm));
        if (hFont) hFontPrev = SelectFont(hDC,hFont);
        if (GetTextMetricsA(hDC,&tm))
        {
            pSize->cx = tm.tmAveCharWidth;
            pSize->cy = tm.tmHeight;

            /* if variable width font */
            if (tm.tmPitchAndFamily & TMPF_FIXED_PITCH)
            {
                SIZE total;
                static const char szAvgChars[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

                /* Calculate a true average as opposed to the one returned
                 * by tmAveCharWidth. This works better when dealing with
                 * proportional spaced fonts and (more important) that's
                 * how Microsoft's dialog creation code calculates the size
                 * of the font
                 */
                if (GetTextExtentPointA(hDC,szAvgChars,sizeof(szAvgChars),&total))
                {
                   /* round up */
                    pSize->cx = ((2*total.cx/sizeof(szAvgChars)) + 1)/2;
                    Success = TRUE;
                }
            }
            else
            {
                Success = TRUE;
            }
        }

        /* select the original font */
        if (hFontPrev) SelectFont(hDC,hFontPrev);
    }
    return (Success);
}


/***********************************************************************
 *           DIALOG_GetCharSize
 *
 *
 *  Calculates the *true* average size of English characters in the
 *  specified font as oppposed to the one returned by GetTextMetrics.
 *  A convenient variant of DIALOG_GetCharSizeFromDC.
 */
static BOOL DIALOG_GetCharSize( HFONT hFont, SIZE * pSize )
{
    HDC  hDC = GetDC(0);
    BOOL Success = DIALOG_GetCharSizeFromDC( hDC, hFont, pSize );
    ReleaseDC(0, hDC);
    return Success;
}


/***********************************************************************
 *           DIALOG_Init
 *
 * Initialisation of the dialog manager.
 */
BOOL DIALOG_Init(void)
{
    HDC16 hdc;
    SIZE size;

      /* Calculate the dialog base units */

    if (!(hdc = CreateDC16( "DISPLAY", NULL, NULL, NULL ))) return FALSE;
    if (!DIALOG_GetCharSizeFromDC( hdc, 0, &size )) return FALSE;
    DeleteDC( hdc );
    xBaseUnit = size.cx;
    yBaseUnit = size.cy;

    dprintf(("USER32: base units = %d,%d\n", xBaseUnit, yBaseUnit ));
    return TRUE;
}


/***********************************************************************
 *           DIALOG_GetControl16
 *
 * Return the class and text of the control pointed to by ptr,
 * fill the header structure and return a pointer to the next control.
 */
static LPCSTR DIALOG_GetControl16( LPCSTR p, DLG_CONTROL_INFO *info )
{
    static char buffer[10];
    int int_id;

    info->x       = GET_WORD(p);  p += sizeof(WORD);
    info->y       = GET_WORD(p);  p += sizeof(WORD);
    info->cx      = GET_WORD(p);  p += sizeof(WORD);
    info->cy      = GET_WORD(p);  p += sizeof(WORD);
    info->id      = GET_WORD(p);  p += sizeof(WORD);
    info->style   = GET_DWORD(p); p += sizeof(DWORD);
    info->exStyle = 0;

    if (*p & 0x80)
    {
        switch((BYTE)*p)
        {
            case 0x80: strcpy( buffer, "BUTTON" ); break;
            case 0x81: strcpy( buffer, "EDIT" ); break;
            case 0x82: strcpy( buffer, "STATIC" ); break;
            case 0x83: strcpy( buffer, "LISTBOX" ); break;
            case 0x84: strcpy( buffer, "SCROLLBAR" ); break;
            case 0x85: strcpy( buffer, "COMBOBOX" ); break;
            default:   buffer[0] = '\0'; break;
        }
        info->className = buffer;
        p++;
    }
    else
    {
   info->className = p;
   p += strlen(p) + 1;
    }

    int_id = ((BYTE)*p == 0xff);
    if (int_id)
    {
     /* Integer id, not documented (?). Only works for SS_ICON controls */
   info->windowName = (LPCSTR)(UINT)GET_WORD(p+1);
   p += 3;
    }
    else
    {
   info->windowName = p;
   p += strlen(p) + 1;
    }

    info->data = (LPVOID)(*p ? p + 1 : NULL);  /* FIXME: should be a segptr */
    p += *p + 1;

    if(int_id)
      dprintf(("USER32:   %s %04x %d, %d, %d, %d, %d, %08lx, %08lx\n",
            info->className,  LOWORD(info->windowName),
            info->id, info->x, info->y, info->cx, info->cy,
            info->style, (DWORD)info->data));
    else
      dprintf(("USER32:   %s '%s' %d, %d, %d, %d, %d, %08lx, %08lx\n",
            info->className,  info->windowName,
            info->id, info->x, info->y, info->cx, info->cy,
            info->style, (DWORD)info->data));

    return p;
}


/***********************************************************************
 *           DIALOG_GetControl32
 *
 * Return the class and text of the control pointed to by ptr,
 * fill the header structure and return a pointer to the next control.
 */
static const WORD *DIALOG_GetControl32( const WORD *p, DLG_CONTROL_INFO *info,
                                        BOOL dialogEx )
{
    if (dialogEx)
    {
        info->helpId  = GET_DWORD(p); p += 2;
        info->exStyle = GET_DWORD(p); p += 2;
        info->style   = GET_DWORD(p); p += 2;
    }
    else
    {
        info->helpId  = 0;
        info->style   = GET_DWORD(p); p += 2;
        info->exStyle = GET_DWORD(p); p += 2;
    }
    info->x       = GET_WORD(p); p++;
    info->y       = GET_WORD(p); p++;
    info->cx      = GET_WORD(p); p++;
    info->cy      = GET_WORD(p); p++;

    if (dialogEx)
    {
        /* id is a DWORD for DIALOGEX */
        info->id = GET_DWORD(p);
        p += 2;
    }
    else
    {
        info->id = GET_WORD(p);
        p++;
    }

    if (GET_WORD(p) == 0xffff)
    {
        static const WCHAR class_names[6][10] =
        {
            { 'B','u','t','t','o','n', },             /* 0x80 */
            { 'E','d','i','t', },                     /* 0x81 */
            { 'S','t','a','t','i','c', },             /* 0x82 */
            { 'L','i','s','t','B','o','x', },         /* 0x83 */
            { 'S','c','r','o','l','l','B','a','r', }, /* 0x84 */
            { 'C','o','m','b','o','B','o','x', }      /* 0x85 */
        };
        WORD id = GET_WORD(p+1);
        if ((id >= 0x80) && (id <= 0x85))
            info->className = (LPCSTR)class_names[id - 0x80];
        else
        {
            info->className = NULL;
            dprintf(("USER32: Error-Unknown built-in class id %04x\n", id ));
        }
        p += 2;
    }
    else
    {
        info->className = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
    }

    if (GET_WORD(p) == 0xffff)  /* Is it an integer id? */
    {
   info->windowName = (LPCSTR)(UINT)GET_WORD(p + 1);
   p += 2;
    }
    else
    {
   info->windowName = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
    }

    dprintf(("USER32:    %ls %ls %d, %d, %d, %d, %d, %08lx, %08lx, %08lx\n",
          (LPCWSTR)info->className,
          (LPCWSTR)info->windowName,
          info->id, info->x, info->y, info->cx, info->cy,
          info->style, info->exStyle, info->helpId ));

    if (GET_WORD(p))
    {
        info->data = (LPVOID)(p + 1);
        p += GET_WORD(p) / sizeof(WORD);
    }
    else info->data = NULL;
    p++;

    /* Next control is on dword boundary */
    return (const WORD *)((((int)p) + 3) & ~3);
}


/***********************************************************************
 *           DIALOG_CreateControls
 *
 * Create the control windows for a dialog.
 */
static BOOL DIALOG_CreateControls( WND *pWnd, LPCSTR strTemplate,
                                     const DLG_TEMPLATE *dlgTemplate,
                                     HINSTANCE hInst, BOOL win32 )
{
    DIALOGINFO *dlgInfo = (DIALOGINFO *)pWnd->wExtra;
    DLG_CONTROL_INFO info;
    HWND hwndCtrl, hwndDefButton = 0;
    INT items = dlgTemplate->nbItems;

    dprintf(("USER32: BEGIN\n" ));
    while (items--)
    {
        {
            strTemplate = (LPCSTR)DIALOG_GetControl32( (WORD *)strTemplate, &info,
                                                    dlgTemplate->dialogEx );
            hwndCtrl = CreateWindowExW( info.exStyle | WS_EX_NOPARENTNOTIFY,
                                          (LPCWSTR)info.className,
                                          (LPCWSTR)info.windowName,
                                          info.style | WS_CHILD,
                                          info.x * dlgInfo->xBaseUnit / 4,
                                          info.y * dlgInfo->yBaseUnit / 8,
                                          info.cx * dlgInfo->xBaseUnit / 4,
                                          info.cy * dlgInfo->yBaseUnit / 8,
                                          pWnd->hwndSelf, (HMENU)info.id,
                                          hInst, info.data );
        }
        if (!hwndCtrl) return FALSE;

            /* Send initialisation messages to the control */
        if (dlgInfo->hUserFont) SendMessageA( hwndCtrl, WM_SETFONT,
                                             (WPARAM)dlgInfo->hUserFont, 0 );
        if (SendMessageA(hwndCtrl, WM_GETDLGCODE, 0, 0) & DLGC_DEFPUSHBUTTON)
        {
              /* If there's already a default push-button, set it back */
              /* to normal and use this one instead. */
            if (hwndDefButton)
                SendMessageA( hwndDefButton, BM_SETSTYLE,
                                BS_PUSHBUTTON,FALSE );
            hwndDefButton = hwndCtrl;
            dlgInfo->idResult = GetWindowWord( hwndCtrl, GWW_ID );
        }
    }
    dprintf(("USER32: END\n" ));
    return TRUE;
}


/***********************************************************************
 *           DIALOG_ParseTemplate32
 *
 * Fill a DLG_TEMPLATE structure from the dialog template, and return
 * a pointer to the first control.
 */
static LPCSTR DIALOG_ParseTemplate32( LPCSTR strTemplate, DLG_TEMPLATE * result )
{
    const WORD *p = (const WORD *)strTemplate;

    result->style = GET_DWORD(p); p += 2;
    if (result->style == 0xffff0001)  /* DIALOGEX resource */
    {
        result->dialogEx = TRUE;
        result->helpId   = GET_DWORD(p); p += 2;
        result->exStyle  = GET_DWORD(p); p += 2;
        result->style    = GET_DWORD(p); p += 2;
    }
    else
    {
        result->dialogEx = FALSE;
        result->helpId   = 0;
        result->exStyle  = GET_DWORD(p); p += 2;
    }
    result->nbItems = GET_WORD(p); p++;
    result->x       = GET_WORD(p); p++;
    result->y       = GET_WORD(p); p++;
    result->cx      = GET_WORD(p); p++;
    result->cy      = GET_WORD(p); p++;
    dprintf(("USER32:DIALOG%s %d, %d, %d, %d, %ld\n",
           result->dialogEx ? "EX" : "", result->x, result->y,
           result->cx, result->cy, result->helpId ));
    dprintf(("USER32: STYLE 0x%08lx\n", result->style ));
    dprintf(("USER32: EXSTYLE 0x%08lx\n", result->exStyle ));

    /* Get the menu name */

    switch(GET_WORD(p))
    {
    case 0x0000:
        result->menuName = NULL;
        p++;
        break;
    case 0xffff:
        result->menuName = (LPCSTR)(UINT)GET_WORD( p + 1 );
        p += 2;
   dprintf(("USER32: MENU %04x\n", LOWORD(result->menuName) ));
        break;
    default:
        result->menuName = (LPCSTR)p;
        dprintf(("USER32: MENU %ls\n", (LPCWSTR)p ));
        p += lstrlenW( (LPCWSTR)p ) + 1;
        break;
    }

    /* Get the class name */

    switch(GET_WORD(p))
    {
    case 0x0000:
        result->className = DIALOG_CLASS_ATOM;
        p++;
        break;
    case 0xffff:
        result->className = (LPCSTR)(UINT)GET_WORD( p + 1 );
        p += 2;
   dprintf(("USER32: CLASS %04x\n", LOWORD(result->className) ));
        break;
    default:
        result->className = (LPCSTR)p;
        dprintf(("USER32: CLASS %ls\n", (LPCWSTR)p ));
        p += lstrlenW( (LPCWSTR)p ) + 1;
        break;
    }

    /* Get the window caption */

    result->caption = (LPCSTR)p;
    p += lstrlenW( (LPCWSTR)p ) + 1;
    dprintf(("USER32: CAPTION %ls\n", (LPCWSTR)result->caption ));

    /* Get the font name */

    if (result->style & DS_SETFONT)
    {
   result->pointSize = GET_WORD(p);
        p++;
        if (result->dialogEx)
        {
            result->weight = GET_WORD(p); p++;
            result->italic = LOBYTE(GET_WORD(p)); p++;
        }
        else
        {
            result->weight = FW_DONTCARE;
            result->italic = FALSE;
        }
   result->faceName = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
   dprintf(("USER32: FONT %d, %ls, %d, %s\n",
              result->pointSize, (LPCWSTR)result->faceName,
              result->weight, result->italic ? "TRUE" : "FALSE" ));
    }

    /* First control is on dword boundary */
    return (LPCSTR)((((int)p) + 3) & ~3);
}


/***********************************************************************
 *           DIALOG_CreateIndirect
 */
HWND DIALOG_CreateIndirect( HINSTANCE hInst, LPCSTR dlgTemplate,
                              BOOL win32Template, HWND owner,
                              DLGPROC16 dlgProc, LPARAM param,
                              WINDOWPROCTYPE procType )
{
    HMENU16 hMenu = 0;
    HFONT16 hFont = 0;
    HWND hwnd;
    RECT rect;
    WND * wndPtr;
    DLG_TEMPLATE strTemplate;
    DIALOGINFO * dlgInfo;
    WORD xUnit = xBaseUnit;
    WORD yUnit = yBaseUnit;

      /* Parse dialog template */

    if (!dlgTemplate) return 0;
    dlgTemplate = DIALOG_ParseTemplate32( dlgTemplate, &strTemplate );

      /* Load menu */

    if (strTemplate.menuName)
      hMenu = LoadMenuW( hInst, (LPCWSTR)strTemplate.menuName );

      /* Create custom font if needed */

    if (strTemplate.style & DS_SETFONT)
    {
          /* The font height must be negative as it is a point size */
          /* (see CreateFont() documentation in the Windows SDK).   */

   if (win32Template)
       hFont = CreateFontW( -strTemplate.pointSize, 0, 0, 0,
                                   strTemplate.weight, strTemplate.italic, FALSE,
                                   FALSE, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY,
                                   FF_DONTCARE, (LPCWSTR)strTemplate.faceName );
   else
       hFont = CreateFont16( -strTemplate.pointSize, 0, 0, 0, FW_DONTCARE,
              FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0,
              PROOF_QUALITY, FF_DONTCARE,
              strTemplate.faceName );
        if (hFont)
        {
            SIZE charSize;
            DIALOG_GetCharSize(hFont,&charSize);
            xUnit = charSize.cx;
            yUnit = charSize.cy;
        }
    }

    /* Create dialog main window */

    rect.left = rect.top = 0;
    rect.right = strTemplate.cx * xUnit / 4;
    rect.bottom = strTemplate.cy * yUnit / 8;
    if (strTemplate.style & DS_MODALFRAME)
        strTemplate.exStyle |= WS_EX_DLGMODALFRAME;
    AdjustWindowRectEx( &rect, strTemplate.style,
                          hMenu ? TRUE : FALSE , strTemplate.exStyle );
    rect.right -= rect.left;
    rect.bottom -= rect.top;

    if ((INT16)strTemplate.x == CW_USEDEFAULT16)
    {
        rect.left = rect.top = win32Template? CW_USEDEFAULT : CW_USEDEFAULT16;
    }
    else
    {
        if (strTemplate.style & DS_CENTER)
        {
            rect.left = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
            rect.top = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
        }
        else
        {
            rect.left += strTemplate.x * xUnit / 4;
            rect.top += strTemplate.y * yUnit / 8;
        }
        if ( !(strTemplate.style & WS_CHILD) )
   {
            INT16 dX, dY;

            if( !(strTemplate.style & DS_ABSALIGN) )
                ClientToScreen( owner, (POINT *)&rect );

            /* try to fit it into the desktop */

            if( (dX = rect.left + rect.right + GetSystemMetrics(SM_CXDLGFRAME)
                 - GetSystemMetrics(SM_CXSCREEN)) > 0 ) rect.left -= dX;
            if( (dY = rect.top + rect.bottom + GetSystemMetrics(SM_CYDLGFRAME)
                 - GetSystemMetrics(SM_CYSCREEN)) > 0 ) rect.top -= dY;
            if( rect.left < 0 ) rect.left = 0;
            if( rect.top < 0 ) rect.top = 0;
        }
    }

        hwnd = CreateWindowExW(strTemplate.exStyle, (LPCWSTR)strTemplate.className,
                                 (LPCWSTR)strTemplate.caption,
                                 strTemplate.style & ~WS_VISIBLE,
                                 rect.left, rect.top, rect.right, rect.bottom,
                                 owner, hMenu, hInst, NULL );

    if (!hwnd)
    {
   if (hFont) DeleteObject( hFont );
   if (hMenu) DestroyMenu( hMenu );
   return 0;
    }
    wndPtr = WIN_FindWndPtr( hwnd );
    wndPtr->flags |= WIN_ISDIALOG;
    wndPtr->helpContext = strTemplate.helpId;

      /* Initialise dialog extra data */

    dlgInfo = (DIALOGINFO *)wndPtr->wExtra;
    WINPROC_SetProc( &dlgInfo->dlgProc, (WNDPROC16)dlgProc, procType, WIN_PROC_WINDOW );
    dlgInfo->hUserFont = hFont;
    dlgInfo->hMenu     = hMenu;
    dlgInfo->xBaseUnit = xUnit;
    dlgInfo->yBaseUnit = yUnit;
    dlgInfo->msgResult = 0;
    dlgInfo->idResult  = 0;
    dlgInfo->flags     = 0;
    dlgInfo->hDialogHeap = 0;

    if (dlgInfo->hUserFont)
        SendMessageA( hwnd, WM_SETFONT, (WPARAM)dlgInfo->hUserFont, 0 );

    /* Create controls */

    if (DIALOG_CreateControls( wndPtr, dlgTemplate, &strTemplate,
                               hInst, win32Template ))
    {
       /* Send initialisation messages and set focus */

   dlgInfo->hwndFocus = GetNextDlgTabItem( hwnd, 0, FALSE );

   if (SendMessageA( hwnd, WM_INITDIALOG, (WPARAM)dlgInfo->hwndFocus, param ))
            SetFocus( dlgInfo->hwndFocus );

   if (strTemplate.style & WS_VISIBLE && !(wndPtr->dwStyle & WS_VISIBLE))
   {
      ShowWindow( hwnd, SW_SHOWNORMAL ); /* SW_SHOW doesn't always work */
      UpdateWindow( hwnd );
   }
        WIN_ReleaseWndPtr(wndPtr);
   return hwnd;
    }
    WIN_ReleaseWndPtr(wndPtr);
    if( IsWindow(hwnd) ) DestroyWindow( hwnd );
    return 0;
}


/***********************************************************************
 *           CreateDialogParam32A   (USER32.73)
 */
HWND WINAPI CreateDialogParamA( HINSTANCE hInst, LPCSTR name,
                                    HWND owner, DLGPROC dlgProc,
                                    LPARAM param )
{
    HANDLE hrsrc = FindResourceA( hInst, name, RT_DIALOGA );
    if (!hrsrc) return 0;
    return CreateDialogIndirectParamA( hInst,
                                         (DLGTEMPLATE*)LoadResource(hInst, hrsrc),
                                         owner, dlgProc, param );
}


/***********************************************************************
 *           CreateDialogParam32W   (USER32.74)
 */
HWND WINAPI CreateDialogParamW( HINSTANCE hInst, LPCWSTR name,
                                    HWND owner, DLGPROC dlgProc,
                                    LPARAM param )
{
    HANDLE hrsrc = FindResourceW( hInst, name, RT_DIALOGW );
    if (!hrsrc) return 0;
    return CreateDialogIndirectParamW( hInst,
                                         (DLGTEMPLATE*)LoadResource(hInst, hrsrc),
                                         owner, dlgProc, param );
}


/***********************************************************************
 *           CreateDialogIndirectParam32A   (USER32.69)
 */
HWND WINAPI CreateDialogIndirectParamA( HINSTANCE hInst,
                                            LPCVOID dlgTemplate,
                                            HWND owner, DLGPROC dlgProc,
                                            LPARAM param )
{
    return DIALOG_CreateIndirect( hInst, (LPCSTR)dlgTemplate, TRUE, owner,
                                  (DLGPROC16)dlgProc, param, WIN_PROC_32A );
}

/***********************************************************************
 *           CreateDialogIndirectParam32AorW   (USER32.71)
 */
HWND WINAPI CreateDialogIndirectParamAorW( HINSTANCE hInst,
                                            LPCVOID dlgTemplate,
                                            HWND owner, DLGPROC dlgProc,
                                            LPARAM param )
{
    dprintf(("USER32- FIXME-assume WIN_PROC_32W\n"));
    return DIALOG_CreateIndirect( hInst, (LPCSTR)dlgTemplate, TRUE, owner,
                                  (DLGPROC16)dlgProc, param, WIN_PROC_32W );
}

/***********************************************************************
 *           CreateDialogIndirectParam32W   (USER32.72)
 */
HWND WINAPI CreateDialogIndirectParamW( HINSTANCE hInst,
                                            LPCVOID dlgTemplate,
                                            HWND owner, DLGPROC dlgProc,
                                            LPARAM param )
{
    return DIALOG_CreateIndirect( hInst, (LPCSTR)dlgTemplate, TRUE, owner,
                                  (DLGPROC16)dlgProc, param, WIN_PROC_32W );
}


/***********************************************************************
 *           DIALOG_DoDialogBox
 */
INT DIALOG_DoDialogBox( HWND hwnd, HWND owner )
{
    WND * wndPtr;
    DIALOGINFO * dlgInfo;
    MSG msg;
    INT retval;

      /* Owner must be a top-level window */
    owner = WIN_GetTopParent( owner );
    if (!(wndPtr = WIN_FindWndPtr( hwnd ))) return -1;
    dlgInfo = (DIALOGINFO *)wndPtr->wExtra;

    if (!dlgInfo->flags & DF_END) /* was EndDialog called in WM_INITDIALOG ? */
    {
        EnableWindow( owner, FALSE );
        ShowWindow( hwnd, SW_SHOW );
        while (MSG_InternalGetMessage(&msg, hwnd, owner, MSGF_DIALOGBOX,
                                      PM_REMOVE, !(wndPtr->dwStyle & DS_NOIDLEMSG), NULL ))
        {
            if (!IsDialogMessageA( hwnd, &msg))
            {
                TranslateMessage( &msg );
                DispatchMessageA( &msg );
            }
            if (dlgInfo->flags & DF_END) break;
        }
        EnableWindow( owner, TRUE );
    }
    retval = dlgInfo->idResult;
    WIN_ReleaseWndPtr(wndPtr);
    DestroyWindow( hwnd );
    return retval;
}


/***********************************************************************
 *           DialogBoxParam32A   (USER32.139)
 */
INT WINAPI DialogBoxParamA( HINSTANCE hInst, LPCSTR name,
                                HWND owner, DLGPROC dlgProc, LPARAM param )
{
    HWND hwnd = CreateDialogParamA( hInst, name, owner, dlgProc, param );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}


/***********************************************************************
 *           DialogBoxParam32W   (USER32.140)
 */
INT WINAPI DialogBoxParamW( HINSTANCE hInst, LPCWSTR name,
                                HWND owner, DLGPROC dlgProc, LPARAM param )
{
    HWND hwnd = CreateDialogParamW( hInst, name, owner, dlgProc, param );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}


/***********************************************************************
 *           DialogBoxIndirectParam32A   (USER32.136)
 */
INT WINAPI DialogBoxIndirectParamA(HINSTANCE hInstance, LPCVOID strTemplate,
                                       HWND owner, DLGPROC dlgProc,
                                       LPARAM param )
{
    HWND hwnd = CreateDialogIndirectParamA( hInstance, strTemplate,
                                                owner, dlgProc, param );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}


/***********************************************************************
 *           DialogBoxIndirectParam32W   (USER32.138)
 */
INT WINAPI DialogBoxIndirectParamW(HINSTANCE hInstance, LPCVOID strTemplate,
                                       HWND owner, DLGPROC dlgProc,
                                       LPARAM param )
{
    HWND hwnd = CreateDialogIndirectParamW( hInstance, strTemplate,
                                                owner, dlgProc, param );
    if (hwnd) return DIALOG_DoDialogBox( hwnd, owner );
    return -1;
}


/***********************************************************************
 *           EndDialog32   (USER32.173)
 */
BOOL WINAPI EndDialog( HWND hwnd, INT retval )
{
    WND * wndPtr = WIN_FindWndPtr( hwnd );
    DIALOGINFO * dlgInfo = (DIALOGINFO *)wndPtr->wExtra;

    dprintf(("USER32:%04x %d\n", hwnd, retval ));

    if( dlgInfo )
    {
        dlgInfo->idResult = retval;
        dlgInfo->flags |= DF_END;
    }

    /* Windows sets the focus to the dialog itself first in EndDialog */

    if (IsChild(hwnd, GetFocus()))
       SetFocus(wndPtr->hwndSelf);

    /* Paint Shop Pro 4.14 calls EndDialog for a CreateDialog* dialog,
     * which isn't "normal". Only DialogBox* dialogs may be EndDialog()ed.
     * Just hide the window
     * and re-enable the owner as windows does it...
     */
    ShowWindow(hwnd, SW_HIDE);

    if(wndPtr->owner)
    {
   HWND hOwner;
   /* Owner must be a top-level window */
   hOwner = WIN_GetTopParent( wndPtr->owner->hwndSelf );
   EnableWindow( hOwner, TRUE );
    }

    WIN_ReleaseWndPtr(wndPtr);

    return TRUE;
}


/***********************************************************************
 *           DIALOG_IsAccelerator
 */
static BOOL DIALOG_IsAccelerator( HWND hwnd, HWND hwndDlg, WPARAM vKey )
{
    HWND hwndControl = hwnd;
    HWND hwndNext;
    WND *wndPtr;
    BOOL RetVal = FALSE;
    INT dlgCode;

    if (vKey == VK_SPACE)
    {
        dlgCode = SendMessageA( hwndControl, WM_GETDLGCODE, 0, 0 );
        if (dlgCode & DLGC_BUTTON)
        {
            SendMessageA( hwndControl, WM_LBUTTONDOWN, 0, 0);
            SendMessageA( hwndControl, WM_LBUTTONUP, 0, 0);
            RetVal = TRUE;
        }
    }
    else
    {
        do
        {
            wndPtr = WIN_FindWndPtr( hwndControl );
            if ( (wndPtr != NULL) &&
                 ((wndPtr->dwStyle & (WS_VISIBLE | WS_DISABLED)) == WS_VISIBLE) )
            {
                dlgCode = SendMessageA( hwndControl, WM_GETDLGCODE, 0, 0 );
                if ( (dlgCode & (DLGC_BUTTON | DLGC_STATIC)) &&
                     (wndPtr->text!=NULL))
                {
                    /* find the accelerator key */
                    LPSTR p = wndPtr->text - 2;
                    do
                    {
                        p = strchr( p + 2, '&' );
                    }
                    while (p != NULL && p[1] == '&');

                    /* and check if it's the one we're looking for */
                    if (p != NULL && toupper( p[1] ) == toupper( vKey ) )
                    {
                        if ((dlgCode & DLGC_STATIC) ||
                            (wndPtr->dwStyle & 0x0f) == BS_GROUPBOX )
                        {
                            /* set focus to the control */
                            SendMessageA( hwndDlg, WM_NEXTDLGCTL,
                                    hwndControl, 1);
                            /* and bump it on to next */
                            SendMessageA( hwndDlg, WM_NEXTDLGCTL, 0, 0);
                        }
                        else if (dlgCode &
             (DLGC_DEFPUSHBUTTON | DLGC_UNDEFPUSHBUTTON))
                        {
                            /* send command message as from the control */
                            SendMessageA( hwndDlg, WM_COMMAND,
                                MAKEWPARAM( LOWORD(wndPtr->wIDmenu),
                                    BN_CLICKED ),
                                (LPARAM)hwndControl );
                        }
                        else
                        {
                            /* click the control */
                            SendMessageA( hwndControl, WM_LBUTTONDOWN, 0, 0);
                            SendMessageA( hwndControl, WM_LBUTTONUP, 0, 0);
                        }
                        RetVal = TRUE;
         WIN_ReleaseWndPtr(wndPtr);
                        break;
                    }
                }
      hwndNext = GetWindow( hwndControl, GW_CHILD );
            }
       else
       {
      hwndNext = 0;
       }
            WIN_ReleaseWndPtr(wndPtr);
       if (!hwndNext)
       {
           hwndNext = GetWindow( hwndControl, GW_HWNDNEXT );
       }
       while (!hwndNext)
       {
      hwndControl = GetParent( hwndControl );
      if (hwndControl == hwndDlg)
      {
          if(hwnd==hwndDlg){  /* prevent endless loop */
              hwndNext=hwnd;
         break;
          }
          hwndNext = GetWindow( hwndDlg, GW_CHILD );
      }
      else
      {
          hwndNext = GetWindow( hwndControl, GW_HWNDNEXT );
      }
       }
            hwndControl = hwndNext;
        }
        while (hwndControl != hwnd);
    }
    return RetVal;
}


/***********************************************************************
 *           DIALOG_IsDialogMessage
 */
static BOOL DIALOG_IsDialogMessage( HWND hwnd, HWND hwndDlg,
                                      UINT message, WPARAM wParam,
                                      LPARAM lParam, BOOL *translate,
                                      BOOL *dispatch, INT dlgCode )
{
    *translate = *dispatch = FALSE;

    if (message == WM_PAINT)
    {
        /* Apparently, we have to handle this one as well */
        *dispatch = TRUE;
        return TRUE;
    }

      /* Only the key messages get special processing */
    if ((message != WM_KEYDOWN) &&
        (message != WM_SYSCHAR) &&
   (message != WM_CHAR))
        return FALSE;

    if (dlgCode & DLGC_WANTMESSAGE)
    {
        *translate = *dispatch = TRUE;
        return TRUE;
    }

    switch(message)
    {
    case WM_KEYDOWN:
        switch(wParam)
        {
        case VK_TAB:
            if (!(dlgCode & DLGC_WANTTAB))
            {
                SendMessageA( hwndDlg, WM_NEXTDLGCTL,
                                (GetKeyState(VK_SHIFT) & 0x8000), 0 );
                return TRUE;
            }
            break;

        case VK_RIGHT:
        case VK_DOWN:
        case VK_LEFT:
        case VK_UP:
            if (!(dlgCode & DLGC_WANTARROWS))
            {
                BOOL fPrevious = (wParam == VK_LEFT || wParam == VK_UP);
                HWND hwndNext =
                    GetNextDlgGroupItem (hwndDlg, GetFocus(), fPrevious );
                SendMessageA( hwndDlg, WM_NEXTDLGCTL, hwndNext, 1 );
                return TRUE;
            }
            break;

        case VK_ESCAPE:
            SendMessageA( hwndDlg, WM_COMMAND, IDCANCEL,
                            (LPARAM)GetDlgItem( hwndDlg, IDCANCEL ) );
            return TRUE;

        case VK_RETURN:
            {
                DWORD dw = SendMessageA( hwndDlg, DM_GETDEFID, 0, 0 );
                if (HIWORD(dw) == DC_HASDEFID)
                {
                    SendMessageA( hwndDlg, WM_COMMAND,
                                    MAKEWPARAM( LOWORD(dw), BN_CLICKED ),
                                    (LPARAM)GetDlgItem(hwndDlg, LOWORD(dw)));
                }
                else
                {
                    SendMessageA( hwndDlg, WM_COMMAND, IDOK,
                                    (LPARAM)GetDlgItem( hwndDlg, IDOK ) );

                }
            }
            return TRUE;
        }
        *translate = TRUE;
        break; /* case WM_KEYDOWN */

    case WM_CHAR:
        if (dlgCode & DLGC_WANTCHARS) break;
        /* drop through */

    case WM_SYSCHAR:
        if (DIALOG_IsAccelerator( hwnd, hwndDlg, wParam ))
        {
            /* don't translate or dispatch */
            return TRUE;
        }
        break;
    }

    /* If we get here, the message has not been treated specially */
    /* and can be sent to its destination window. */
    *dispatch = TRUE;
    return TRUE;
}


/***********************************************************************
 *           IsDialogMessage32A   (USER32.342)
 */
BOOL WINAPI IsDialogMessageA( HWND hwndDlg, LPMSG msg )
{
    BOOL ret, translate, dispatch;
    INT dlgCode;

    if ((hwndDlg != msg->hwnd) && !IsChild( hwndDlg, msg->hwnd ))
        return FALSE;

    dlgCode = SendMessageA( msg->hwnd, WM_GETDLGCODE, 0, (LPARAM)msg);
    ret = DIALOG_IsDialogMessage( msg->hwnd, hwndDlg, msg->message,
                                  msg->wParam, msg->lParam,
                                  &translate, &dispatch, dlgCode );
    if (translate) TranslateMessage( msg );
    if (dispatch) DispatchMessageA( msg );
    return ret;
}


/***********************************************************************
 *           IsDialogMessage32W   (USER32.343)
 */
BOOL WINAPI IsDialogMessageW( HWND hwndDlg, LPMSG msg )
{
    BOOL ret, translate, dispatch;
    INT dlgCode;

    if ((hwndDlg != msg->hwnd) && !IsChild( hwndDlg, msg->hwnd ))
        return FALSE;

    dlgCode = SendMessageW( msg->hwnd, WM_GETDLGCODE, 0, (LPARAM)msg);
    ret = DIALOG_IsDialogMessage( msg->hwnd, hwndDlg, msg->message,
                                  msg->wParam, msg->lParam,
                                  &translate, &dispatch, dlgCode );
    if (translate) TranslateMessage( msg );
    if (dispatch) DispatchMessageW( msg );
    return ret;
}


/****************************************************************
 *         GetDlgCtrlID32   (USER32.234)
 */
INT WINAPI GetDlgCtrlID( HWND hwnd )
{
    INT retvalue;
    WND *wndPtr = WIN_FindWndPtr(hwnd);
    if (!wndPtr) return 0;
    retvalue = wndPtr->wIDmenu;
    WIN_ReleaseWndPtr(wndPtr);
    return retvalue;
}


/***********************************************************************
 *           GetDlgItem32   (USER32.235)
 */
HWND WINAPI GetDlgItem( HWND hwndDlg, INT id )
{
    WND *pWnd;

    if (!(pWnd = WIN_FindWndPtr( hwndDlg ))) return 0;
    for (WIN_UpdateWndPtr(&pWnd,pWnd->child); pWnd;WIN_UpdateWndPtr(&pWnd,pWnd->next))
        if (pWnd->wIDmenu == (UINT16)id)
        {
            HWND retvalue = pWnd->hwndSelf;
            WIN_ReleaseWndPtr(pWnd);
            return retvalue;
        }
    return 0;
}


/*******************************************************************
 *           SendDlgItemMessage32A   (USER32.452)
 */
LRESULT WINAPI SendDlgItemMessageA( HWND hwnd, INT id, UINT msg,
                                      WPARAM wParam, LPARAM lParam )
{
    HWND hwndCtrl = GetDlgItem( hwnd, id );
    if (hwndCtrl) return SendMessageA( hwndCtrl, msg, wParam, lParam );
    else return 0;
}


/*******************************************************************
 *           SendDlgItemMessage32W   (USER32.453)
 */
LRESULT WINAPI SendDlgItemMessageW( HWND hwnd, INT id, UINT msg,
                                      WPARAM wParam, LPARAM lParam )
{
    HWND hwndCtrl = GetDlgItem( hwnd, id );
    if (hwndCtrl) return SendMessageW( hwndCtrl, msg, wParam, lParam );
    else return 0;
}


/*******************************************************************
 *           SetDlgItemText32A   (USER32.478)
 */
BOOL WINAPI SetDlgItemTextA( HWND hwnd, INT id, LPCSTR lpString )
{
    return SendDlgItemMessageA( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpString );
}


/*******************************************************************
 *           SetDlgItemText32W   (USER32.479)
 */
BOOL WINAPI SetDlgItemTextW( HWND hwnd, INT id, LPCWSTR lpString )
{
    return SendDlgItemMessageW( hwnd, id, WM_SETTEXT, 0, (LPARAM)lpString );
}


/***********************************************************************
 *           GetDlgItemText32A   (USER32.237)
 */
UINT WINAPI GetDlgItemTextA( HWND hwnd, INT id, LPSTR str, UINT len )
{
    return (UINT)SendDlgItemMessageA( hwnd, id, WM_GETTEXT,
                                         len, (LPARAM)str );
}


/***********************************************************************
 *           GetDlgItemText32W   (USER32.238)
 */
UINT WINAPI GetDlgItemTextW( HWND hwnd, INT id, LPWSTR str, UINT len )
{
    return (UINT)SendDlgItemMessageW( hwnd, id, WM_GETTEXT,
                                         len, (LPARAM)str );
}


/*******************************************************************
 *           SetDlgItemInt32   (USER32.477)
 */
BOOL WINAPI SetDlgItemInt( HWND hwnd, INT id, UINT value,
                             BOOL fSigned )
{
    char str[20];

    if (fSigned) sprintf( str, "%d", (INT)value );
    else sprintf( str, "%u", value );
    SendDlgItemMessageA( hwnd, id, WM_SETTEXT, 0, (LPARAM)str );
    return TRUE;
}


/***********************************************************************
 *           GetDlgItemInt32   (USER32.236)
 */
UINT WINAPI GetDlgItemInt( HWND hwnd, INT id, BOOL *translated,
                               BOOL fSigned )
{
    char str[30];
    char * endptr;
    long result = 0;

    if (translated) *translated = FALSE;
    if (!SendDlgItemMessageA(hwnd, id, WM_GETTEXT, sizeof(str), (LPARAM)str))
        return 0;
    if (fSigned)
    {
        result = strtol( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if (((result == LONG_MIN) || (result == LONG_MAX)) && (errno==ERANGE))
            return 0;
    }
    else
    {
        result = strtoul( str, &endptr, 10 );
        if (!endptr || (endptr == str))  /* Conversion was unsuccessful */
            return 0;
        if ((result == ULONG_MAX) && (errno == ERANGE)) return 0;
    }
    if (translated) *translated = TRUE;
    return (UINT)result;
}


/***********************************************************************
 *           CheckDlgButton32   (USER32.45)
 */
BOOL WINAPI CheckDlgButton( HWND hwnd, INT id, UINT check )
{
    SendDlgItemMessageA( hwnd, id, BM_SETCHECK, check, 0 );
    return TRUE;
}


/***********************************************************************
 *           IsDlgButtonChecked32   (USER32.344)
 */
UINT WINAPI IsDlgButtonChecked( HWND hwnd, UINT id )
{
    return (UINT)SendDlgItemMessageA( hwnd, id, BM_GETCHECK, 0, 0 );
}


/***********************************************************************
 *           CheckRB
 *
 * Callback function used to check/uncheck radio buttons that fall
 * within a specific range of IDs.
 */
static BOOL CALLBACK CheckRB(HWND hwndChild, LPARAM lParam)
{
    LONG lChildID = GetWindowLongA(hwndChild, GWL_ID);
    RADIOGROUP *lpRadioGroup = (RADIOGROUP *) lParam;

    if ((lChildID >= lpRadioGroup->firstID) &&
        (lChildID <= lpRadioGroup->lastID))
    {
        if (lChildID == lpRadioGroup->checkID)
        {
            SendMessageA(hwndChild, BM_SETCHECK, BST_CHECKED, 0);
        }
        else
        {
            SendMessageA(hwndChild, BM_SETCHECK, BST_UNCHECKED, 0);
        }
    }

    return TRUE;
}


/***********************************************************************
 *           CheckRadioButton32   (USER32.48)
 */
BOOL WINAPI CheckRadioButton( HWND hwndDlg, UINT firstID,
                              UINT lastID, UINT checkID )
{
    RADIOGROUP radioGroup;

    /* perform bounds checking for a radio button group */
    radioGroup.firstID = min(min(firstID, lastID), checkID);
    radioGroup.lastID = max(max(firstID, lastID), checkID);
    radioGroup.checkID = checkID;

    return EnumChildWindows(hwndDlg, (WNDENUMPROC)CheckRB,
                            (LPARAM)&radioGroup);
}


/***********************************************************************
 *           GetDialogBaseUnits   (USER.243) (USER32.233)
 */
DWORD WINAPI GetDialogBaseUnits(void)
{
    return MAKELONG( xBaseUnit, yBaseUnit );
}


/***********************************************************************
 *           MapDialogRect32   (USER32.382)
 */
BOOL WINAPI MapDialogRect( HWND hwnd, LPRECT rect )
{
    DIALOGINFO * dlgInfo;
    WND * wndPtr = WIN_FindWndPtr( hwnd );
    if (!wndPtr) return FALSE;
    dlgInfo = (DIALOGINFO *)wndPtr->wExtra;
    rect->left   = (rect->left * dlgInfo->xBaseUnit) / 4;
    rect->right  = (rect->right * dlgInfo->xBaseUnit) / 4;
    rect->top    = (rect->top * dlgInfo->yBaseUnit) / 8;
    rect->bottom = (rect->bottom * dlgInfo->yBaseUnit) / 8;
    WIN_ReleaseWndPtr(wndPtr);
    return TRUE;
}


/***********************************************************************
 *           GetNextDlgGroupItem32   (USER32.275)
 */
HWND WINAPI GetNextDlgGroupItem( HWND hwndDlg, HWND hwndCtrl,
                                     BOOL fPrevious )
{
    WND *pWnd = NULL,
        *pWndLast = NULL,
        *pWndCtrl = NULL,
        *pWndDlg = NULL;
    HWND retvalue;

    if (!(pWndDlg = WIN_FindWndPtr( hwndDlg ))) return 0;
    if (hwndCtrl)
    {
        if (!(pWndCtrl = WIN_FindWndPtr( hwndCtrl )))
    {
            retvalue = 0;
            goto END;
        }
        /* Make sure hwndCtrl is a top-level child */
        while ((pWndCtrl->dwStyle & WS_CHILD) && (pWndCtrl->parent != pWndDlg))
            WIN_UpdateWndPtr(&pWndCtrl,pWndCtrl->parent);
        if (pWndCtrl->parent != pWndDlg)
        {
            retvalue = 0;
            goto END;
        }
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        if (!(pWndCtrl = WIN_LockWndPtr(pWndDlg->child)))
        {
            retvalue = 0;
            goto END;
        }
        if (fPrevious)
            while (pWndCtrl->next) WIN_UpdateWndPtr(&pWndCtrl,pWndCtrl->next);
    }

    pWndLast = WIN_LockWndPtr(pWndCtrl);
    pWnd = WIN_LockWndPtr(pWndCtrl->next);

    while (1)
    {
        if (!pWnd || (pWnd->dwStyle & WS_GROUP))
        {
            /* Wrap-around to the beginning of the group */
            WND *pWndTemp;

            WIN_UpdateWndPtr( &pWnd, pWndDlg->child );
            for ( pWndTemp = WIN_LockWndPtr( pWnd );
                  pWndTemp;
                  WIN_UpdateWndPtr( &pWndTemp, pWndTemp->next) )
            {
                if (pWndTemp->dwStyle & WS_GROUP) WIN_UpdateWndPtr( &pWnd, pWndTemp );
                if (pWndTemp == pWndCtrl) break;
            }
            WIN_ReleaseWndPtr( pWndTemp );
        }
        if (pWnd == pWndCtrl) break;
   if ((pWnd->dwStyle & WS_VISIBLE) && !(pWnd->dwStyle & WS_DISABLED))
   {
            WIN_UpdateWndPtr(&pWndLast,pWnd);
       if (!fPrevious) break;
   }
        WIN_UpdateWndPtr(&pWnd,pWnd->next);
    }
    retvalue = pWndLast->hwndSelf;

    WIN_ReleaseWndPtr(pWndLast);
    WIN_ReleaseWndPtr(pWnd);
END:
    WIN_ReleaseWndPtr(pWndCtrl);
    WIN_ReleaseWndPtr(pWndDlg);

    return retvalue;
}


/***********************************************************************
 *           GetNextDlgTabItem32   (USER32.276)
 */
HWND WINAPI GetNextDlgTabItem( HWND hwndDlg, HWND hwndCtrl,
                                   BOOL fPrevious )
{
    WND *pWnd = NULL,
        *pWndLast = NULL,
        *pWndCtrl = NULL,
        *pWndDlg = NULL;
    HWND retvalue;

    if (!(pWndDlg = WIN_FindWndPtr( hwndDlg ))) return 0;
    if (hwndCtrl)
    {
        if (!(pWndCtrl = WIN_FindWndPtr( hwndCtrl )))
    {
            retvalue = 0;
            goto END;
        }
        /* Make sure hwndCtrl is a top-level child */
        while ((pWndCtrl->dwStyle & WS_CHILD) && (pWndCtrl->parent != pWndDlg))
            WIN_UpdateWndPtr(&pWndCtrl,pWndCtrl->parent);
        if (pWndCtrl->parent != pWndDlg)
        {
            retvalue = 0;
            goto END;
    }
    }
    else
    {
        /* No ctrl specified -> start from the beginning */
        if (!(pWndCtrl = WIN_LockWndPtr(pWndDlg->child)))
        {
            retvalue = 0;
            goto END;
        }

        if (!fPrevious)
            while (pWndCtrl->next) WIN_UpdateWndPtr(&pWndCtrl,pWndCtrl->next);
    }

    pWndLast = WIN_LockWndPtr(pWndCtrl);
    pWnd = WIN_LockWndPtr(pWndCtrl->next);
    while (1)
    {
        if (!pWnd) pWnd = WIN_LockWndPtr(pWndDlg->child);
        if (pWnd == pWndCtrl) break;
   if ((pWnd->dwStyle & WS_TABSTOP) && (pWnd->dwStyle & WS_VISIBLE) &&
            !(pWnd->dwStyle & WS_DISABLED))
   {
            WIN_UpdateWndPtr(&pWndLast,pWnd);
       if (!fPrevious) break;
   }
        WIN_UpdateWndPtr(&pWnd,pWnd->next);
    }
    retvalue = pWndLast->hwndSelf;

    WIN_ReleaseWndPtr(pWndLast);
    WIN_ReleaseWndPtr(pWnd);
END:
    WIN_ReleaseWndPtr(pWndCtrl);
    WIN_ReleaseWndPtr(pWndDlg);

    return retvalue;

}


/**********************************************************************
 *           DIALOG_DlgDirSelect
 *
 * Helper function for DlgDirSelect*
 */
static BOOL DIALOG_DlgDirSelect( HWND hwnd, LPSTR str, INT len,
                                   INT id, BOOL unicode,
                                   BOOL combo )
{
    char *buffer, *ptr;
    INT item, size;
    BOOL ret;
    HWND listbox = GetDlgItem( hwnd, id );

    dprintf(("USER32:%04x '%s' %d\n", hwnd, str, id ));
    if (!listbox) return FALSE;

    item = SendMessageA(listbox, combo ? CB_GETCURSEL
                                             : LB_GETCURSEL, 0, 0 );
    if (item == LB_ERR) return FALSE;
    size = SendMessageA(listbox, combo ? CB_GETLBTEXTLEN
                                             : LB_GETTEXTLEN, 0, 0 );
    if (size == LB_ERR) return FALSE;

    if (!(buffer = (char*)HEAP_malloc( size+1 ))) return FALSE;

    SendMessageA( listbox, combo ? CB_GETLBTEXT : LB_GETTEXT,
                  item, (LPARAM)buffer );

    if ((ret = (buffer[0] == '[')))  /* drive or directory */
    {
        if (buffer[1] == '-')  /* drive */
        {
            buffer[3] = ':';
            buffer[4] = 0;
            ptr = buffer + 2;
        }
        else
        {
            buffer[strlen(buffer)-1] = '\\';
            ptr = buffer + 1;
        }
    }
    else ptr = buffer;

    if (unicode) lstrcpynAtoW( (LPWSTR)str, ptr, len );
    else lstrcpynA( str, ptr, len );
    HEAP_free( buffer );
    dprintf(("USER32:Returning %d '%s'\n", ret, str ));
    return ret;
}


/**********************************************************************
 *     DIALOG_DlgDirList
 *
 * Helper function for DlgDirList*
 */
static INT DIALOG_DlgDirList( HWND hDlg, LPSTR spec, INT idLBox,
                                INT idStatic, UINT attrib, BOOL combo )
{
    int drive;
    HWND hwnd;
    LPSTR orig_spec = spec;

#define SENDMSG(msg,wparam,lparam) \
    ((attrib & DDL_POSTMSGS) ? PostMessageA( hwnd, msg, wparam, lparam ) \
                             : SendMessageA( hwnd, msg, wparam, lparam ))

    dprintf(("USER32:%04x '%s' %d %d %04x\n",
                    hDlg, spec ? spec : "NULL", idLBox, idStatic, attrib ));

    if (spec && spec[0] && (spec[1] == ':'))
    {
        drive = toupper( spec[0] ) - 'A';
        spec += 2;
        if (!DRIVE_SetCurrentDrive( drive )) return FALSE;
    }
    else drive = DRIVE_GetCurrentDrive();

    /* If the path exists and is a directory, chdir to it */
    if (!spec || !spec[0] || DRIVE_Chdir( drive, spec )) spec = "*.*";
    else
    {
        char *p, *p2;
        p = spec;
        if ((p2 = strrchr( p, '\\' ))) p = p2;
        if ((p2 = strrchr( p, '/' ))) p = p2;
        if (p != spec)
        {
            char sep = *p;
            *p = 0;
            if (!DRIVE_Chdir( drive, spec ))
            {
                *p = sep;  /* Restore the original spec */
                return FALSE;
            }
            spec = p + 1;
        }
    }

    dprintf(("USER32:path=%c:\\%s mask=%s\n",
                    'A' + drive, DRIVE_GetDosCwd(drive), spec ));

    if (idLBox && ((hwnd = GetDlgItem( hDlg, idLBox )) != 0))
    {
        SENDMSG( combo ? CB_RESETCONTENT : LB_RESETCONTENT, 0, 0 );
        if (attrib & DDL_DIRECTORY)
        {
            if (!(attrib & DDL_EXCLUSIVE))
            {
                if (SENDMSG( combo ? CB_DIR : LB_DIR,
                             attrib & ~(DDL_DIRECTORY | DDL_DRIVES),
                             (LPARAM)spec ) == LB_ERR)
                    return FALSE;
            }
            if (SENDMSG( combo ? CB_DIR : LB_DIR,
                       (attrib & (DDL_DIRECTORY | DDL_DRIVES)) | DDL_EXCLUSIVE,
                         (LPARAM)"*.*" ) == LB_ERR)
                return FALSE;
        }
        else
        {
            if (SENDMSG( combo ? CB_DIR : LB_DIR, attrib,
                         (LPARAM)spec ) == LB_ERR)
                return FALSE;
        }
    }

    if (idStatic && ((hwnd = GetDlgItem( hDlg, idStatic )) != 0))
    {
        char temp[512];
        int drive = DRIVE_GetCurrentDrive();
        strcpy( temp, "A:\\" );
        temp[0] += drive;
        lstrcpynA( temp + 3, DRIVE_GetDosCwd(drive), sizeof(temp)-3 );
        CharLowerA( temp );
        /* Can't use PostMessage() here, because the string is on the stack */
        SetDlgItemTextA( hDlg, idStatic, temp );
    }

    if (orig_spec && (spec != orig_spec))
    {
        /* Update the original file spec */
        char *p = spec;
        while ((*orig_spec++ = *p++));
    }

    return TRUE;
#undef SENDMSG
}


/**********************************************************************
 *     DIALOG_DlgDirListW
 *
 * Helper function for DlgDirList*32W
 */
static INT DIALOG_DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                                 INT idStatic, UINT attrib, BOOL combo )
{
    if (spec)
    {
        LPSTR specA = HEAP_strdupWtoA( GetProcessHeap(), 0, spec );
        INT ret = DIALOG_DlgDirList( hDlg, specA, idLBox, idStatic,
                                       attrib, combo );
        lstrcpyAtoW( spec, specA );
        HeapFree( GetProcessHeap(), 0, specA );
        return ret;
    }
    return DIALOG_DlgDirList( hDlg, NULL, idLBox, idStatic, attrib, combo );
}



/**********************************************************************
 *           DlgDirSelectEx32A    (USER32.149)
 */
BOOL WINAPI DlgDirSelectExA( HWND hwnd, LPSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id, FALSE, FALSE );
}


/**********************************************************************
 *           DlgDirSelectEx32W    (USER32.150)
 */
BOOL WINAPI DlgDirSelectExW( HWND hwnd, LPWSTR str, INT len, INT id )
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, FALSE );
}


/**********************************************************************
 *           DlgDirSelectComboBoxEx32A    (USER32.147)
 */
BOOL WINAPI DlgDirSelectComboBoxExA( HWND hwnd, LPSTR str, INT len,
                                         INT id )
{
    return DIALOG_DlgDirSelect( hwnd, str, len, id,  FALSE, TRUE );
}


/**********************************************************************
 *           DlgDirSelectComboBoxEx32W    (USER32.148)
 */
BOOL WINAPI DlgDirSelectComboBoxExW( HWND hwnd, LPWSTR str, INT len,
                                         INT id)
{
    return DIALOG_DlgDirSelect( hwnd, (LPSTR)str, len, id, TRUE, TRUE );
}


/**********************************************************************
 *     DlgDirList32A    (USER32.143)
 */
INT WINAPI DlgDirListA( HWND hDlg, LPSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *     DlgDirList32W    (USER32.146)
 */
INT WINAPI DlgDirListW( HWND hDlg, LPWSTR spec, INT idLBox,
                            INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idLBox, idStatic, attrib, FALSE );
}


/**********************************************************************
 *     DlgDirListComboBox32A    (USER32.144)
 */
INT WINAPI DlgDirListComboBoxA( HWND hDlg, LPSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirList( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}


/**********************************************************************
 *     DlgDirListComboBox32W    (USER32.145)
 */
INT WINAPI DlgDirListComboBoxW( HWND hDlg, LPWSTR spec, INT idCBox,
                                    INT idStatic, UINT attrib )
{
    return DIALOG_DlgDirListW( hDlg, spec, idCBox, idStatic, attrib, TRUE );
}
