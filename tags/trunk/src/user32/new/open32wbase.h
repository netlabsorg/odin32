/* $Id: open32wbase.h,v 1.2 1999-09-05 08:34:01 sandervl Exp $ */
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
      ULONG             origStyle;                
      ULONG             origStyleEx;              
      ULONG             hevObjWinCreated;         
      #define  PMS_SIZE  10
      INT               PMSIdx_beginPaint;        
      ULONG             PMS_beginPaint[PMS_SIZE]; 
      Win32WndClass    *pWindowClass;             
      PVOID             pWinExtra;
      PVOID             beginPaintHPS_HDC;
      PVOID             ownDeviceContext;
      ULONG             ulNextChildId;
      PVOID             pfnwpWindows;
      PVOID             pfnwpPMOriginal;

      ULONG             bZeroId:1,                
                        bInCreate:1,
                        bDragDrop:1,              
                        bEraseBkgnd:1,            
                        bPSEraseFlag:1,           
                        bSupressErase:1,          
                        bPaintNow:1,              
                        bRedraw:1,
                        bDestroyed:1,
                        bSendSetFocus:1,
                        bGotSetFocus:1,
                        ulReserved:21;
};

#pragma pack()

#endif

