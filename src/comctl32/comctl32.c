/*
 * Win32 common controls implementation
 *
 * Copyright (C) 1999 Achim Hasenmueller
 *
 * Based on the work of the WINE group (www.winehq.com)
 *
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */

#include "comctl32.h"
#include "progress.h"
#include "comboex.h"
#include "animate.h"
#include "datetime.h"
#include "flatsb.h"
#include "hotkey.h"
#include "ipaddress.h"
#include "monthcal.h"
#include "nativefont.h"
#include "pager.h"
#include "tab.h"
#include "status.h"
#include "header.h"
#include "updown.h"
#include "rebar.h"
#include "trackbar.h"
#include "tooltips.h"
#include "toolbar.h"
#include "treeview.h"

HANDLE COMCTL32_hHeap = (HANDLE)NULL;
HMODULE COMCTL32_hModule = 0;
LPSTR    COMCTL32_aSubclass = (LPSTR)NULL;

void RegisterCOMCTL32WindowClasses(unsigned long hinstDLL)
{
  /* create private heap */
  COMCTL32_hHeap = HeapCreate(0, 0x10000, 0);
  /* add global subclassing atom (used by 'tooltip' and 'updown') */
  COMCTL32_aSubclass = (LPSTR)(DWORD)GlobalAddAtomA ("CC32SubclassInfo");
  /* This will be wrong for any other process attching in this address-space! */
  COMCTL32_hModule = (HMODULE)hinstDLL;

  /* register controls */
  PROGRESS_Register();
  COMBOEX_Register();
  ANIMATE_Register();
  DATETIME_Register();
  FLATSB_Register();
  HOTKEY_Register();
//  IPADDRESS_Register();
  MONTHCAL_Register();
  NATIVEFONT_Register();
  PAGER_Register();
  TAB_Register();
  STATUS_Register();
  HEADER_Register();
  UPDOWN_Register();
  REBAR_Register();
  TRACKBAR_Register();
  TOOLTIPS_Register();
  TOOLBAR_Register();
  TREEVIEW_Register();
} 

void UnregisterCOMCTL32WindowClasses(void)
{
  /* unregister controls */
  PROGRESS_Unregister();
  COMBOEX_Unregister();
  ANIMATE_Unregister();
  DATETIME_Unregister();
  FLATSB_Unregister();
  HOTKEY_Unregister();
//  IPADDRESS_Unregister();
  MONTHCAL_Unregister();
  NATIVEFONT_Unregister();
  PAGER_Unregister();
  TAB_Unregister();
  STATUS_Unregister();
  HEADER_Unregister();
  UPDOWN_Unregister();
  REBAR_Unregister();
  TRACKBAR_Unregister();
  TOOLTIPS_Unregister();
  TOOLBAR_Unregister();
  TREEVIEW_Unregister();

  /* destroy private heap */
  HeapDestroy(COMCTL32_hHeap);
  COMCTL32_hHeap = (HANDLE)NULL;
}

/***********************************************************************
 * InitCommonControls [COMCTL32.17]
 *
 * Registers the common controls.
 *
 * PARAMS
 *     No parameters.
 *
 * RETURNS
 *     No return values.
 *
 * NOTES
 *     This function is just a dummy.
 *     The Win95 controls are registered at the DLL's initialization.
 *     To register other controls InitCommonControlsEx() must be used.
 */

VOID WINAPI InitCommonControls(VOID)
{
}
