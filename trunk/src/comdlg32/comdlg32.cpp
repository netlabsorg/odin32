/* $Id: comdlg32.cpp,v 1.32 2001-08-02 14:49:55 sandervl Exp $ */

/*
 * COMDLG32 implementation
 *
 * Copyright 1998 Sander van Leeuwen
 * Copyright 1999 Patrick Haller
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */


/****************************************************************************
 * Includes                                                                 *
 ****************************************************************************/

#include <os2win.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <misc.h>
#include <odinwrap.h>
#include <winuser32.h>
#include <unicode.h>
#include <wingdi32.h>
#include <codepage.h>

ODINDEBUGCHANNEL(COMDLG32-COMDLG32)


#define COMDLG32_CHECKHOOK(a,b,c)           \
  if(a->Flags & b)                          \
  {                                         \
    a->lpfnHook = 0; \
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);

#define COMDLG32_CHECKHOOK2(a,b,c,d)        \
  if(a->Flags & b)                          \
  {                                         \
    a->d = 0; \
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);

#ifdef USING_OPEN32

#define FLAG_TO_OPEN32	     0
#define FLAG_FROM_OPEN32     1

//******************************************************************************
//******************************************************************************
HGLOBAL GlobalCopy(HGLOBAL hDest, HGLOBAL hSource, BOOL fToOpen32)
{
 LPVOID src;
 LPVOID dest;
 ULONG  size;

      if(fToOpen32 == FLAG_TO_OPEN32) {
          src = GlobalLock(hSource);
          if(src) {
              size = GlobalSize(hSource);
              if(hDest == NULL) {
                  hDest = O32_GlobalAlloc(GHND, size);
              }
              dest  = O32_GlobalLock(hDest);
              memcpy(dest, src, size);
              O32_GlobalUnlock(hDest);
          }
          GlobalUnlock(hSource);
      }
      else {
          src = O32_GlobalLock(hSource);
          if(src) {
              size = O32_GlobalSize(hSource);
              if(hDest == NULL) {
                  hDest = GlobalAlloc(GHND, size);
              }
              dest  = GlobalLock(hDest);
              memcpy(dest, src, size);
              GlobalUnlock(hDest);
          }
          O32_GlobalUnlock(hSource);
      }
      return hDest;
}
#endif
#if 1
/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PrintDlgA,
              LPPRINTDLGA, lppd)
{
  BOOL      ret;
#ifdef USING_OPEN32
  HGLOBAL   hDevmode = 0, hDevNames = 0;
  HGLOBAL   hOrgDevmode = 0, hOrgDevNames = 0;
  DEVMODEA *devmode, *devmodeorg;
  DEVNAMES *devnames, *devnamesorg;
  ULONG     size;
#endif

  COMDLG32_CHECKHOOK2(lppd, PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
  COMDLG32_CHECKHOOK2(lppd, PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

#ifdef USING_OPEN32
  if(lppd->hDevMode) {
      hOrgDevmode = lppd->hDevMode;
      lppd->hDevMode = GlobalCopy(NULL, hOrgDevmode, FLAG_TO_OPEN32);
  }
  if(lppd->hDevNames) {
      hOrgDevNames = lppd->hDevNames;
      lppd->hDevNames = GlobalCopy(NULL, hOrgDevNames, FLAG_TO_OPEN32);
  }
#endif
  HWND hwndOwner = lppd->hwndOwner;
  if(lppd->hwndOwner) {
      lppd->hwndOwner = Win32ToOS2Handle(lppd->hwndOwner);
  }
  ret = O32_PrintDlg(lppd);
  lppd->hwndOwner = hwndOwner;
#ifdef USING_OPEN32
  if(ret == TRUE) {
      if(lppd->hDevMode) {
          hDevmode = lppd->hDevMode;
          lppd->hDevMode = GlobalCopy(hOrgDevmode, lppd->hDevMode, FLAG_FROM_OPEN32);
          O32_GlobalFree(hDevmode);
      }
      if(lppd->hDevNames) {
          hDevNames = lppd->hDevNames;
          lppd->hDevNames = GlobalCopy(hOrgDevNames, lppd->hDevNames, FLAG_FROM_OPEN32);
          O32_GlobalFree(hDevNames);
      }
  }
#endif
  if(lppd->hDC) {
      OSLibGpiSetCp(lppd->hDC, GetDisplayCodepage());
  }
  return ret;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PrintDlgW,
              LPPRINTDLGW, lppd)
{

  PRINTDLGA pd;
  BOOL      bResult;

  //SvL: TODO: hDevMode & hDevNames
  dprintf(("PrintDlgW: ERROR NOT COMPLETE (UNICODE TRANSLATION)!!"));

  memcpy(&pd,          // make binary copy first to save all the fields
         lppd,
         sizeof(pd));

  // convert to ASCII string
  if ((lppd->Flags & PD_ENABLEPRINTTEMPLATE) &&
      (lppd->lpPrintTemplateName != NULL))
    pd.lpPrintTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpPrintTemplateName);
  else
    pd.lpPrintTemplateName = NULL;

  if ((lppd->Flags & PD_ENABLESETUPTEMPLATE) &&
      (lppd->lpSetupTemplateName != NULL))
    pd.lpSetupTemplateName = UnicodeToAsciiString((WCHAR*)lppd->lpSetupTemplateName);
  else
    pd.lpSetupTemplateName = NULL;

  COMDLG32_CHECKHOOK2((&pd), PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
  COMDLG32_CHECKHOOK2((&pd), PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

  bResult = O32_PrintDlg(&pd); // call ASCII API

  if (pd.lpPrintTemplateName != NULL) FreeAsciiString((char*)pd.lpPrintTemplateName);
  if (pd.lpSetupTemplateName != NULL) FreeAsciiString((char*)pd.lpSetupTemplateName);

  // copy back result
  lppd->Flags     = pd.Flags;
  lppd->nFromPage = pd.nFromPage;
  lppd->nToPage   = pd.nToPage;
  lppd->nMinPage  = pd.nMinPage;
  lppd->nMaxPage  = pd.nMaxPage;
  lppd->nCopies   = pd.nCopies;
  //@@@PH: all pass-back fields ?

  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PageSetupDlgA,
              LPPAGESETUPDLGA, lppsd)
{

  dprintf(("COMDLG32: PageSetupDlgA not implemented.\n"));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, PageSetupDlgW,
              LPPAGESETUPDLGW, lppsd)
{

  dprintf(("COMDLG32: PageSetupDlgW(%08xh) not implemented.\n"));

  //COMDLG32_CHECKHOOK2(lppsd, PSD_ENABLESETUPHOOK, LPPAGESETUPHOOK, lpfnPageSetupHook)

  return(FALSE);
}

#endif
