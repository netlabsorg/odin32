/*
 * Native resource management (dialogs, bitmaps, ...)
 *
 *
 * Copyright (c) 1999 Christoph Bratschi (cbratschi@datacomm.ch)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include <os2win.h>
#include "user32.h"
#include "wndproc.h"
#include "wndclass.h"

//******************************************************************************
// CB: Loads and starts a native OS/2 dialog with Win32 DlgProc
//     (used in COMCTL32)
// full name: NativeDialogBoxIndirectParam
//******************************************************************************
INT WINAPI NativeDlgBoxIP(HMODULE hmodule,
                          HINSTANCE hinst, LPCSTR lpszName,
                          HWND hwndParent, DLGPROC dlgproc,
                          LPARAM lParamInit)
{
  BOOL rc;
  DLGTEMPLATE *os2dlg;
  HRSRC hRes;
/* //CB: synchronize with new dialog.cpp code
  dprintf(("NativeDlgBoxIP"));

  hRes = O32_FindResource(hmodule,lpszName,RT_DIALOGA);
  if (hRes == 0) return (INT)-1;

  dprintf((" hRes = %d",hRes));

  os2dlg = (DLGTEMPLATE*)O32_LoadResource(hmodule,hRes);
  if (os2dlg == NULL) return (INT)-1;

  dprintf((" os2dlg = %d",os2dlg));

  Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc,os2dlg);
  rc = O32_DialogBoxIndirectParam(hinst,os2dlg,hwndParent,(DLGPROC_O32)dialog->GetOS2Callback(),lParamInit);
  //dialog already destroyed when this returns
  dprintf(("NativeDlgBoxIP returned %X\n", rc));
*/
  return rc;
}
//******************************************************************************
// CB: Loads and starts a native OS/2 dialog with Win32 DlgProc
//     (used in COMCTL32)
// full name: NativeCreateDialogIndirectParam
//******************************************************************************
INT WINAPI NativeCreateDlgIP(HMODULE hmodule,
                                HINSTANCE hinst, LPCSTR lpszName,
                                HWND hwndParent, DLGPROC dlgproc,
                                LPARAM lParamInit)
{
  BOOL rc;
  DLGTEMPLATE *os2dlg;
  HRSRC hRes;
/* //CB: synchronize with new dialog.cpp code
  dprintf(("NativeCreateDlgBoxIP"));

  hRes = O32_FindResource(hmodule,lpszName,RT_DIALOGA);
  if (hRes == 0) return (INT)-1;

  dprintf((" hRes = %d",hRes));

  os2dlg = (DLGTEMPLATE*)O32_LoadResource(hmodule,hRes);
  if (os2dlg == NULL) return (INT)-1;

  dprintf((" os2dlg = %d",os2dlg));

  Win32WindowProc *dialog = new Win32WindowProc((WNDPROC)dlgproc,os2dlg);
  rc = O32_CreateDialogIndirectParam(hinst,os2dlg,hwndParent,(DLGPROC_O32)dialog->GetOS2Callback(),lParamInit);
  //dialog already destroyed when this returns
  dprintf(("NativeDlgBoxIP returned %X\n", rc));
*/
  return rc;
}
//******************************************************************************
// CB: loads OS/2 bitmaps
//******************************************************************************
HBITMAP WINAPI NativeLoadBitmap(HINSTANCE hInstance,LPCTSTR lpBitmapName)
{
  return O32_LoadBitmap(hInstance,lpBitmapName);
}
