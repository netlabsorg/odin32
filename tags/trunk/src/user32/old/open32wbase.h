/* $Id: open32wbase.h,v 1.1 2001-05-11 08:35:18 sandervl Exp $ */
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
ULONG _System PIEPS (ULONG, ULONG);
void _System _WinSetDAXData (HWND, PVOID);
}

inline void _System WinSetDAXData (HWND a, PVOID b)
{
 USHORT sel = RestoreOS2FS();

    _WinSetDAXData(a, b);
    SetFS(sel);
} 

#define PIEPS(x,y)

#pragma pack(1)

class fakeOpen32WinBaseClass
{
   public:
      virtual BOOL      ni08(void) { PIEPS (1000,10); return FALSE; }
      virtual BOOL      ni10(void) { PIEPS (1100,10); return FALSE; }
      virtual BOOL      ni18(void) { PIEPS (1200,10); return FALSE; }
      virtual BOOL      ni20(void) { PIEPS (1300,10); return FALSE; }
      virtual BOOL      ni28(void) { PIEPS (1400,10); return FALSE; }
      virtual BOOL      ni30(void) { PIEPS (1500,10); return FALSE; }
      virtual BOOL      ni38(void) { PIEPS (1600,10); return FALSE; }
      virtual BOOL      ni40(void) { PIEPS (1700,10); return FALSE; }
      virtual BOOL      ni48(void) { PIEPS (1800,10); return FALSE; }
      virtual BOOL      ni50(void) { PIEPS (1900,10); return FALSE; }
      virtual BOOL      ni58(void) { PIEPS (2000,10); return FALSE; }
      virtual BOOL      ni60(void) { PIEPS (2100,10); return FALSE; }
      virtual BOOL      ni68(void) { PIEPS (2200,10); return FALSE; }
      virtual BOOL      ni70(void) { PIEPS (2300,10); return FALSE; }
      virtual BOOL      ni78(void) { PIEPS (2400,10); return FALSE; }
      virtual BOOL      ni80(void) { PIEPS (2500,10); return FALSE; }
      virtual BOOL      ni88(void) { PIEPS (2600,10); return FALSE; }
      virtual BOOL      clientArea(PRECTL pRectl) { pRectl=pRectl; return FALSE; }
      virtual BOOL      redraw() { return bRedraw; }
      virtual VOID      setRedraw( BOOL bOn = TRUE ) { bRedraw = bOn; }
      virtual BOOL      niA8(void) { PIEPS (2700,10); return FALSE; }
      virtual BOOL      niB0(void) { PIEPS (2800,10); return FALSE; }
      virtual BOOL      niB8(void) { PIEPS (2900,10); return FALSE; }
      virtual BOOL      niC0(void) { PIEPS (3000,10); return FALSE; }
      virtual BOOL      niC8(void) { PIEPS (3100,10); return FALSE; }
      virtual BOOL      niD0(void) { PIEPS (3200,10); return FALSE; }

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

