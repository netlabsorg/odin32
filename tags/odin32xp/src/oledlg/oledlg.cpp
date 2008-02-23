/* $Id: oledlg.cpp,v 1.4 2005-06-26 13:37:55 sao2l02 Exp $ */
/*
 *	OLEDLG library
 *
 *	Copyright 1998	Patrik Stridvall
 *	Copyright 1999  Jens Wiessner
 */

/*	At the moment, these are only empty stubs.
 */

// ><DJR 17.05.99 Force to use C-interfaces for now to prevent CALLBACK definition compiler error
#define CINTERFACE 1

// ><DJR 17.05.99 Move standard includes to before os2win.h [memcmp]
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <wchar.h>

// ><DJR 17.05.99 Prevent CALLCONV becoming _System
#include <win/wtypes.h>
#include <winbase.h>
#undef CALLCONV

#include <os2win.h>
#include <odinwrap.h>
#include <ole.h>
#include <winreg.h>

#include "wine/obj_base.h"
#include "wine/obj_misc.h"
#include "wine/obj_inplace.h"
#include "wine/obj_dataobject.h"
#include "wine/obj_oleobj.h"
#include "wine/obj_marshal.h"
#include "wine/obj_moniker.h"
#include "wine/obj_clientserver.h"
#include "wine/obj_dragdrop.h"
#include <heapstring.h>
#include <prsht.h>
#include <oledlg.h>

#include <stdio.h>
#include <string.h>
#include "windef.h"
#include "winerror.h"
#include "winbase.h"
#include "wine/obj_base.h"


ODINDEBUGCHANNEL(oledlg)

/***********************************************************************
 *           OleUIAddVerbMenuA (OLEDLG.1)
 */
BOOL WINAPI OleUIAddVerbMenuA( LPOLEOBJECT lpOleObj, LPCSTR lpszShortType,
  HMENU hMenu, UINT uPos, UINT uIDVerbMin, UINT uIDVerbMax,
  BOOL bAddConvert, UINT idConvert, HMENU *lphMenu)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIAddVerbMenuA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUICanConvertOrActivateAs (OLEDLG.2)
 */
BOOL WINAPI OleUICanConvertOrActivateAs(REFCLSID rClsid, BOOL fIsLinkedObject, WORD wFormat)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUICanConvertOrActivateAs not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIInsertObjectA (OLEDLG.3)
 */
UINT WINAPI OleUIInsertObjectA(LPOLEUIINSERTOBJECTA lpOleUIInsertObject)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIInsertObjectA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPasteSpecialA (OLEDLG.4)
 */
UINT WINAPI OleUIPasteSpecialA(LPOLEUIPASTESPECIALA lpOleUIPasteSpecial)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIPasteSpecialA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIEditLinksA (OLEDLG.5)
 */
UINT WINAPI OleUIEditLinksA(LPOLEUIEDITLINKSA lpOleUIEditLinks)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIEditLinksA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIChangeIconA (OLEDLG.6)
 */
UINT WINAPI OleUIChangeIconA(LPOLEUICHANGEICONA lpOleUIChangeIcon)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIChangeIconA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIConvertA (OLEDLG.7)
 */
UINT WINAPI OleUIConvertA(LPOLEUICONVERTA lpOleUIConvert)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIConvertA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIBusyA (OLEDLG.8)
 */
UINT WINAPI OleUIBusyA(LPOLEUIBUSYA lpOleUIBusy)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIBusyA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIUpdateLinksA (OLEDLG.9)
 */
BOOL WINAPI OleUIUpdateLinksA(LPOLEUILINKCONTAINERA lpOleUILinkCntr,
  HWND hwndParent, LPSTR lpszTitle, INT cLinks)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIUpdateLinksA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIPromptUserA (OLEDLG.10)
 */
INT CDECL OleUIPromptUserA(INT nTemplate, HWND hwndParent, ...)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIPromptUserA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIObjectPropertiesA (OLEDLG.11)
 */
UINT WINAPI OleUIObjectPropertiesA( LPOLEUIOBJECTPROPSA lpOleUIObjectProps)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIObjectPropertiesA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeSourceA (OLEDLG.12)
 */
UINT WINAPI OleUIChangeSourceA( LPOLEUICHANGESOURCEA lpOleUIChangeSource)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIChangeSourceA not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPromptUserW (OLEDLG.13)
 */
INT CDECL OleUIPromptUserW(INT nTemplate, HWND hwndParent, ...)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIPromptUserW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIAddVerbMenuW (OLEDLG.14)
 */
BOOL WINAPI OleUIAddVerbMenuW( LPOLEOBJECT lpOleObj, LPCWSTR lpszShortType,
  HMENU hMenu, UINT uPos, UINT uIDVerbMin, UINT uIDVerbMax,
  BOOL bAddConvert, UINT idConvert, HMENU *lphMenu)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIAddVerbMenuW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIBusyW (OLEDLG.15)
 */
UINT WINAPI OleUIBusyW(LPOLEUIBUSYW lpOleUIBusy)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIBusyW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeIconW (OLEDLG.16)
 */
UINT WINAPI OleUIChangeIconW( LPOLEUICHANGEICONW lpOleUIChangeIcon)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIChangeIconW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeSourceW (OLEDLG.17)
 */
UINT WINAPI OleUIChangeSourceW( LPOLEUICHANGESOURCEW lpOleUIChangeSource)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIChangeSourceW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIConvertW (OLEDLG.18)
 */
UINT WINAPI OleUIConvertW(LPOLEUICONVERTW lpOleUIConvert)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIConvertW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIEditLinksW (OLEDLG.19)
 */
UINT WINAPI OleUIEditLinksW(LPOLEUIEDITLINKSW lpOleUIEditLinks)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIEditLinksW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIInsertObjectW (OLEDLG.20)
 */
UINT WINAPI OleUIInsertObjectW(LPOLEUIINSERTOBJECTW lpOleUIInsertObject)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIInsertObjectW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIObjectPropertiesW (OLEDLG.21)
 */
UINT WINAPI OleUIObjectPropertiesW( LPOLEUIOBJECTPROPSW lpOleUIObjectProps)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIObjectPropertiesW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPasteSpecialW (OLEDLG.22)
 */
UINT WINAPI OleUIPasteSpecialW(LPOLEUIPASTESPECIALW lpOleUIPasteSpecial)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIPasteSpecialW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIUpdateLinksW (OLEDLG.23)
 */
BOOL WINAPI OleUIUpdateLinksW( LPOLEUILINKCONTAINERW lpOleUILinkCntr,
  HWND hwndParent, LPWSTR lpszTitle, INT cLinks)
{
#ifdef DEBUG
  dprintf(("OLEDLG: OleUIUpdateLinksW not implemented\n"));
#endif
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
