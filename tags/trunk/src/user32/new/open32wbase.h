/* $Id: open32wbase.h,v 1.4 1999-09-05 16:06:21 dengert Exp $ */
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
ULONG _System DosBeep (ULONG, ULONG);
}

#define DosBeep(x,y)

#pragma pack(1)

class fakeOpen32WinBaseClass
{
   public:
      virtual BOOL      ni08(void) { DosBeep (1000,10); return FALSE; }
      virtual BOOL      ni10(void) { DosBeep (1100,10); return FALSE; }
      virtual BOOL      ni18(void) { DosBeep (1200,10); return FALSE; }
      virtual BOOL      ni20(void) { DosBeep (1300,10); return FALSE; }
      virtual BOOL      ni28(void) { DosBeep (1400,10); return FALSE; }
      virtual BOOL      ni30(void) { DosBeep (1500,10); return FALSE; }
      virtual BOOL      ni38(void) { DosBeep (1600,10); return FALSE; }
      virtual BOOL      ni40(void) { DosBeep (1700,10); return FALSE; }
      virtual BOOL      ni48(void) { DosBeep (1800,10); return FALSE; }
      virtual BOOL      ni50(void) { DosBeep (1900,10); return FALSE; }
      virtual BOOL      ni58(void) { DosBeep (2000,10); return FALSE; }
      virtual BOOL      ni60(void) { DosBeep (2100,10); return FALSE; }
      virtual BOOL      ni68(void) { DosBeep (2200,10); return FALSE; }
      virtual BOOL      ni70(void) { DosBeep (2300,10); return FALSE; }
      virtual BOOL      ni78(void) { DosBeep (2400,10); return FALSE; }
      virtual BOOL      ni80(void) { DosBeep (2500,10); return FALSE; }
      virtual BOOL      ni88(void) { DosBeep (2600,10); return FALSE; }
      virtual BOOL      clientArea(PRECTL pRectl) { pRectl=pRectl; return FALSE; }
      virtual BOOL      redraw() { return bRedraw; }
      virtual VOID      setRedraw( BOOL bOn = TRUE ) { bRedraw = bOn; }
      virtual BOOL      niA8(void) { DosBeep (2700,10); return FALSE; }
      virtual BOOL      niB0(void) { DosBeep (2800,10); return FALSE; }
      virtual BOOL      niB8(void) { DosBeep (2900,10); return FALSE; }
      virtual BOOL      niC0(void) { DosBeep (3000,10); return FALSE; }
      virtual BOOL      niC8(void) { DosBeep (3100,10); return FALSE; }
      virtual BOOL      niD0(void) { DosBeep (3200,10); return FALSE; }

      char              type;
      ULONG             filler1[1];
      HWND              hwndThis;
      ULONG             filler2[15];
      Win32WndClass    *pWindowClass;             /* ptr to window class        */
      ULONG             filler3[2];
      PVOID             ownDC;
      ULONG             filler4[3];
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

