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

HANDLE COMCTL32_hHeap = (HANDLE)NULL;


void RegisterCOMCTL32WindowClasses(void)
{
  /* create private heap */
  COMCTL32_hHeap = HeapCreate(0, 0x10000, 0);

  /* register progress control */
  PROGRESS_Register();
  /* register extended combobox control */
  COMBOEX_Register();
  /* register animation control */
  ANIMATE_Register();
  /* register date time control */
  DATETIME_Register();
  /* register the flat scrollbar control */
  FLATSB_Register();
  /* register hotkey control */
  HOTKEY_Register();
  /* register IP address control */
//  IPADDRESS_Register();
  /* register month calender control */
  MONTHCAL_Register();
  /* register native font control */
  NATIVEFONT_Register();
  /* register pager control */
  PAGER_Register();
  /* register tab control */
  TAB_Register();
} 

void UnregisterCOMCTL32WindowClasses(void)
{
  /* unregister progress control */
  PROGRESS_Unregister();
  /* unregister extended combobox control */
  COMBOEX_Unregister();
  /* unregister animation control */
  ANIMATE_Unregister();
  /* unregister date time control */
  DATETIME_Unregister();
  /* unregister flat scrollbar control */
  FLATSB_Unregister();
  /* unregister hotkey control */
  HOTKEY_Unregister();
  /* unregister IP address control */
//  IPADDRESS_Unregister();
  /* unregister month calendar control */
  MONTHCAL_Unregister();
  /* unregister native font control */
  NATIVEFONT_Unregister();
  /* unregister pager control */
  PAGER_Unregister();
  /* unregister tab control */
  TAB_Unregister();

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
