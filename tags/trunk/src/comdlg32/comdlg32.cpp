/* $Id: comdlg32.cpp,v 1.28 2001-01-11 18:05:02 sandervl Exp $ */

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

  COMDLG32_CHECKHOOK2(lppd, PD_ENABLEPRINTHOOK, LPPRINTHOOKPROC,lpfnPrintHook)
  COMDLG32_CHECKHOOK2(lppd, PD_ENABLESETUPHOOK, LPSETUPHOOKPROC,lpfnSetupHook)

  return O32_PrintDlg(lppd);
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

