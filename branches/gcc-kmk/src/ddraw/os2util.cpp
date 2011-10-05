/* $Id: OS2UTIL.CPP,v 1.16 2003-02-04 17:40:55 sandervl Exp $ */

/*
 * OS/2 Utility functions
 *
 * Copyright 1998 Sander van Leeuwen (sandervl@xs4all.nl)
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#define INCL_DOS
#define INCL_DOSMEMMGR
#define INCL_DOSDEVICES
#define INCL_WIN
#include <os2wrap.h>
#include <misc.h>
#include "os2util.h"
#include "cio2.h"

//******************************************************************************
//******************************************************************************
char *OS2AllocMem(ULONG size)
{
 PVOID  lpMem;
 APIRET rc;

  rc = DosAllocMem(&lpMem, size, PAG_READ|PAG_WRITE|PAG_COMMIT);
  if(rc) {
    dprintf(("DDRAW: DosAllocMem returned %d", rc));
    return(NULL);
  }
  return((char *)lpMem);
}
//******************************************************************************
//******************************************************************************
void OS2FreeMem(char *lpMem)
{
 APIRET rc;

  rc = DosFreeMem(lpMem);
  if(rc) {
    dprintf(("DDRAW: DosFreeMem returned %d", rc));
  }
}//******************************************************************************
//******************************************************************************
void OS2MaximizeWindow(HWND hwndClient)
{
  WinSetWindowPos(hwndClient, HWND_TOP, 0, 0, 0, 0, SWP_MAXIMIZE);
}
//******************************************************************************
//******************************************************************************
BOOL OSLibPostMessage(HWND hwnd, ULONG msg, ULONG mp1, ULONG mp2)
{
    return WinPostMsg(hwnd, msg, (MPARAM)mp1, (MPARAM)mp2);
}
//******************************************************************************
//******************************************************************************

extern "C" {
BOOL    APIENTRY _GpiEnableYInversion (HPS hps, LONG lHeight);
}

void InverseDC(HDC hdc, LONG lHeight)
{
 USHORT sel = RestoreOS2FS();

// _GpiEnableYInversion( WinGetPS( Win32ToOS2Handle( WindowFromDC(hdc) ) ), lHeight);
 _GpiEnableYInversion( hdc, lHeight);
 SetFS(sel);

}
//******************************************************************************
//******************************************************************************
int InitIO()
{
#if 0
  WORD  gdt;
  HRESULT rc;
  HFILE  device;
  ULONG  ulAction;

  rc = DosOpen( "\\dev\\fastio$",
                &device,
                &ulAction,
                0,0,1,
                OPEN_ACCESS_READWRITE|OPEN_SHARE_DENYNONE,
                0);
  if(rc)
    return rc;

  rc = DosDevIOCtl( device, 118, 100, 0,0,0,&gdt,2,&ulAction);

  DosClose(device);

  if(rc)
  {
    return rc;
  }

  USHORT sel = RestoreOS2FS();

  io_init2(gdt);

  SetFS(sel);
#endif
  return 0;
}
//******************************************************************************
//******************************************************************************
