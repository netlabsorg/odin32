/* $Id: oledlg.cpp,v 1.1 1999-08-30 22:16:48 sandervl Exp $ */
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
//#include <uconv.h>
#undef CALLCONV

#include <os2win.h>
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
#include "debugtools.h"
#include <debugdefs.h>


DEFAULT_DEBUG_CHANNEL(ole)

/***********************************************************************
 *           OleUIAddVerbMenu32A (OLEDLG.1)
 */
BOOL WINAPI OleUIAddVerbMenuA( LPOLEOBJECT lpOleObj, LPCSTR lpszShortType,
  HMENU hMenu, UINT uPos, UINT uIDVerbMin, UINT uIDVerbMax,
  BOOL bAddConvert, UINT idConvert, HMENU *lphMenu)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUICanConvertOrActivateAs32 (OLEDLG.2)
 */
BOOL WINAPI OleUICanConvertOrActivateAs(REFCLSID rClsid, BOOL fIsLinkedObject, WORD wFormat)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIInsertObject32A (OLEDLG.3)
 */
UINT WINAPI OleUIInsertObjectA(LPOLEUIINSERTOBJECTA lpOleUIInsertObject)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPasteSpecial32A (OLEDLG.4)
 */
UINT WINAPI OleUIPasteSpecialA(LPOLEUIPASTESPECIALA lpOleUIPasteSpecial)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIEditLinks32A (OLEDLG.5)
 */
UINT WINAPI OleUIEditLinksA(LPOLEUIEDITLINKSA lpOleUIEditLinks)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIChangeIcon32A (OLEDLG.6)
 */
UINT WINAPI OleUIChangeIconA(LPOLEUICHANGEICONA lpOleUIChangeIcon)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIConvert32A (OLEDLG.7)
 */
UINT WINAPI OleUIConvertA(LPOLEUICONVERTA lpOleUIConvert)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}


/***********************************************************************
 *           OleUIBusy32A (OLEDLG.8)
 */
UINT WINAPI OleUIBusyA(LPOLEUIBUSYA lpOleUIBusy)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIUpdateLinks32A (OLEDLG.9)
 */
BOOL WINAPI OleUIUpdateLinksA(LPOLEUILINKCONTAINERA lpOleUILinkCntr,
  HWND hwndParent, LPSTR lpszTitle, INT cLinks)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIPromptUser32A (OLEDLG.10)
 */
INT __cdecl OleUIPromptUserA(INT nTemplate, HWND hwndParent, ...)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIObjectProperties32A (OLEDLG.11)
 */
UINT WINAPI OleUIObjectPropertiesA( LPOLEUIOBJECTPROPSA lpOleUIObjectProps)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeSource32A (OLEDLG.12)
 */
UINT WINAPI OleUIChangeSourceA( LPOLEUICHANGESOURCEA lpOleUIChangeSource)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPromptUser32W (OLEDLG.13)
 */
INT __cdecl OleUIPromptUserW(INT nTemplate, HWND hwndParent, ...)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIAddVerbMenu32W (OLEDLG.14)
 */
BOOL WINAPI OleUIAddVerbMenuW( LPOLEOBJECT lpOleObj, LPCWSTR lpszShortType,
  HMENU hMenu, UINT uPos, UINT uIDVerbMin, UINT uIDVerbMax,
  BOOL bAddConvert, UINT idConvert, HMENU *lphMenu)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}

/***********************************************************************
 *           OleUIBusy32W (OLEDLG.15)
 */
UINT WINAPI OleUIBusyW(LPOLEUIBUSYW lpOleUIBusy)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeIcon32W (OLEDLG.16)
 */
UINT WINAPI OleUIChangeIconW( LPOLEUICHANGEICONW lpOleUIChangeIcon)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIChangeSource32W (OLEDLG.17)
 */
UINT WINAPI OleUIChangeSourceW( LPOLEUICHANGESOURCEW lpOleUIChangeSource)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIConvert32W (OLEDLG.18)
 */
UINT WINAPI OleUIConvertW(LPOLEUICONVERTW lpOleUIConvert)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIEditLinks32W (OLEDLG.19)
 */
UINT WINAPI OleUIEditLinksW(LPOLEUIEDITLINKSW lpOleUIEditLinks)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIInsertObject32W (OLEDLG.20)
 */
UINT WINAPI OleUIInsertObjectW(LPOLEUIINSERTOBJECTW lpOleUIInsertObject)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIObjectProperties32W (OLEDLG.21)
 */
UINT WINAPI OleUIObjectPropertiesW( LPOLEUIOBJECTPROPSW lpOleUIObjectProps)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIPasteSpecial32W (OLEDLG.22)
 */
UINT WINAPI OleUIPasteSpecialW(LPOLEUIPASTESPECIALW lpOleUIPasteSpecial)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return OLEUI_FALSE;
}

/***********************************************************************
 *           OleUIUpdateLinks32W (OLEDLG.23)
 */
BOOL WINAPI OleUIUpdateLinksW( LPOLEUILINKCONTAINERW lpOleUILinkCntr,
  HWND hwndParent, LPWSTR lpszTitle, INT cLinks)
{
  SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
  return FALSE;
}
