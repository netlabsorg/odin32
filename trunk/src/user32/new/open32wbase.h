/* $Id: open32wbase.h,v 1.1 1999-09-04 20:03:10 dengert Exp $ */
/*
 * Open32 Window fake Base Class for OS/2
 *
 *
 * Copyright 1999      Daniela Engert (dani@ngrt.de)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#ifndef __OPEN32WINBASE_H__
#define __OPEN32WINBASE_H__

extern "C" {
void _System WinSetDAXData (HWND, PVOID);
}

#pragma pack(1)

class fakeOpen32WinBaseClass
{
   public:
      virtual BOOL      clientArea(PRECTL pRectl) { pRectl=pRectl; return FALSE; }
      virtual BOOL      redraw() { return bRedraw; }
      virtual VOID      setRedraw( BOOL bOn = TRUE ) { bRedraw = bOn; }

//   private:
      char              type;
      LONG              lUserData;
      HWND              hwndThisObject;
      HWND              hwndFocusPartner;
      ULONG             origStyle;                /* Original windows style */
      ULONG             origStyleEx;              /* Original extended windows style */
      ULONG             hevObjWinCreated;         /* Posted by ObjWin thread upon creation of WinobjWin object */
      #define  PMS_SIZE  10
      INT               PMSIdx_beginPaint;        /* Poor Man's Stack index for begin paint HPS handles */
      ULONG             PMS_beginPaint[PMS_SIZE]; /* Poor Man's Stack for begin paint HPS handles       */
      Win32WndClass    *pWindowClass;             /* ptr to window class        */
      PVOID             pWinExtra;
      PVOID             beginPaintHPS_HDC;
      PVOID             ownDeviceContext;
      ULONG             ulNextChildId;
      PVOID             pfnwpWindows;
      PVOID             pfnwpPMOriginal;

      ULONG             bZeroId:1,                /* See Note 1 */
                        bInCreate:1,
                        bDragDrop:1,              /* Set by DragAcceptFiles() */
                        bEraseBkgnd:1,            /* See Note 2 */
                        bPSEraseFlag:1,           /* See Note 2 */
                        bSupressErase:1,          /* See Note 2 */
                        bPaintNow:1,              /* See Note 2 */
                        bRedraw:1,
                        bDestroyed:1,
                        bSendSetFocus:1,
                        bGotSetFocus:1,
                        ulReserved:21;
};

#pragma pack()

/* NOTES:
 *  1) The window ID for many DAX windows will default to zero.  This is
 *     because the window ID is taken from the hmenu parameter in the
 *     CreateWindow call and this parameter will very often be 0 for child
 *     windows.  Since PM sends many control messages with only an id as a
 *     means of identification, we are forced to substitute another window ID
 *     internally when a window is created with an ID of 0.  The private
 *     bZeroId flag indicates when this happens.  One disadvantage is that
 *     the window ID as queried from PM for a DAX window will not be the real
 *     DAX window ID when it should be zero.  Because of this the windowId
 *     member function should be used any time the ID is going to be passed
 *     back to a DAX app (in a message or structure, for instance).
 *     Fortunately this does not happen often.
 *  2) bEraseBkgnd is set to TRUE if the background needs to be erased.  The
 *     bPSEraseFlag tells BeginPaint what to set the PAINTSTRUCT fErased flag
 *     to when bEraseBkgnd is FALSE.  The bPSEraseFlag is necessary because
 *     RedrawWindow must set bEraseBkgnd to FALSE if RDW_ERASENOW is used,
 *     but BeginPaint must still set fErase to TRUE if the WM_ERASEBKGND
 *     message returned FALSE.
 */

#endif

