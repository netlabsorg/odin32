/* $Id: comdlg32.cpp,v 1.27 2000-12-30 13:43:57 sandervl Exp $ */

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

ODINFUNCTION1(BOOL, GetSaveFileNameA,
              LPOPENFILENAMEA, lpofn)
{
  return GetFileDialog95A(lpofn, SAVE_DIALOG);
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

ODINFUNCTION1(BOOL, GetSaveFileNameW,
              LPOPENFILENAMEW, lpofn)
{
  return GetFileDialog95W(lpofn, SAVE_DIALOG);
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

ODINFUNCTION1(BOOL, GetOpenFileNameA,
              LPOPENFILENAMEA, lpofn)
{
  return GetFileDialog95A(lpofn, OPEN_DIALOG);
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

ODINFUNCTION1(BOOL, GetOpenFileNameW,
              LPOPENFILENAMEW, lpofn)
{
  return GetFileDialog95W(lpofn, OPEN_DIALOG);
}

#if 0
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

ODINFUNCTION3(INT16, GetFileTitleA32,
              LPCSTR, lpFile,
              LPSTR, lpTitle,
              UINT, cbBuf)
{
  return O32_GetFileTitle(lpFile,
                          lpTitle,
                          cbBuf);
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

ODINFUNCTION1(BOOL, ChooseColorA,
              LPCHOOSECOLORA, lpcc)
{

  COMDLG32_CHECKHOOK(lpcc, CC_ENABLEHOOK, LPCCHOOKPROC)

  return O32_ChooseColor(lpcc);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : casting is a little hot, should work however
 *             assuming lpcc->lpTemplateName is READONLY pointer!
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *****************************************************************************/

ODINFUNCTION1(BOOL, ChooseColorW,
              LPCHOOSECOLORW, lpcc)
{
  BOOL            bResult;
  LPCWSTR         lpTemplateNameBackup = lpcc->lpTemplateName;

  // if no template is to convert, we can take this shortcut
  if (!(lpcc->Flags & CC_ENABLETEMPLATE))
    return O32_ChooseColor( (LPCHOOSECOLORA)lpcc );


  // convert to ASCII string
  if (lpcc->lpTemplateName != NULL)
    lpcc->lpTemplateName = (LPCWSTR)UnicodeToAsciiString((WCHAR*)lpcc->lpTemplateName);

  COMDLG32_CHECKHOOK(lpcc, CC_ENABLEHOOK, LPCCHOOKPROC)

  bResult = O32_ChooseColor((LPCHOOSECOLORA)lpcc);  // call ASCII version

  // free temporary ASCII string and restore UNICODE string
  if (lpcc->lpTemplateName != NULL)
  {
    FreeAsciiString((char*)lpcc->lpTemplateName);
    lpcc->lpTemplateName = lpTemplateNameBackup;
  }

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

ODINFUNCTION1(BOOL, ChooseFontA,
              LPCHOOSEFONTA, lpcf)
{

  COMDLG32_CHECKHOOK(lpcf, CF_ENABLEHOOK, WNDPROC)

  return O32_ChooseFont(lpcf);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : unknown yet, what is INPUT and what is OUTPUT string
 * Status    :
 *
 * Author    : Patrick Haller [Tue, 1998/02/10 01:55]
 *             Edgar Buerkle  [Mon, 1999/06/28 19:35]
 *****************************************************************************/

ODINFUNCTION1(BOOL, ChooseFontW,
              LPCHOOSEFONTW, lpcf)
{
  BOOL            bResult;
  CHOOSEFONTA     asciicf;
  LOGFONTA        asciilf;
  char            szAsciiStyle[64];

  // NOTE: LOGFONTW/A is NOT converted !
  dprintf(("COMDLG32: ChooseFontW not correctly implemented.\n"));

  if (!lpcf)
  {
    SetLastError(ERROR_INVALID_PARAMETER);

    return FALSE;
  }

  // convert to ASCII string
  memcpy(&asciicf,     // make binary copy of CHOOSEFONTW
         lpcf,         // to save the flags
         sizeof(asciicf));

  memcpy (&asciilf,
          lpcf->lpLogFont,
          sizeof(LOGFONTA));

  asciicf.lpLogFont = &asciilf; // update pointer

  // lpTemplatenName bug in open32 ?  This doesn't work.
  // TODO: CF_ENABLETEMPLATEHANDLE
  if (lpcf->Flags & CF_ENABLETEMPLATE)
    if((int)asciicf.lpTemplateName >> 16 != 0)
      asciicf.lpTemplateName = UnicodeToAsciiString((LPWSTR)lpcf->lpTemplateName);

  //CB: NT's clock.exe traps here!
  if (lpcf->lpszStyle)
  {
    UnicodeToAsciiN(lpcf->lpszStyle,
                    szAsciiStyle,
                    sizeof(szAsciiStyle));

    asciicf.lpszStyle = szAsciiStyle;
  }

  UnicodeToAsciiN(lpcf->lpLogFont->lfFaceName,
                  asciilf.lfFaceName,
                  LF_FACESIZE-1);

  // LPCFHOOKPROC != WNDPROC ?
  COMDLG32_CHECKHOOK((&asciicf), CF_ENABLEHOOK, WNDPROC)

  // switch strings
  bResult = O32_ChooseFont((LPCHOOSEFONTA)&asciicf);        // call ASCII version

  if (bResult)
  {
    // transfer BACK resulting strings !!!
    AsciiToUnicodeN(asciicf.lpLogFont->lfFaceName,
                    lpcf->lpLogFont->lfFaceName,
                    LF_FACESIZE-1);

    if (lpcf->lpszStyle) AsciiToUnicode(asciicf.lpszStyle,lpcf->lpszStyle);
  }

  if (lpcf->Flags & CF_ENABLETEMPLATE)
    if((int)asciicf.lpTemplateName >> 16 != 0)
      FreeAsciiString((char*)asciicf.lpTemplateName);

  // copy back fields
  lpcf->Flags = asciicf.Flags;

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

ODINFUNCTION0(DWORD, CommDlgExtendedError32)
{
  return O32_CommDlgExtendedError();
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

ODINFUNCTION1(HWND, FindTextA32,
              LPFINDREPLACEA, lpfr)
{

  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  return O32_FindText(lpfr);
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

ODINFUNCTION1(HWND, FindTextW32,
              LPFINDREPLACEW, lpfr)
{
  BOOL            bResult;
  FINDREPLACEA    fr;

  memcpy(&fr,          // make binary copy first to save all the fields
         lpfr,
         sizeof(fr));

    // convert to ASCII string
  if ((lpfr->Flags & FR_ENABLETEMPLATE) &&
      (lpfr->lpTemplateName != NULL))
    fr.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpfr->lpTemplateName);
  else
    fr.lpTemplateName = NULL;

  if (lpfr->lpstrFindWhat != NULL)
    fr.lpstrFindWhat = UnicodeToAsciiString((WCHAR*)lpfr->lpstrFindWhat);

  if (lpfr->lpstrReplaceWith != NULL)
    fr.lpstrReplaceWith = UnicodeToAsciiString((WCHAR*)lpfr->lpstrReplaceWith);


  COMDLG32_CHECKHOOK((&fr), FR_ENABLEHOOK, WNDPROC)

  bResult = O32_FindText(&fr);                         // call ASCII version

  // @@@PH: Note -- we might have to transfer BACK resulting strings !!!
  // free temporary ASCII string and restore UNICODE string
  if (fr.lpTemplateName != NULL)
    FreeAsciiString((char*)fr.lpTemplateName);

  // free temporary ASCII string and restore UNICODE string
  if (fr.lpstrFindWhat != NULL)
  {
    AsciiToUnicode(fr.lpstrFindWhat,
                   lpfr->lpstrFindWhat);

    FreeAsciiString((char*)fr.lpstrFindWhat);
  }

  if (fr.lpstrReplaceWith != NULL)
  {
    AsciiToUnicode(fr.lpstrReplaceWith,
                   lpfr->lpstrReplaceWith);

    FreeAsciiString((char*)fr.lpstrReplaceWith);
  }

  // copy back fields
  lpfr->Flags = fr.Flags;

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

ODINFUNCTION3(INT16, GetFileTitleW32,
              LPCWSTR, lpFile,
              LPWSTR, lpTitle,
              UINT, cbBuf)
{
  LPSTR lpFileBackup;
  char  szTitle[256];
  INT16 iResult;

  lpFileBackup = UnicodeToAsciiString((LPWSTR)lpFile);
  iResult      = O32_GetFileTitle(lpFileBackup,
                                  szTitle,
                                  cbBuf);

  FreeAsciiString(lpFileBackup);

  // transform result into Unicode
  AsciiToUnicode(szTitle,
                 lpTitle);

  return iResult;
}
#endif

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


#if 0
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

ODINFUNCTION1(HWND, ReplaceTextA32,
              LPFINDREPLACEA, lpfr)
{

  COMDLG32_CHECKHOOK(lpfr, FR_ENABLEHOOK, WNDPROC)

  return O32_ReplaceText(lpfr);
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

ODINFUNCTION1(HWND, ReplaceTextW32,
              LPFINDREPLACEW, lpfr)
{
  BOOL            bResult;
  FINDREPLACEA    fr;

  dprintf(("COMDLG32: ReplaceTextW(%08xh)\n",
           lpfr));

  memcpy(&fr,          // make binary copy first to save all the fields
         lpfr,
         sizeof(fr));

    // convert to ASCII string
  if ((lpfr->Flags & FR_ENABLETEMPLATE) &&
      (lpfr->lpTemplateName != NULL))
    fr.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpfr->lpTemplateName);
  else
    fr.lpTemplateName = NULL;

  if (lpfr->lpstrFindWhat != NULL)
    fr.lpstrFindWhat = UnicodeToAsciiString((WCHAR*)lpfr->lpstrFindWhat);

  if (lpfr->lpstrReplaceWith != NULL)
    fr.lpstrReplaceWith = UnicodeToAsciiString((WCHAR*)lpfr->lpstrReplaceWith);


  COMDLG32_CHECKHOOK((&fr), FR_ENABLEHOOK, WNDPROC)

  bResult = O32_ReplaceText(&fr);                         // call ASCII version

  // @@@PH: Note -- we might have to transfer BACK resulting strings !!!
  // free temporary ASCII string and restore UNICODE string
  if (fr.lpTemplateName != NULL)
    FreeAsciiString((char*)fr.lpTemplateName);

  // free temporary ASCII string and restore UNICODE string
  if (fr.lpstrFindWhat != NULL)
  {
    AsciiToUnicode(fr.lpstrFindWhat,
                   lpfr->lpstrFindWhat);

    FreeAsciiString((char*)fr.lpstrFindWhat);
  }

  if (fr.lpstrReplaceWith != NULL)
  {
    AsciiToUnicode(fr.lpstrReplaceWith,
                   lpfr->lpstrReplaceWith);

    FreeAsciiString((char*)fr.lpstrReplaceWith);
  }

  // copy back fields
  lpfr->Flags = fr.Flags;

  return bResult;
}
#endif

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

