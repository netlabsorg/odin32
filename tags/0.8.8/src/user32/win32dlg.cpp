/* $Id: win32dlg.cpp,v 1.83 2004-03-18 15:38:42 sandervl Exp $ */
/*
 * Win32 Dialog Code for OS/2
 *
 * Copyright 1999-2001 Sander van Leeuwen (sandervl@xs4all.nl) (Wine port & OS/2 adaption)
 *
 * Based on Wine code (990815; windows\dialog.c)
 *
 * Copyright 1993, 1994, 1996 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <windowsx.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include "win32dlg.h"
#include <win/winproc.h>
#include "oslibmsg.h"
#include "oslibwin.h"
#include "win32wdesktop.h"
#include "controls.h"
#include "syscolor.h"
#include "hook.h"
#include <math.h>
#include <unicode.h>
#include "user32api.h"

#define DBG_LOCALLOG    DBG_win32dlg
#include "dbglocal.h"

#define DEFAULT_DLGFONT "9.WarpSans"

#define GET_SHORT(ptr)     (*(SHORT *)(ptr))


//******************************************************************************
//******************************************************************************
Win32Dialog::Win32Dialog(HINSTANCE hInst, LPCSTR dlgTemplate, HWND owner,
                         DLGPROC dlgProc, LPARAM param, BOOL isUnicode)
                    : Win32BaseWindow()
{
  RECT rect;
  WORD style;
  ATOM classAtom;

    this->isUnicode = isUnicode;
    hUserFont        = 0;
    hMenu        = 0;
    hwndFocus    = 0;
    Win32DlgProc = 0;
    msgResult    = 0;
    userDlgData  = 0;
    idResult     = 0;
    dialogFlags  = 0;
    fDialogInit  = FALSE;
    memset(&dlgInfo, 0, sizeof(dlgInfo));

    dprintf(("********* CREATE DIALOG ************"));
    if(fInitialized == FALSE) {
        if(DIALOG_Init() == FALSE) {
            dprintf(("DIALOG_Init FAILED!"));
            DebugInt3();
            SetLastError(ERROR_GEN_FAILURE);
            return;
        }
        fInitialized = TRUE;
    }
    xUnit = xBaseUnit;
    yUnit = yBaseUnit;

    /* Parse dialog template */
    dlgTemplate = parseTemplate(dlgTemplate, &dlgInfo);

    /* Load menu */
    if (dlgInfo.menuName)
    {
        hMenu = LoadMenuW( hInst, (LPCWSTR)dlgInfo.menuName );
    }

    dprintf(("Dialog style %x", LOWORD(dlgInfo.style)));

    /* Create custom font if needed */
    if (dlgInfo.style & DS_SETFONT)
    {
        /* The font height must be negative as it is a point size */
        /* and must be converted to pixels first */
        /* (see CreateFont() documentation in the Windows SDK).   */
        int pixels;
        if (((short)dlgInfo.pointSize) < 0)
            pixels = -((short)dlgInfo.pointSize);
        else
        {
            HDC hdc = GetDC(0);
            pixels = dlgInfo.pointSize * GetDeviceCaps(hdc, LOGPIXELSY)/72;
            ReleaseDC(0, hdc);
        }

        hUserFont = CreateFontW(-pixels, 0, 0, 0,
                            dlgInfo.weight, dlgInfo.italic, FALSE,
                            FALSE, DEFAULT_CHARSET, 0, 0, PROOF_QUALITY,
                            FF_DONTCARE, (LPCWSTR)dlgInfo.faceName );
        if (hUserFont)
        {
            SIZE charSize;
            getCharSize(hUserFont,&charSize);
            xUnit = charSize.cx;
            yUnit = charSize.cy;
        }
    }

    //Set help id
    setWindowContextHelpId(dlgInfo.helpId);

    /* Create dialog main window */
    rect.left = rect.top = 0;
    rect.right = dlgInfo.cx * xUnit / 4;
    rect.bottom = dlgInfo.cy * yUnit / 8;
    if (dlgInfo.style & DS_MODALFRAME)
        dlgInfo.exStyle |= WS_EX_DLGMODALFRAME;

    AdjustWindowRectEx( &rect, dlgInfo.style, hMenu ? TRUE : FALSE , dlgInfo.exStyle );
    rect.right -= rect.left;
    rect.bottom -= rect.top;

    if ((INT16)dlgInfo.x == CW_USEDEFAULT16)
    {
        rect.left = rect.top = CW_USEDEFAULT;
    }
    else
    {
        if (dlgInfo.style & DS_CENTER)
        {
            rect.left = (GetSystemMetrics(SM_CXSCREEN) - rect.right) / 2;
            rect.top = (GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / 2;
        }
        else
        {
            rect.left += dlgInfo.x * xUnit / 4;
            rect.top += dlgInfo.y * yUnit / 8;
        }
        if ( !(dlgInfo.style & WS_CHILD) )
        {
            INT dX, dY;

            if( !(dlgInfo.style & DS_ABSALIGN) && owner)
                ClientToScreen(owner, (POINT *)&rect );

            /* try to fit it into the desktop */

            if( (dX = rect.left + rect.right + GetSystemMetrics(SM_CXDLGFRAME)
                 - GetSystemMetrics(SM_CXSCREEN)) > 0 ) rect.left -= dX;
            if( (dY = rect.top + rect.bottom + GetSystemMetrics(SM_CYDLGFRAME)
                 - GetSystemMetrics(SM_CYSCREEN)) > 0 ) rect.top -= dY;
            if( rect.left < 0 ) rect.left = 0;
            if( rect.top < 0 ) rect.top = 0;
        }
    }

    /* Create the dialog window */

    /* Find the class atom */
    if (!HIWORD(dlgInfo.className))
    {
        classAtom = (ATOM)LOWORD(dlgInfo.className);
    }
    else
    if (!(classAtom = GlobalFindAtomW((LPWSTR)dlgInfo.className)))
    {
        SetLastError(ERROR_INVALID_PARAMETER);
        return;
    }
    CREATESTRUCTA cs;
    cs.lpCreateParams = NULL;
    cs.hInstance      = hInst;
    cs.hMenu          = hMenu;
    cs.hwndParent     = owner;
    cs.x              = rect.left;
    cs.y              = rect.top;
    cs.cx             = rect.right;
    cs.cy             = rect.bottom;
    cs.style          = dlgInfo.style & ~WS_VISIBLE;

    if(!isUnicode) {
        if(dlgInfo.caption) {
                cs.lpszName  = UnicodeToAsciiString((LPWSTR)dlgInfo.caption);
        }
        else    cs.lpszName  = 0;
        if(dlgInfo.className) {
                cs.lpszClass = UnicodeToAsciiString((LPWSTR)dlgInfo.className);
        }
        else    cs.lpszClass = 0;
    }
    else {
        cs.lpszName       = dlgInfo.caption;
        cs.lpszClass      = dlgInfo.className;
    }
    cs.dwExStyle      = dlgInfo.exStyle;
    if (dlgInfo.style & DS_CONTEXTHELP) cs.dwExStyle |= WS_EX_CONTEXTHELP;

    //Mask away WS_CAPTION style for dialogs with DS_CONTROL style
    //(necessary for OFN_ENABLETEMPLATE file open dialogs)
    //(verified this behaviour in NT4, SP6)
    if (dlgInfo.style & DS_CONTROL)     cs.style &= ~(WS_CAPTION);

    fIsDialog = TRUE;
    WINPROC_SetProc((HWINDOWPROC *)&Win32DlgProc, (WNDPROC)dlgProc, (isUnicode) ? WIN_PROC_32W : WIN_PROC_32A, WIN_PROC_WINDOW);

    this->tmpParam       = param;
    this->tmpDlgTemplate = (LPSTR)dlgTemplate;

    if (CreateWindowExA(&cs, classAtom) == FALSE)
    {
        if (hUserFont) DeleteObject( hUserFont );
        if (hMenu) DestroyMenu( hMenu );
        SetLastError(ERROR_OUTOFMEMORY); //TODO: Wrong error
        return;
    }
    SetLastError(0);
    return;
}
//******************************************************************************
//******************************************************************************
Win32Dialog::~Win32Dialog()
{
    if (hUserFont) DeleteObject( hUserFont );
    if (hMenu) DestroyMenu( hMenu );

    WINPROC_FreeProc((HWINDOWPROC)Win32DlgProc, WIN_PROC_WINDOW);
}
//******************************************************************************
//******************************************************************************
ULONG Win32Dialog::MsgCreate(HWND hwndOS2)
{
 CREATESTRUCTA *cs = tmpcs;  //pointer to CREATESTRUCT used in CreateWindowExA method
 LPARAM         param = tmpParam;
 LPSTR          dlgTemplate = tmpDlgTemplate;

    if(Win32BaseWindow::MsgCreate(hwndOS2) == FALSE) {
        dprintf(("********* DIALOG CREATION FAILED! (main dialog window) ************"));
        return FALSE;
    }

    if(!isUnicode) {
        if(cs->lpszName) FreeAsciiString((LPSTR)cs->lpszName);
        if(HIWORD(cs->lpszClass)) {
                FreeAsciiString((LPSTR)cs->lpszClass);
        }
    }

    if (hUserFont)
        SendMessageA(getWindowHandle(), WM_SETFONT, (WPARAM)hUserFont, 0 );

    /* Create controls */
    if (createControls(dlgTemplate, hInstance))
    {
        dprintf(("********* DIALOG CONTROLS CREATED ************"));
        /* Send initialisation messages and set focus */
        hwndFocus = GetNextDlgTabItem( getWindowHandle(), 0, FALSE );
        dprintf(("dlg ctor: GetNextDlgTabItem returned %x, capture hwnd = %x", hwndFocus, GetCapture()));

        fDialogInit = TRUE; //WM_NCCALCSIZE can now be sent to dialog procedure

        HWND hwndPreInitFocus = GetFocus();
        if(SendMessageA(getWindowHandle(), WM_INITDIALOG, (WPARAM)hwndFocus, param))
        {
            //SvL: Experiments in NT4 show that dialogs that are children don't
            //     receive focus. Not sure if this is always true. (couldn't
            //     find any remarks about this in the SDK docs)
            if(!(getStyle() & WS_CHILD))
            {
                /* check where the focus is again,
                 * some controls status might have changed in WM_INITDIALOG */
                hwndFocus = GetNextDlgTabItem( getWindowHandle(), 0, FALSE );
                if(hwndFocus) {
                    SetFocus(hwndFocus);
                }
            }
        }
        else
        {
            //SvL: Experiments in NT4 show that dialogs that are children don't
            //     receive focus. Not sure if this is always true. (couldn't
            //     find any remarks about this in the SDK docs)
            if(!(getStyle() & WS_CHILD))
            {
                /* If the dlgproc has returned FALSE (indicating handling of keyboard focus)
                   but the focus has not changed, set the focus where we expect it. */
                if ( (getStyle() & WS_VISIBLE) && ( GetFocus() == hwndPreInitFocus ) )
                    SetFocus( hwndFocus );
            }
        }

        if (dlgInfo.style & WS_VISIBLE && !(getStyle() & WS_VISIBLE))
        {
            ShowWindow( SW_SHOWNORMAL );    /* SW_SHOW doesn't always work */
////Note: this was removed in Wine
////            UpdateWindow( getWindowHandle() );
        }
        SetLastError(ERROR_SUCCESS);
        dprintf(("********* DIALOG CREATED ************"));
        return TRUE;
    }
    dprintf(("********* DIALOG CREATION FAILED! ************"));
    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dialog::MapDialogRect(LPRECT rect)
{
    rect->left   = (rect->left * xUnit) / 4;
    rect->right  = (rect->right * xUnit) / 4;
    rect->top    = (rect->top * yUnit) / 8;
    rect->bottom = (rect->bottom * yUnit) / 8;
    return TRUE;
}
/***********************************************************************
 *           DIALOG_DoDialogBox
 */
INT Win32Dialog::doDialogBox()
{
  Win32BaseWindow *topOwner;
  MSG msg;
  INT retval;

    dprintf(("doDialogBox %x", getWindowHandle()));
    /* Owner must be a top-level window */
    if(getOwner() == NULL) {
         windowDesktop->addRef();
         topOwner = windowDesktop;
    }
    else topOwner = GetWindowFromHandle(getOwner()->GetTopParent());

    if(topOwner == NULL) {
        dprintf(("Dialog box has no top owner!!!"));
        return -1;
    }

    if (!dialogFlags & DF_END) /* was EndDialog called in WM_INITDIALOG ? */
    {
        HWND hwndOldDialog;
        BOOL bOldOwner;

        fIsModalDialog = TRUE;
        topOwner->EnableWindow(FALSE);

        bOldOwner = topOwner->IsModalDialogOwner();
        topOwner->setModalDialogOwner(TRUE);
        hwndOldDialog = topOwner->getOS2HwndModalDialog();
        topOwner->setOS2HwndModalDialog(OS2HwndFrame);
        ShowWindow(SW_SHOW);

        //CB: 100% CPU usage, need a better solution with OSLibWinGetMsg
        //    is WM_ENTERIDLE used and leaving away breaks an application?
        //    this style was useful for Win3.1 but today there are threads
        // solution: send only few WM_ENTERIDLE messages
        while (TRUE)
        {
          if (!PeekMessageA(&msg,0,0,0,PM_NOREMOVE))
          {
                if(!(getStyle() & DS_NOIDLEMSG))
                    SendMessageA(topOwner->getWindowHandle(), WM_ENTERIDLE,MSGF_DIALOGBOX,getWindowHandle());
                GetMessageA(&msg,0,0,0);
          }
          else  PeekMessageA(&msg,0,0,0,PM_REMOVE);

          /* Call message filters */
          if (HOOK_IsHooked( WH_SYSMSGFILTER ) || HOOK_IsHooked( WH_MSGFILTER ))
          {
            LPMSG pmsg = (LPMSG)HeapAlloc( GetProcessHeap(), 0, sizeof(MSG) );
            if (pmsg)
            {
                BOOL ret;
                *pmsg = msg;
                ret = (HOOK_CallHooksA( WH_SYSMSGFILTER, MSGF_DIALOGBOX, 0,
                                          (LPARAM) pmsg ) ||
                       HOOK_CallHooksA( WH_MSGFILTER, MSGF_DIALOGBOX, 0,
                                          (LPARAM) pmsg ));

                HeapFree( GetProcessHeap(), 0, pmsg );
                if (ret)
                {
                    /* Message filtered -> remove it from the queue */
                    /* if it's still there. */
                    continue;
                }
            }
          }

          if(msg.message == WM_QUIT)
          {
                dprintf(("Win32Dialog::doDialogBox: received  WM_QUIT"));
                break;
          }
          if (!IsDialogMessageA( getWindowHandle(), &msg))
          {
                TranslateMessage( &msg );
                DispatchMessageA( &msg );
          }
          if (dialogFlags & DF_END)
                break;
        }
        topOwner->setModalDialogOwner(bOldOwner);
        topOwner->setOS2HwndModalDialog(hwndOldDialog);
        if (!bOldOwner) topOwner->EnableWindow(TRUE);
    }
    RELEASE_WNDOBJ(topOwner);
    retval = idResult;
    DestroyWindow();
    return retval;
}
/***********************************************************************
 *           DIALOG_Init
 *
 * Initialisation of the dialog manager.
 */
BOOL Win32Dialog::DIALOG_Init(void)
{
    HDC hdc;
    SIZE size;

    /* Calculate the dialog base units */
    if (!(hdc = CreateDCA( "DISPLAY", NULL, NULL, NULL ))) return FALSE;
    if (!getCharSizeFromDC( hdc, 0, &size )) return FALSE;
    DeleteDC( hdc );
    dprintf(("Dialog base unit size (%d,%d)", size.cx, size.cy));
    xBaseUnit = size.cx;
    yBaseUnit = size.cy;

    return TRUE;
}
/***********************************************************************
 *           DIALOG_GetCharSizeFromDC
 *
 *
 *  Calculates the *true* average size of English characters in the
 *  specified font as oppposed to the one returned by GetTextMetrics.
 */
BOOL Win32Dialog::getCharSizeFromDC( HDC hDC, HFONT hUserFont, SIZE * pSize )
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
        if (hUserFont) hFontPrev = SelectFont(hDC,hUserFont);
        if (GetTextMetricsA(hDC,&tm))
        {
            pSize->cx = tm.tmAveCharWidth;
            pSize->cy = tm.tmHeight;

            /* if variable width font */
            if (tm.tmPitchAndFamily & TMPF_FIXED_PITCH)
            {
                SIZE total;
                static const char szAvgChars[53] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

                /* Calculate a true average as opposed to the one returned
                 * by tmAveCharWidth. This works better when dealing with
                 * proportional spaced fonts and (more important) that's
                 * how Microsoft's dialog creation code calculates the size
                 * of the font
                 */
#ifdef __WIN32OS2__
                if (GetTextExtentPointA(hDC,szAvgChars,sizeof(szAvgChars)-1,&total))
                {
                   /* round up */
                    pSize->cx = ((2*total.cx/(sizeof(szAvgChars)-1)) + 1)/2;
                    Success = TRUE;
                }
#else
                if (GetTextExtentPointA(hDC,szAvgChars,sizeof(szAvgChars),&total))
                {
                   /* round up */
                    pSize->cx = ((2*total.cx/sizeof(szAvgChars)) + 1)/2;
                    Success = TRUE;
                }
#endif
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
BOOL Win32Dialog::getCharSize( HFONT hUserFont, SIZE * pSize )
{
    HDC  hDC = CreateCompatibleDC(0);
    BOOL Success = getCharSizeFromDC( hDC, hUserFont, pSize );
    DeleteDC(hDC);
    return Success;
}
/***********************************************************************
 *           DIALOG_ParseTemplate32
 *
 * Fill a DLG_TEMPLATE structure from the dialog template, and return
 * a pointer to the first control.
 */
LPCSTR Win32Dialog::parseTemplate( LPCSTR dlgtemplate, DLG_TEMPLATE * result )
{
    const WORD *p = (const WORD *)dlgtemplate;

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
    //x & y are signed words
    result->x       = GET_SHORT(p); p++;
    result->y       = GET_SHORT(p); p++;
    result->cx      = GET_WORD(p); p++;
    result->cy      = GET_WORD(p); p++;

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
        break;
    default:
        result->menuName = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
        break;
    }

    /* Get the class name */
    switch(GET_WORD(p))
    {
    case 0x0000:
        result->className = (LPCSTR)DIALOG_CLASS_NAMEW;
        p++;
        break;
    case 0xffff:
        result->className = (LPCSTR)(UINT)GET_WORD( p + 1 );
        p += 2;
        break;
    default:
        result->className = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
        break;
    }

    /* Get the window caption */

    result->caption = (LPCSTR)p;
    p += lstrlenW( (LPCWSTR)p ) + 1;

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
    }

    /* First control is on dword boundary */
    return (LPCSTR)((((int)p) + 3) & ~3);
}
/***********************************************************************
 *           DIALOG_GetControl32
 *
 * Return the class and text of the control pointed to by ptr,
 * fill the header structure and return a pointer to the next control.
 */
WORD *Win32Dialog::getControl(const WORD *p, DLG_CONTROL_INFO *info, BOOL dialogEx)
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
    //SvL: x & y are signed words!
    info->x       = GET_SHORT(p); p++;
    info->y       = GET_SHORT(p); p++;
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
            dprintf(("Unknown built-in class id %04x\n", id ));
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
        //Wine add 3 (bytes) here. But that causes problems with certain
        //InstallShield installers.
        p += 2;
    }
    else
    {
        info->windowName = (LPCSTR)p;
        p += lstrlenW( (LPCWSTR)p ) + 1;
    }

    if (GET_WORD(p))
    {
        info->data = (LPVOID)(p + 1);
        p += GET_WORD(p) / sizeof(WORD);
    }
    else info->data = NULL;
    p++;

    /* Next control is on dword boundary */
    return (WORD *)((((int)p) + 3) & ~3);
}


/***********************************************************************
 *           DIALOG_CreateControls
 *
 * Create the control windows for a dialog.
 */
BOOL Win32Dialog::createControls(LPCSTR dlgtemplate, HINSTANCE hInst)
{
    DLG_CONTROL_INFO info;
    HWND hwndCtrl, hwndDefButton = 0;
    INT items = dlgInfo.nbItems;

    while (items--)
    {
        dlgtemplate = (LPCSTR)getControl( (WORD *)dlgtemplate, &info, dlgInfo.dialogEx );

        /* Is this it? */
        if (info.style & WS_BORDER)
        {
            info.style &= ~WS_BORDER;
            info.exStyle |= WS_EX_CLIENTEDGE;
        }

        dprintf(("Create CONTROL %d", info.id));

        hwndCtrl = ::CreateWindowExW( info.exStyle | WS_EX_NOPARENTNOTIFY,
                                      (LPWSTR)info.className,
                                      (LPWSTR)info.windowName,
                                      info.style | WS_CHILD,
                                      MulDiv(info.x, xUnit, 4),
                                      MulDiv(info.y, yUnit, 8),
                                      MulDiv(info.cx, xUnit, 4),
                                      MulDiv(info.cy, yUnit, 8),
                                      getWindowHandle(), (HMENU)info.id,
                                      hInst, info.data );

        if (!hwndCtrl) return FALSE;

            /* Send initialisation messages to the control */
        if (hUserFont) ::SendMessageA( hwndCtrl, WM_SETFONT, (WPARAM)hUserFont, 0 );

        if (::SendMessageA(hwndCtrl, WM_GETDLGCODE, 0, 0) & DLGC_DEFPUSHBUTTON)
        {
            /* If there's already a default push-button, set it back */
            /* to normal and use this one instead. */
            if (hwndDefButton)
                ::SendMessageA( hwndDefButton, BM_SETSTYLE,
                                BS_PUSHBUTTON,FALSE );
            hwndDefButton = hwndCtrl;
            idResult = ::GetWindowWord( hwndCtrl, GWW_ID );
        }
        dprintf(("Create CONTROL %d DONE", info.id));
    }
    return TRUE;
}
/***********************************************************************
 *           DEFDLG_Proc
 *
 * Implementation of DefDlgProc(). Only handle messages that need special
 * handling for dialogs.
 */
LRESULT Win32Dialog::DefDlg_Proc(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_ERASEBKGND:
    {
      RECT rect;
      int rc;

        if (!windowClass) return 0;

        rc = GetClipBox( (HDC)wParam, &rect );
        if ((rc == SIMPLEREGION) || (rc == COMPLEXREGION))
        {
            HBRUSH hBrush = SendMessageA(getWindowHandle(), WM_CTLCOLORDLG, wParam, getWindowHandle());
            if(GetObjectType(hBrush) != OBJ_BRUSH) {
                DefWndControlColor(CTLCOLOR_DLG, (HDC)wParam);
            }
            FillRect( (HDC)wParam, &rect, hBrush);
        }

        return 1;
    }

    case WM_NCDESTROY:
          /* Free dialog heap (if created) */
#if 0
        if (dlgInfo->hDialogHeap)
        {
            GlobalUnlock16(dlgInfo->hDialogHeap);
            GlobalFree16(dlgInfo->hDialogHeap);
            dlgInfo->hDialogHeap = 0;
        }
#endif
          /* Delete font */
        if (hUserFont)
        {
            DeleteObject( hUserFont );
            hUserFont = 0;
        }

          /* Delete menu */
        if (hMenu)
        {
            DestroyMenu( hMenu );
            hMenu = 0;
        }

        /* Delete window procedure */
        Win32DlgProc = 0;
        dialogFlags |= DF_END;  /* just in case */

          /* Window clean-up */
        return DefWindowProcA(msg, wParam, lParam );

    case WM_SHOWWINDOW:
        if (!wParam) saveFocus();
        return DefWindowProcA(msg, wParam, lParam );

    case WM_ACTIVATE:
        if (wParam) {
                restoreFocus();
        }
        else    saveFocus();
        return 0;

    case WM_SETFOCUS:
        restoreFocus();
        return 0;

    case DM_SETDEFID:
        if (dialogFlags & DF_END)
            return 1;

        setDefButton(wParam ? GetDlgItem( getWindowHandle(), wParam ) : 0 );
        return 1;

    case DM_GETDEFID:
    {
        HWND hwndDefId;
        if (dialogFlags & DF_END) return 0;
        if (idResult)
            return MAKELONG( idResult, DC_HASDEFID );
        if ((hwndDefId = findDefButton()) != 0)
            return MAKELONG( GetDlgCtrlID( hwndDefId ), DC_HASDEFID);

        return 0;
    }

    case WM_NEXTDLGCTL:
    {
        HWND hwndDest = (HWND)wParam;
        if (!lParam)
            hwndDest = GetNextDlgTabItem(getWindowHandle(), GetFocus(), wParam);
        if (hwndDest) setFocus( hwndDest );
        setDefButton( hwndDest );
        return 0;
    }

    case WM_ENTERMENULOOP:
    case WM_LBUTTONDOWN:
    case WM_NCLBUTTONDOWN:
    {
        HWND hwndCurFocus = GetFocus();
        if (hwndCurFocus)
        {
            Win32BaseWindow *wndFocus = Win32BaseWindow::GetWindowFromHandle(hwndFocus);

            if(wndFocus)
            {
                /* always make combo box hide its listbox control */
                if( CONTROLS_IsControl( wndFocus, COMBOBOX_CONTROL ) )
                    SendMessageA(wndFocus->getWindowHandle(), CB_SHOWDROPDOWN, FALSE, 0 );
                else
                if( CONTROLS_IsControl( wndFocus, EDIT_CONTROL ) &&
                    CONTROLS_IsControl( wndFocus->getParent(), COMBOBOX_CONTROL ))
                    SendMessageA(wndFocus->getWindowHandle(), CB_SHOWDROPDOWN, FALSE, 0 );
                RELEASE_WNDOBJ(wndFocus);
            }
        }
        return DefWindowProcA( msg, wParam, lParam );
    }

    case WM_GETFONT:
        return hUserFont;

    case WM_CLOSE:
        PostMessageA(getWindowHandle(), WM_COMMAND, IDCANCEL, (LPARAM)GetDlgItem( getWindowHandle(), IDCANCEL ) );
        return 0;

    case WM_NOTIFYFORMAT:
        return DefWindowProcA(msg, wParam, lParam );
    }
    return 0;
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Dialog::DefDlgProcA(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = FALSE;

    msgResult = 0;

    //Dialogs never receive these messages
    if (Msg == WM_CREATE || Msg == WM_NCCREATE) {
        return (LRESULT)1;
    }
    //Never send a WM_NCCALCSIZE to a dialog before it has received it's WM_INITDIALOG message
    //(causes problems for sysinf32.exe)
    if(!fDialogInit && Msg == WM_NCCALCSIZE) {
        return DefWindowProcA(Msg, wParam, lParam );
    }

    if (Win32DlgProc) {      /* Call dialog procedure */
        result = CallWindowProcA((WNDPROC)Win32DlgProc, getWindowHandle(), Msg, wParam, lParam);
    }

    if (!result && IsWindow())
    {
        /* callback didn't process this message */
        switch(Msg)
        {
            case WM_ERASEBKGND:
            case WM_SHOWWINDOW:
            case WM_ACTIVATE:
            case WM_SETFOCUS:
            case DM_SETDEFID:
            case DM_GETDEFID:
            case WM_NEXTDLGCTL:
            case WM_GETFONT:
            case WM_CLOSE:
            case WM_NCDESTROY:
            case WM_ENTERMENULOOP:
            case WM_LBUTTONDOWN:
            case WM_NCLBUTTONDOWN:
                 return DefDlg_Proc(Msg, (WPARAM)wParam, lParam);

            case WM_INITDIALOG:
            case WM_VKEYTOITEM:
            case WM_COMPAREITEM:
            case WM_CHARTOITEM:
                 break;

            default:
                 return DefWindowProcA(Msg, wParam, lParam );
        }
    }
    return DefDlg_Epilog(Msg, result);
}
//******************************************************************************
//******************************************************************************
LRESULT Win32Dialog::DefDlgProcW(UINT Msg, WPARAM wParam, LPARAM lParam)
{
    BOOL result = FALSE;

    msgResult = 0;

    //Dialogs never receive these messages
    if (Msg == WM_CREATE || Msg == WM_NCCREATE) {
        return (LRESULT)1;
    }
    //Never send a WM_NCCALCSIZE to a dialog before it has received it's WM_INITDIALOG message
    //(causes problems for sysinf32.exe)
    if(!fDialogInit && Msg == WM_NCCALCSIZE) {
        return DefWindowProcW(Msg, wParam, lParam );
    }

    if (Win32DlgProc) {      /* Call dialog procedure */
        result = CallWindowProcW((WNDPROC)Win32DlgProc, getWindowHandle(), Msg, wParam, lParam);
    }

    if (!result && IsWindow())
    {
        /* callback didn't process this message */
        switch(Msg)
        {
            case WM_ERASEBKGND:
            case WM_SHOWWINDOW:
            case WM_ACTIVATE:
            case WM_SETFOCUS:
            case DM_SETDEFID:
            case DM_GETDEFID:
            case WM_NEXTDLGCTL:
            case WM_GETFONT:
            case WM_CLOSE:
            case WM_NCDESTROY:
            case WM_ENTERMENULOOP:
            case WM_LBUTTONDOWN:
            case WM_NCLBUTTONDOWN:
                 return DefDlg_Proc(Msg, (WPARAM)wParam, lParam);

            case WM_INITDIALOG:
            case WM_VKEYTOITEM:
            case WM_COMPAREITEM:
            case WM_CHARTOITEM:
                 break;

            default:
                 return DefWindowProcW(Msg, wParam, lParam );
        }
    }
    return DefDlg_Epilog(Msg, result);
}
/***********************************************************************
 *           DEFDLG_Epilog
 */
LRESULT Win32Dialog::DefDlg_Epilog(UINT msg, BOOL fResult)
{
    /* see SDK 3.1 */
    if ((msg >= WM_CTLCOLORMSGBOX && msg <= WM_CTLCOLORSTATIC) ||
         msg == WM_CTLCOLOR || msg == WM_COMPAREITEM ||
         msg == WM_VKEYTOITEM || msg == WM_CHARTOITEM ||
         msg == WM_QUERYDRAGICON || msg == WM_INITDIALOG)
        return fResult;

    return msgResult;
}
/***********************************************************************
 *           DEFDLG_SetFocus
 *
 * Set the focus to a control of the dialog, selecting the text if
 * the control is an edit dialog.
 */
void Win32Dialog::setFocus(HWND hwndCtrl )
{
    HWND hwndPrev = GetFocus();

#ifdef __WIN32OS2__
    if(hwndPrev == hwndCtrl) {
        //do nothing
        //Acrobat Reader 4.05 opening screen (when EULA has not been shown
        //local machine/software/adobe/acrobat reader/4.0/adobeviewer/EULA = 0)
        //All text selected in edit control because focus set when the control
        //already has focus.
        //Not entirely sure this is 100% correct, but at least now it behaves
        //like AR 4.05 in Windows NT 4.0 SP6
        return;
    }
#endif

    if (IsChild( hwndPrev ))
    {
        if (::SendMessageA( hwndPrev, WM_GETDLGCODE, 0, 0 ) & DLGC_HASSETSEL)
            ::SendMessageA( hwndPrev, EM_SETSEL, TRUE, MAKELONG( -1, 0 ) );
    }
    if (::SendMessageA(hwndCtrl, WM_GETDLGCODE, 0, 0 ) & DLGC_HASSETSEL)
        ::SendMessageA(hwndCtrl, EM_SETSEL, 0, -1 );
    SetFocus( hwndCtrl );
}


/***********************************************************************
 *           DEFDLG_SaveFocus
 */
BOOL Win32Dialog::saveFocus()
{
    HWND hwndCurrentFocus = GetFocus();

    if (!hwndCurrentFocus || !IsChild( hwndCurrentFocus )) return FALSE;

    hwndFocus = hwndCurrentFocus;
      /* Remove default button */
    return TRUE;
}


/***********************************************************************
 *           DEFDLG_RestoreFocus
 */
BOOL Win32Dialog::restoreFocus()
{
    if (!hwndFocus || IsWindowIconic()) return FALSE;

    if (!::IsWindow( hwndFocus )) return FALSE;

    /* Don't set the focus back to controls if EndDialog is already called.*/
    if (!(dialogFlags & DF_END))
       setFocus(hwndFocus);

    /* This used to set infoPtr->hwndFocus to NULL for no apparent reason,
       sometimes losing focus when receiving WM_SETFOCUS messages. */
    return TRUE;
}


/***********************************************************************
 *           DEFDLG_FindDefButton
 *
 * Find the current default push-button.
 */
HWND Win32Dialog::findDefButton()
{
    HWND hwndChild = GetWindow( GW_CHILD );
    while (hwndChild)
    {
        if (::SendMessageA( hwndChild, WM_GETDLGCODE, 0, 0 ) & DLGC_DEFPUSHBUTTON)
            break;
        hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT );
    }
    return hwndChild;
}


/***********************************************************************
 *           DEFDLG_SetDefButton
 *
 * Set the new default button to be hwndNew.
 */
BOOL Win32Dialog::setDefButton(HWND hwndNew )
{
    if (hwndNew &&
        !(::SendMessageA(hwndNew, WM_GETDLGCODE, 0, 0 ) & DLGC_UNDEFPUSHBUTTON))
        return FALSE;  /* Destination is not a push button */

    if (idResult)  /* There's already a default pushbutton */
    {
        HWND hwndOld = GetDlgItem( getWindowHandle(), idResult );
        if (::SendMessageA( hwndOld, WM_GETDLGCODE, 0, 0) & DLGC_DEFPUSHBUTTON)
            ::SendMessageA( hwndOld, BM_SETSTYLE, BS_PUSHBUTTON, TRUE );
    }
    if (hwndNew)
    {
        ::SendMessageA( hwndNew, BM_SETSTYLE, BS_DEFPUSHBUTTON, TRUE );
        idResult = GetDlgCtrlID( hwndNew );
    }
    else idResult = 0;
    return TRUE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dialog::endDialog(int retval)
{
    HWND hwnd = getWindowHandle();

    dialogFlags |= DF_END;
    idResult = retval;

//    BOOL wasEnabled = (dlgInfo.flags & DF_OWNERENABLED);

//    if (wasEnabled && (owner = GetWindow( hwnd, GW_OWNER )))
//        DIALOG_EnableOwner( owner );

    /* Windows sets the focus to the dialog itself in EndDialog */

#ifdef __WIN32OS2__
    if (::IsChild(hwnd, GetFocus()))
    {
       ::SetFocus( hwnd );
       //SvL: Enable the owner if the focus is on one of its children.
       //     If we don't, then PM will switch focus to another app since
       //     we hide the window below (hidden windows cannot have focus)
       HWND owner = ::GetWindow( hwnd, GW_OWNER);
       if(owner)
           ::EnableWindow(owner, TRUE);
    }
#else
    if (::IsChild(hwnd, GetFocus()))
       ::SetFocus( hwnd );
#endif

    /* Don't have to send a ShowWindow(SW_HIDE), just do
       SetWindowPos with SWP_HIDEWINDOW as done in Windows */

    ::SetWindowPos(hwnd, (HWND)0, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE
                 | SWP_NOZORDER | SWP_NOACTIVATE | SWP_HIDEWINDOW);

    /* unblock dialog loop */
    PostMessageA(hwnd, WM_NULL, 0, 0);
    return TRUE;
}
//******************************************************************************
//******************************************************************************
LONG Win32Dialog::SetWindowLong(int index, ULONG value, BOOL fUnicode)
{
    LONG oldval;

    dprintf2(("Win32Dialog::SetWindowLongA %x %d %x", getWindowHandle(), index, value));
    switch(index)
    {
    case DWL_DLGPROC:
    {
        //Note: Type of SetWindowLong determines new window proc type
        //      UNLESS the new window proc has already been registered
        //      (use the old type in that case)
        //      (VERIFIED in NT 4, SP6)
        WINDOWPROCTYPE type = WINPROC_GetProcType((HWINDOWPROC)value);
        if(type == WIN_PROC_INVALID) {
            type = (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A;
        }
        oldval = (LONG)WINPROC_GetProc((HWINDOWPROC)Win32DlgProc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
        WINPROC_SetProc((HWINDOWPROC *)&Win32DlgProc, (WNDPROC)value, type, WIN_PROC_WINDOW);
        return oldval;
    }
    case DWL_MSGRESULT:
        oldval = msgResult;
        msgResult = value;
        return oldval;
    case DWL_USER:
        oldval = userDlgData;
        userDlgData = value;
        return oldval;
    default:
        return Win32BaseWindow::SetWindowLong(index, value, fUnicode);
    }
}
//******************************************************************************
//******************************************************************************
ULONG Win32Dialog::GetWindowLong(int index, BOOL fUnicode)
{
    dprintf2(("Win32Dialog::GetWindowLongA %x %d", getWindowHandle(), index));
    switch(index)
    {
    case DWL_DLGPROC:
        return (ULONG)WINPROC_GetProc((HWINDOWPROC)Win32DlgProc, (fUnicode) ? WIN_PROC_32W : WIN_PROC_32A);
    case DWL_MSGRESULT:
        return msgResult;
    case DWL_USER:
        return userDlgData;
    default:
        return Win32BaseWindow::GetWindowLong(index, fUnicode);
    }
}
//******************************************************************************
//******************************************************************************
BOOL DIALOG_Register()
{
    WNDCLASSA wndClass;

    ZeroMemory(&wndClass,sizeof(WNDCLASSA));
    wndClass.style         = CS_GLOBALCLASS | CS_SAVEBITS;
    wndClass.lpfnWndProc   = (WNDPROC)DefDlgProcA;
    wndClass.cbClsExtra    = 0;
    wndClass.cbWndExtra    = 0;
    wndClass.hCursor       = LoadCursorA(0,IDC_ARROWA);
    wndClass.hbrBackground = GetSysColorBrush(COLOR_BTNFACE);
    wndClass.lpszClassName = DIALOG_CLASS_NAMEA;

    return RegisterClassA(&wndClass);
}
//******************************************************************************
//******************************************************************************
BOOL DIALOG_Unregister()
{
    if (GlobalFindAtomA(DIALOG_CLASS_NAMEA))
            return UnregisterClassA(DIALOG_CLASS_NAMEA,(HINSTANCE)NULL);
    else    return FALSE;
}
//******************************************************************************
//******************************************************************************
BOOL Win32Dialog::fInitialized = FALSE;
int  Win32Dialog::xBaseUnit    = 10;
int  Win32Dialog::yBaseUnit    = 20;
