/* $Id: comdlg32.cpp,v 1.25 2000-03-24 18:22:55 achimha Exp $ */

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
#include <win32wnd.h>

ODINDEBUGCHANNEL(COMDLG32-COMDLG32)

// this controls whether we want to use the Open32 OS/2 file dialogs
// or our own Win95 like file dialogs
#define useWinFileDlg 1

#if 0
#define COMDLG32_CHECKHOOK(a,b,c)           \
  if(a->Flags & b)                          \
  {                                         \
    wndproc = CreateWindowProc((WNDPROC)a->lpfnHook);\
    if(wndproc == NULL)                     \
      return(FALSE);                        \
                                            \
    a->lpfnHook = (c)Win32WindowProc::GetOS2Callback();\
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);

#define COMDLG32_CHECKHOOK2(a,b,c,d)        \
  if(a->Flags & b)                          \
  {                                         \
    wndproc = CreateWindowProc((WNDPROC)a->d);\
    if(wndproc == NULL)                     \
      return(FALSE);                        \
                                            \
    a->d = (c)Win32WindowProc::GetOS2Callback();\
  } \
  a->hwndOwner = Win32ToOS2Handle(a->hwndOwner);
#else
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
#endif


/*****************************************************************************
 * Name      : iFileDlg_ScanFilterToken
 * Purpose   : scan for valid / invalid filter tokens and
 *             advance array pointer
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    :
 *****************************************************************************/

// scan filter tokens for validity
static BOOL iFileDlg_ScanFilterToken( LPSTR *plpstrPair )
{
  LPSTR lpstrTemp;
  LPSTR lpstrNext;
  BOOL  fOK = TRUE;

  lpstrTemp = *plpstrPair;
  if (lpstrTemp[0] != 0)
  {
    // get filter description
    lpstrTemp = lpstrTemp + strlen(lpstrTemp);
    lpstrNext = lpstrTemp + 1;
    if (lpstrNext[0] == 0)
      fOK = FALSE;
    else
    {
      // get filter mask
      lpstrTemp = lpstrNext;
      lpstrTemp = lpstrTemp + strlen(lpstrTemp);
      lpstrTemp++;
    }

    if (fOK)
      *plpstrPair = lpstrTemp;
  }
  return fOK;
}


/*****************************************************************************
 * Name      : iFileDlg_CleanFilterArray
 * Purpose   : remove erroneous array entries at the end to prevent
 *             Open32 complain about them but open the file dialog
 *             instead.
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    :
 *
 * Author    :
 *****************************************************************************/

static DWORD iFileDlg_CleanFilterArray( LPSTR lpstrFilters)
{
   DWORD  dwCount = 0;
   LPSTR  lpstrTemp;

   if (lpstrFilters && *lpstrFilters)
   {
     lpstrTemp = lpstrFilters;
     while (lpstrTemp[0] != 0)
     {
        // if an invalid filter token is found, such as found
        // in NT4's Regedit e. g., return number of proper
        // filter tokens.
        // Here however, as were calling Open32, we need
        // to correct the filter array.
        if (iFileDlg_ScanFilterToken(&lpstrTemp) == FALSE)
        {
          //@@@PH two alternative methods:
          // - copy array to new, corrected array while scanning tokens
          // - just overwrite bogus characters at the end of the array
          *lpstrTemp++ = 0; // try string termination
          *lpstrTemp   = 0;
          return dwCount;
        }

        dwCount++;
     }
   }
   return dwCount;
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

ODINFUNCTION1(BOOL, GetSaveFileNameA,
              LPOPENFILENAMEA, lpofn)
{
  if(useWinFileDlg || (lpofn->Flags & (OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE))) {
   return GetFileDialog95A(lpofn, SAVE_DIALOG);
  }

  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  //Note: fix IBM's proper error checking for NT's lazy check
  if (lpofn->lpstrFilter != NULL)
    iFileDlg_CleanFilterArray((LPSTR)lpofn->lpstrFilter);

  return(O32_GetSaveFileName(lpofn));
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
  OPENFILENAMEA   ofn;
  char*           szFile;
  char*           szFileTitle;
  char*           szCustFilter;
  BOOL            bResult;

  if(useWinFileDlg || (lpofn->Flags & (OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE))) {
   return GetFileDialog95W(lpofn, SAVE_DIALOG);
  }

  memcpy(&ofn,          // make binary copy first to save all the fields
         lpofn,
         sizeof(ofn));

    // convert to ASCII string
  if ((lpofn->Flags && OFN_ENABLETEMPLATE) &&
      (lpofn->lpTemplateName != NULL))
    ofn.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpofn->lpTemplateName);
  else
    ofn.lpTemplateName = NULL;

  if (lpofn->lpstrFilter != NULL)
    ofn.lpstrFilter = UnicodeToAsciiString((WCHAR*)lpofn->lpstrFilter);

  if (lpofn->lpstrInitialDir != NULL)
    ofn.lpstrInitialDir = UnicodeToAsciiString((WCHAR*)lpofn->lpstrInitialDir);

  if (lpofn->lpstrTitle != NULL)
    ofn.lpstrTitle = UnicodeToAsciiString((WCHAR*)lpofn->lpstrTitle);

  if (lpofn->lpstrDefExt != NULL)
    ofn.lpstrDefExt = UnicodeToAsciiString((WCHAR*)lpofn->lpstrDefExt);

  szFile             = (char*)malloc(lpofn->nMaxFile);
  szFile[0]          = 0;

  if (*lpofn->lpstrFile != 0)
    UnicodeToAscii(lpofn->lpstrFile,
                   szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    szFileTitle        = (char*)malloc(lpofn->nMaxFileTitle);
    szFileTitle[0]     = 0;

    if (*lpofn->lpstrFileTitle != 0)
      UnicodeToAscii(lpofn->lpstrFileTitle,
                     szFileTitle);
  }
  else
    szFileTitle = NULL;

  if (lpofn->lpstrCustomFilter != NULL)
  {
    szCustFilter       = (char*)malloc(lpofn->nMaxCustFilter);
    szCustFilter[0]    = 0;


    if (*lpofn->lpstrCustomFilter != 0)
       UnicodeToAscii(lpofn->lpstrCustomFilter,
                      szCustFilter);
  }
  else
    szCustFilter = NULL;

  ofn.lpstrFile         = szFile;
  ofn.lpstrFileTitle    = szFileTitle;
  ofn.lpstrCustomFilter = szCustFilter;

  // call ascii variant of function
  // @@@PH switch to ODIN_GetSaveFileNameA later
  bResult = GetSaveFileNameA(&ofn);

  if (ofn.lpTemplateName    != NULL) FreeAsciiString((char*)ofn.lpTemplateName);
  if (ofn.lpstrFilter       != NULL) FreeAsciiString((char*)ofn.lpstrFilter);
  if (ofn.lpstrInitialDir   != NULL) FreeAsciiString((char*)ofn.lpstrInitialDir);
  if (ofn.lpstrTitle        != NULL) FreeAsciiString((char*)ofn.lpstrTitle);
  if (ofn.lpstrDefExt       != NULL) FreeAsciiString((char*)ofn.lpstrDefExt);

  // transform back the result
  AsciiToUnicode(ofn.lpstrFile,
                 lpofn->lpstrFile);
  free(szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    AsciiToUnicode(ofn.lpstrFileTitle,
                   lpofn->lpstrFileTitle);
    free(szFileTitle);
  }

  if (lpofn->lpstrCustomFilter != NULL)
  {
    AsciiToUnicode(ofn.lpstrCustomFilter,
                   lpofn->lpstrCustomFilter);
    free(szCustFilter);
  }

  // copy over some altered flags
  lpofn->nFilterIndex   = ofn.nFilterIndex;
  lpofn->Flags          = ofn.Flags;
  lpofn->nFileOffset    = ofn.nFileOffset;
  lpofn->nFileExtension = ofn.nFileExtension;

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

ODINFUNCTION1(BOOL, GetOpenFileNameA,
              LPOPENFILENAMEA, lpofn)
{
  BOOL rc;

  CheckCurFS();
  if(useWinFileDlg || (lpofn->Flags & (OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE)))
  {
    return GetFileDialog95A(lpofn, OPEN_DIALOG);
  }
  COMDLG32_CHECKHOOK(lpofn, OFN_ENABLEHOOK, WNDPROC)

  //Note: fix IBM's proper error checking for NT's lazy check
  if (lpofn->lpstrFilter != NULL)
    iFileDlg_CleanFilterArray((LPSTR)lpofn->lpstrFilter);

  rc = O32_GetOpenFileName(lpofn);
  CheckCurFS();
  return rc;
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
  OPENFILENAMEA   ofn;
  char*           szFile;
  char*           szFileTitle;
  char*           szCustFilter;
  BOOL            bResult;

  if(useWinFileDlg || (lpofn->Flags & (OFN_ENABLETEMPLATE|OFN_ENABLETEMPLATEHANDLE))) {
   return GetFileDialog95W(lpofn, OPEN_DIALOG);
  }

  memcpy(&ofn,          // make binary copy first to save all the fields
         lpofn,
         sizeof(ofn));

    // convert to ASCII string
  if ((lpofn->Flags && OFN_ENABLETEMPLATE) &&
      (lpofn->lpTemplateName != NULL))
    ofn.lpTemplateName = UnicodeToAsciiString((WCHAR*)lpofn->lpTemplateName);
  else
    ofn.lpTemplateName = NULL;

  if (lpofn->lpstrFilter != NULL)
    ofn.lpstrFilter = UnicodeToAsciiString((WCHAR*)lpofn->lpstrFilter);

  if (lpofn->lpstrInitialDir != NULL)
    ofn.lpstrInitialDir = UnicodeToAsciiString((WCHAR*)lpofn->lpstrInitialDir);

  if (lpofn->lpstrTitle != NULL)
    ofn.lpstrTitle = UnicodeToAsciiString((WCHAR*)lpofn->lpstrTitle);

  if (lpofn->lpstrDefExt != NULL)
    ofn.lpstrDefExt = UnicodeToAsciiString((WCHAR*)lpofn->lpstrDefExt);

  szFile             = (char*)malloc(lpofn->nMaxFile);
  szFile[0]          = 0;

  if (*lpofn->lpstrFile != 0)
    UnicodeToAscii(lpofn->lpstrFile,
                   szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    szFileTitle        = (char*)malloc(lpofn->nMaxFileTitle);
    szFileTitle[0]     = 0;

    if (*lpofn->lpstrFileTitle != 0)
      UnicodeToAscii(lpofn->lpstrFileTitle,
                     szFileTitle);
  }
  else
    szFileTitle = NULL;

  if (lpofn->lpstrCustomFilter != NULL)
  {
    szCustFilter       = (char*)malloc(lpofn->nMaxCustFilter);
    szCustFilter[0]    = 0;


    if (*lpofn->lpstrCustomFilter != 0)
       UnicodeToAscii(lpofn->lpstrCustomFilter,
                      szCustFilter);
  }
  else
    szCustFilter = NULL;

  ofn.lpstrFile         = szFile;
  ofn.lpstrFileTitle    = szFileTitle;
  ofn.lpstrCustomFilter = szCustFilter;

  // call ascii variant of function
  // @@@PH switch to ODIN_GetOpenFileNameA later
  bResult = GetOpenFileNameA(&ofn);

  if (ofn.lpTemplateName    != NULL) FreeAsciiString((char*)ofn.lpTemplateName);
  if (ofn.lpstrFilter       != NULL) FreeAsciiString((char*)ofn.lpstrFilter);
  if (ofn.lpstrInitialDir   != NULL) FreeAsciiString((char*)ofn.lpstrInitialDir);
  if (ofn.lpstrTitle        != NULL) FreeAsciiString((char*)ofn.lpstrTitle);
  if (ofn.lpstrDefExt       != NULL) FreeAsciiString((char*)ofn.lpstrDefExt);

  // transform back the result
  AsciiToUnicode(ofn.lpstrFile,
                 lpofn->lpstrFile);
  free(szFile);

  if (lpofn->lpstrFileTitle != NULL)
  {
    AsciiToUnicode(ofn.lpstrFileTitle,
                   lpofn->lpstrFileTitle);
    free(szFileTitle);
  }

  if (lpofn->lpstrCustomFilter != NULL)
  {
    AsciiToUnicode(ofn.lpstrCustomFilter,
                   lpofn->lpstrCustomFilter);
    free(szCustFilter);
  }

  // copy over some altered flags
  lpofn->nFilterIndex   = ofn.nFilterIndex;
  lpofn->Flags          = ofn.Flags;
  lpofn->nFileOffset    = ofn.nFileOffset;
  lpofn->nFileExtension = ofn.nFileExtension;

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

