/* $Id: winaccel.cpp,v 1.1 2001-05-11 08:36:29 sandervl Exp $ */
/*
 * Win32 accelerator key functions for OS/2
 *
 * Based on Wine: (windows\input.c; loader\resource.c) (20000130)
 * Copyright 1993 Bob Amstadt
 * Copyright 1996 Albrecht Kleine
 * Copyright 1997 David Faure
 * Copyright 1998 Morten Welinder
 * Copyright 1998 Ulrich Weigand
 * Copyright 1993 Robert J. Amstadt
 * Copyright 1995 Alexandre Julliard
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <misc.h>
#include <heapstring.h>
#include "win32wbase.h"
#include "win32wmdiclient.h"
#include <win\winnls.h>

#define DBG_LOCALLOG    DBG_winaccel
#include "dbglocal.h"

/**********************************************************************
 *           KBD_translate_accelerator
 *
 * FIXME: should send some WM_INITMENU or/and WM_INITMENUPOPUP  -messages
 */
static BOOL KBD_translate_accelerator(HWND hWnd,LPMSG msg,
                                      BYTE fVirt,WORD key,WORD cmd)
{
    BOOL sendmsg = FALSE;

    if(msg->wParam == key)
    {
        if (msg->message == WM_CHAR) {
            if ( !(fVirt & FALT) && !(fVirt & FVIRTKEY) )
            {
                dprintf(("TranslateAccelerator: found accel for WM_CHAR: ('%c')\n", msg->wParam&0xff));
                sendmsg=TRUE;
            }
        }
        else
        {
            if(fVirt & FVIRTKEY) {
                INT mask = 0;
                if(GetKeyState(VK_SHIFT) & 0x8000) mask |= FSHIFT;
                if(GetKeyState(VK_CONTROL) & 0x8000) mask |= FCONTROL;
                if(GetKeyState(VK_MENU) & 0x8000) mask |= FALT;

                if(mask == (fVirt & (FSHIFT | FCONTROL | FALT)))
                        sendmsg=TRUE;
                else    dprintf(("TranslateAccelerator: but incorrect SHIFT/CTRL/ALT-state %x != %x", mask, fVirt));
            }
            else
            {
                if (!(msg->lParam & 0x01000000))  /* no special_key */
                {
                    if ((fVirt & FALT) && (msg->lParam & 0x20000000))
                    {                                                   /* ^^ ALT pressed */
                        dprintf(("TranslateAccelerator: found accel for Alt-%c\n", msg->wParam&0xff));
                        sendmsg=TRUE;
                    }
                }
            }
        }

        if (sendmsg)      /* found an accelerator, but send a message... ? */
        {
            INT  iSysStat,iStat,mesg=0;
            HMENU hMenu;

            if (msg->message == WM_KEYUP || msg->message == WM_SYSKEYUP) {
                mesg=1;
            }
            else
                if (GetCapture())
                    mesg=2;
                else
                if (!IsWindowEnabled(hWnd))
                    mesg=3;
                else
                {
                   Win32BaseWindow *window;

                    window = Win32BaseWindow::GetWindowFromHandle(hWnd);
                    if(!window) {
                        return FALSE; //should never happen! (already checked)
                    }

                    hMenu = (window->getStyle() & WS_CHILD) ? 0 : (HMENU)window->GetMenu();

                    iSysStat = (window->GetSysMenu()) ? GetMenuState(GetSubMenu(window->GetSysMenu(), 0),
                                                                     cmd, MF_BYCOMMAND) : -1 ;
                    iStat = (hMenu) ? GetMenuState(hMenu, cmd, MF_BYCOMMAND) : -1 ;

                    if (iSysStat!=-1)
                    {
                        if (iSysStat & (MF_DISABLED|MF_GRAYED))
                            mesg=4;
                        else
                            mesg=WM_SYSCOMMAND;
                    }
                    else
                    {
                        if (iStat!=-1)
                        {
                            if (IsIconic(hWnd)) {
                                mesg=5;
                            }
                            else
                            {
                                if (iStat & (MF_DISABLED|MF_GRAYED))
                                    mesg=6;
                                else
                                    mesg=WM_COMMAND;
                            }
                        }
                        else
                            mesg=WM_COMMAND;
                    }
                }
                if ( mesg==WM_COMMAND || mesg==WM_SYSCOMMAND )
                {
                    SendMessageA(hWnd, mesg, cmd, 0x00010000L);
                }
                else
                {
                    /*  some reasons for NOT sending the WM_{SYS}COMMAND message:
                     *   #0: unknown (please report!)
                     *   #1: for WM_KEYUP,WM_SYSKEYUP
                     *   #2: mouse is captured
                     *   #3: window is disabled
                     *   #4: it's a disabled system menu option
                     *   #5: it's a menu option, but window is iconic
                     *   #6: it's a menu option, but disabled
                     */
                    if(mesg==0)
                            dprintf(("ERROR: unknown reason - please report!"));
                    else    dprintf(("but won't send WM_{SYS}COMMAND, reason is #%d\n",mesg));

                }
                return TRUE;
            }
            dprintf(("TranslateAccelerator: not match for %x %x %x", fVirt, key, cmd));
    }
    return FALSE;
}
/*****************************************************************************
 * Name      : int WIN32API TranslateAcceleratorA
 * Purpose   : Translate WM_*KEYDOWN messages to WM_COMMAND messages
 *             according to Accelerator table
 * Parameters: HWND hwnd, HACCEL haccel, LPMSG lpmsg
 * Variables :
 * Result    : int FALSE (no accelerator found) TRUE (accelerator found)
 * Remark    : if a accelerator is found it is not neccesarely executed
 *             depends on window stat
 *
 *****************************************************************************/
INT WINAPI TranslateAcceleratorA(HWND hWnd, HACCEL hAccel, LPMSG msg)
{
    /* YES, Accel16! */
    LPACCEL lpAccelTbl;
    Win32BaseWindow *window;
    int i;

    SetLastError(ERROR_SUCCESS);
    if (msg == NULL)
    {
          dprintf(("TranslateAcceleratorAmsg null; should hang here to be win compatible"));
          SetLastError(ERROR_INVALID_PARAMETER);
          return 0;
    }
    if (!hAccel || !(lpAccelTbl = (LPACCEL)GlobalLock(hAccel)))
    {
          dprintf(("TranslateAcceleratorA: invalid accel handle=%x", hAccel));
          SetLastError(ERROR_INVALID_PARAMETER);
          return 0;
    }
    window = Win32BaseWindow::GetWindowFromHandle(hWnd);
    if(!window) {
          dprintf(("TranslateAccelerator, window %x not found", hWnd));
          SetLastError(ERROR_INVALID_WINDOW_HANDLE);
          return 0;
    }
    if ((msg->message != WM_KEYDOWN &&
         msg->message != WM_KEYUP &&
         msg->message != WM_SYSKEYDOWN &&
         msg->message != WM_SYSKEYUP &&
         msg->message != WM_CHAR))
    {
          return 0;
    }

/*    TRACE_(accel)("TranslateAccelerators hAccel=%04x, hWnd=%04x,"
      "msg->hwnd=%04x, msg->message=%04x, wParam=%08x, lParam=%lx\n",
      hAccel,hWnd,msg->hwnd,msg->message,msg->wParam,msg->lParam); */

    i = 0;
    do
    {
        if (KBD_translate_accelerator(hWnd,msg,lpAccelTbl[i].fVirt,
                                      lpAccelTbl[i].key,lpAccelTbl[i].cmd))
        {
            return 1;
        }
    }
    while ((lpAccelTbl[i++].fVirt & 0x80) == 0);

//    WARN_(accel)("couldn't translate accelerator key\n");
    return 0;
}
//******************************************************************************
//******************************************************************************
BOOL WIN32API TranslateMDISysAccel(HWND hwndClient, LPMSG msg)
{
    SetLastError(ERROR_SUCCESS);

    if(IsWindow(hwndClient) && (msg->message == WM_KEYDOWN || msg->message == WM_SYSKEYDOWN))
    {
        Win32MDIClientWindow *clientWnd;
        Win32MDIChildWindow  *mdichild;
        HWND                  hwndChild;

        clientWnd = (Win32MDIClientWindow *)Win32BaseWindow::GetWindowFromHandle(hwndClient);
        if(!clientWnd) {
            dprintf(("TranslateMDISysAccel window %x not found", hwndClient));
            SetLastError(ERROR_INVALID_WINDOW_HANDLE);
            return FALSE;
        }

        mdichild = clientWnd->getActiveChild();
        if(!mdichild) {
            dprintf(("TranslateMDISysAccel NO active MDI child!!"));
            return FALSE;
        }
        hwndChild = mdichild->getWindowHandle();

        if(IsWindow(hwndChild) && !(GetWindowLongA(hwndChild,GWL_STYLE) & WS_DISABLED) )
	    {
    	    WPARAM	wParam = 0;

	        /* translate if the Ctrl key is down and Alt not. */
	        if( (GetKeyState(VK_CONTROL) & 0x8000) &&
	            !(GetKeyState(VK_MENU) & 0x8000))
	        {
		        switch( msg->wParam )
		        {
		        case VK_F6:
		        case VK_TAB:
			        wParam = ( GetKeyState(VK_SHIFT) & 0x8000 )
				                ? SC_NEXTWINDOW : SC_PREVWINDOW;
			        break;
		        case VK_F4:
		        case VK_RBUTTON:
			        wParam = SC_CLOSE;
			        break;
		        default:
			        return 0;
		        }
	            SendMessageA(hwndChild, WM_SYSCOMMAND, wParam, (LPARAM)msg->wParam);
	            return 1;
	        }
	    }
    }
    return 0; /* failure */
}
/**********************************************************************
 *			LoadAccelerators32W	[USER.177]
 * The image layout seems to look like this (not 100% sure):
 * 00:	BYTE	type		type of accelerator
 * 01:	BYTE	pad		(to WORD boundary)
 * 02:	WORD	event
 * 04:	WORD	IDval		
 * 06:	WORD	pad		(to DWORD boundary)
 */
HACCEL WINAPI LoadAcceleratorsW(HINSTANCE instance,LPCWSTR lpTableName)
{
    HRSRC hRsrc;
    HACCEL hMem,hRetval=0;
    DWORD size;

    if (!(hRsrc = FindResourceW( instance, lpTableName, RT_ACCELERATORW )))
    {
        dprintf(("LoadAcceleratorsW couldn't find accelerator table resource %x %x", instance, lpTableName));
        return 0;
    }
    else {
        hMem = LoadResource( instance, hRsrc );
        size = SizeofResource( instance, hRsrc );
        if(size >= sizeof(PE_ACCEL))
        {
	        LPPE_ACCEL accel_table = (LPPE_ACCEL) hMem;
	        LPACCEL accel;
	        int i,nrofaccells = size/sizeof(PE_ACCEL);

	        hRetval = GlobalAlloc(0,sizeof(ACCEL)*nrofaccells);
	        accel   = (LPACCEL)GlobalLock(hRetval);
	
	        for (i=0;i<nrofaccells;i++) {
		        accel[i].fVirt = accel_table[i].fVirt;
		        accel[i].key = accel_table[i].key;
		        accel[i].cmd = accel_table[i].cmd;
	        }
	        accel[i-1].fVirt |= 0x80;
        }
    }
    dprintf(("LoadAcceleratorsW returned %x %x %x\n", instance, lpTableName, hRetval));
    return hRetval;
}

HACCEL WINAPI LoadAcceleratorsA(HINSTANCE instance,LPCSTR lpTableName)
{
	LPWSTR	 uni;
	HACCEL result;
	if (HIWORD(lpTableName))
		uni = HEAP_strdupAtoW( GetProcessHeap(), 0, lpTableName );
	else
		uni = (LPWSTR)lpTableName;
	result = LoadAcceleratorsW(instance,uni);
	if (HIWORD(uni)) HeapFree( GetProcessHeap(), 0, uni);
	return result;
}

/**********************************************************************
 *             CopyAcceleratorTable32A   (USER32.58)
 */
INT WINAPI CopyAcceleratorTableA(HACCEL src, LPACCEL dst, INT entries)
{
  return CopyAcceleratorTableW(src, dst, entries);
}

/**********************************************************************
 *             CopyAcceleratorTable32W   (USER32.59)
 *
 * By mortene@pvv.org 980321
 */
INT WINAPI CopyAcceleratorTableW(HACCEL src, LPACCEL dst, INT entries)
{
  int i,xsize;
  LPACCEL accel = (LPACCEL)GlobalLock(src);
  BOOL done = FALSE;

  /* Do parameter checking to avoid the explosions and the screaming
     as far as possible. */
  if((dst && (entries < 1)) || (src == (HACCEL)NULL) || !accel)
  {
        dprintf(("CopyAcceleratorTableW: Application sent invalid parameters (%p %p %d).\n", (LPVOID)src, (LPVOID)dst, entries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
  }
  xsize = GlobalSize(src)/sizeof(ACCEL);
  if (xsize>entries) entries=xsize;

  i=0;
  while(!done) {
    /* Spit out some debugging information. */
//    TRACE_(accel)("accel %d: type 0x%02x, event '%c', IDval 0x%04x.\n",
//	  i, accel[i].fVirt, accel[i].key, accel[i].cmd);

    /* Copy data to the destination structure array (if dst == NULL,
       we're just supposed to count the number of entries). */
    if(dst) {
        dst[i].fVirt = accel[i].fVirt;
        dst[i].key = accel[i].key;
        dst[i].cmd = accel[i].cmd;

        /* Check if we've reached the end of the application supplied
           accelerator table. */
        if(i+1 == entries) {
	        /* Turn off the high order bit, just in case. */
	        dst[i].fVirt &= 0x7f;
	        done = TRUE;
        }
    }

    /* The highest order bit seems to mark the end of the accelerator
       resource table, but not always. Use GlobalSize() check too. */
    if((accel[i].fVirt & 0x80) != 0) done = TRUE;

    i++;
  }

  return i;
}

/*********************************************************************
 *                    CreateAcceleratorTable   (USER32.64)
 *
 * By mortene@pvv.org 980321
 */
HACCEL WINAPI CreateAcceleratorTableA(LPACCEL lpaccel, INT cEntries)
{
  HACCEL	hAccel;
  LPACCEL	accel;
  int		i;

  /* Do parameter checking just in case someone's trying to be
     funny. */
  if(cEntries < 1) {
        dprintf(("CreateAcceleratorTableA: Application sent invalid parameters (%p %d).\n", lpaccel, cEntries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }
  dprintf(("FIXME: CreateAcceleratorTableA: should check that the accelerator descriptions are valid return NULL and SetLastError() if not"));

  /* Allocate memory and copy the table. */
  hAccel = GlobalAlloc(0,cEntries*sizeof(ACCEL));

  if(!hAccel) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return (HACCEL)NULL;
  }
  accel = (LPACCEL)GlobalLock(hAccel);
  for (i=0;i<cEntries;i++) {
  	    accel[i].fVirt = lpaccel[i].fVirt;
  	    accel[i].key = lpaccel[i].key;
  	    accel[i].cmd = lpaccel[i].cmd;
  }
  /* Set the end-of-table terminator. */
  accel[cEntries-1].fVirt |= 0x80;

  dprintf(("CreateAcceleratorTableA %x %x returned %x\n", lpaccel, cEntries, hAccel));
  return hAccel;
}

/*********************************************************************
 *                    CreateAcceleratorTableW   (USER32.64)
 *
 *
 */
HACCEL WINAPI CreateAcceleratorTableW(LPACCEL lpaccel, INT cEntries)
{
  HACCEL	hAccel;
  LPACCEL	accel;
  int		i;
  char		ckey;

  /* Do parameter checking just in case someone's trying to be
     funny. */
  if(cEntries < 1) {
        dprintf(("CreateAcceleratorTableW: Application sent invalid parameters (%p %d).\n", lpaccel, cEntries));
        SetLastError(ERROR_INVALID_PARAMETER);
        return NULL;
  }
  dprintf(("FIXME: CreateAcceleratorTableW: should check that the accelerator descriptions are valid return NULL and SetLastError() if not"));

  /* Allocate memory and copy the table. */
  hAccel = GlobalAlloc(0,cEntries*sizeof(ACCEL));

  if(!hAccel) {
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return (HACCEL)NULL;
  }
  accel = (LPACCEL)GlobalLock(hAccel);

  for (i=0;i<cEntries;i++) {
       accel[i].fVirt = lpaccel[i].fVirt;
       if( !(accel[i].fVirt & FVIRTKEY) ) {
    	    ckey = (char) lpaccel[i].key;
            MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, &ckey, 1, &accel[i].key, 1);
       }
       else  accel[i].key = lpaccel[i].key;
       accel[i].cmd = lpaccel[i].cmd;
  }

  /* Set the end-of-table terminator. */
  accel[cEntries-1].fVirt |= 0x80;

  dprintf(("CreateAcceleratorTableW %x %x returned %x\n", lpaccel, cEntries, hAccel));
  return hAccel;
}

/******************************************************************************
 * DestroyAcceleratorTable [USER32.130]
 * Destroys an accelerator table
 *
 * NOTES
 *    By mortene@pvv.org 980321
 *
 * PARAMS
 *    handle [I] Handle to accelerator table
 *
 * RETURNS STD
 */
BOOL WINAPI DestroyAcceleratorTable( HACCEL handle )
{
    return GlobalFree(handle);
}

