/* $Id: tapi32.cpp,v 1.5 2001-09-09 13:33:38 sandervl Exp $ */

/*
 * TAPI2032 stubs
 *
 * Copyright 1998 Felix Maschek
 * Copyright 2001 Sander van Leeuwen
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <odinwrap.h>
#include <memory.h>
//#include <comtype.h>
#include <misc.h>
#include <tapi.h>
#include <unicode.h>

ODINDEBUGCHANNEL(TAPI32)

#define TAPI_MAGIC	0x12345678

typedef struct {
   DWORD        magic;
   HINSTANCE    hInstance;
   LINECALLBACK lpfnCallback;
} TAPIINFO, *LPTAPIINFO;

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineInitialize,
              LPHLINEAPP, lphLineApp,
              HINSTANCE, hInstance,
              LINECALLBACK, lpfnCallback,
              LPCSTR, lpszAppName,
              LPDWORD, lpdwNumDevs)
{
  LPTAPIINFO tapiinfo;

  if(lphLineApp)  *lphLineApp = 0;
  if(lpdwNumDevs) *lpdwNumDevs = 0;

  if(lpszAppName && *lpszAppName) {
      dprintf(("lineInitialize %s", lpszAppName));
  }
  if(!lphLineApp || !lpdwNumDevs) {
      return LINEERR_INVALPARAM;
  }

  tapiinfo = (LPTAPIINFO)HeapAlloc(GetProcessHeap(), 0, sizeof(TAPIINFO));
  if(tapiinfo == NULL) {
      return LINEERR_NOMEM;
  }
  tapiinfo->magic        = TAPI_MAGIC;
  tapiinfo->lpfnCallback = lpfnCallback;
  tapiinfo->hInstance    = hInstance;
  *lphLineApp  = (HLINEAPP)tapiinfo;
  *lpdwNumDevs = 1;
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineShutdown,
              HLINEAPP, hLineApp)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  HeapFree(GetProcessHeap(), 0, tapiinfo);
  return LINEERR_OPERATIONFAILED;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineNegotiateAPIVersion,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPILowVersion,
              DWORD, dwAPIHighVersion,
              LPDWORD, lpdwAPIVersion,
              LPLINEEXTENSIONID, lpExtensionID)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  if(dwDeviceID > 0) {
      return LINEERR_NODEVICE;
  }
  if(!lpdwAPIVersion) {
      return LINEERR_INVALPARAM;
  }
  *lpdwAPIVersion = dwAPIHighVersion;
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineNegotiateExtVersion,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtLowVersion,
              DWORD, dwExtHighVersion,
              LPDWORD, lpdwExtVersion)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  if(dwDeviceID > 0) {
      return LINEERR_NODEVICE;
  }
  if(!lpdwExtVersion) {
      return LINEERR_INVALPARAM;
  }
  *lpdwExtVersion = dwExtHighVersion;
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineGetDevCaps,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              LPLINEDEVCAPS, lpLineDevCaps)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;
  DWORD totalsize;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  if(dwDeviceID > 0 && dwDeviceID != -1) {
      return LINEERR_NODEVICE;
  }
  if(!lpLineDevCaps) {
      return LINEERR_INVALPARAM;
  }
  totalsize = lpLineDevCaps->dwTotalSize;
  if(lpLineDevCaps->dwTotalSize < sizeof(LINEDEVCAPS)) {
      lpLineDevCaps->dwNeededSize = sizeof(LINEDEVCAPS);
      return 0;
  }
  memset(lpLineDevCaps, 0, totalsize);
  lpLineDevCaps->dwTotalSize   = totalsize;
  lpLineDevCaps->dwNeededSize  = sizeof(LINEDEVCAPS);
  lpLineDevCaps->dwBearerModes = LINEBEARERMODE_VOICE;
  lpLineDevCaps->dwMediaModes  = LINEMEDIAMODE_DATAMODEM;
  lpLineDevCaps->dwLineFeatures= LINEFEATURE_MAKECALL;

  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetTranslateCaps,
              HLINEAPP, hLineApp,
              DWORD, dwAPIVersion,
              LPLINETRANSLATECAPS, lpTranslateCaps)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;
  LPLINELOCATIONENTRY lpLocationEntry;
  DWORD totalsize;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  if(!lpTranslateCaps) {
      return LINEERR_INVALPARAM;
  }
  totalsize = lpTranslateCaps->dwTotalSize;
  if(lpTranslateCaps->dwTotalSize < sizeof(LINETRANSLATECAPS) /* + sizeof(LINELOCATIONENTRY) */) {
      lpTranslateCaps->dwNeededSize = sizeof(LINETRANSLATECAPS); //+ sizeof(LINELOCATIONENTRY);
      return 0;
  }
  memset(lpTranslateCaps, 0, totalsize);
  lpTranslateCaps->dwTotalSize   = totalsize;
  lpTranslateCaps->dwNeededSize  = sizeof(LINETRANSLATECAPS); // + sizeof(LINELOCATIONENTRY);
//  lpTranslateCaps->dwLocationListOffset = sizeof(LINETRANSLATECAPS);
//  lpLocationEntry = (LPLINELOCATIONENTRY)
//        (((LPBYTE) lpTranslateCaps) + lpTranslateCaps->dwLocationListOffset);
//  lpLocationEntry = 
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetCountry,
              DWORD, dwCountryID,
              DWORD, dwAPIVersion,
              LPLINECOUNTRYLIST, lpLineCountryList)
{
  DWORD totalsize;

  if(!lpLineCountryList) {
      return LINEERR_INVALPARAM;
  }
  totalsize = lpLineCountryList->dwTotalSize;
  if(lpLineCountryList->dwTotalSize < sizeof(LINECOUNTRYLIST)) {
      lpLineCountryList->dwNeededSize = sizeof(LINECOUNTRYLIST);
      return 0;
  }
  memset(lpLineCountryList, 0, totalsize);
  lpLineCountryList->dwTotalSize   = totalsize;
  lpLineCountryList->dwNeededSize  = sizeof(LINECOUNTRYLIST);
  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION7(DWORD, lineTranslateAddress,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              LPCSTR, lpszAddressIn,
              DWORD, dwCard,
              DWORD, dwTranslateOptions,
              LPLINETRANSLATEOUTPUT, lpTranslateOutput)
{
  LPTAPIINFO tapiinfo = (LPTAPIINFO)hLineApp;
  DWORD totalsize;

  if(hLineApp == NULL || tapiinfo->magic != TAPI_MAGIC) {
      return LINEERR_INVALAPPHANDLE;
  }
  if(dwDeviceID > 0 && dwDeviceID != -1) {
      return LINEERR_NODEVICE;
  }
  if(!lpszAddressIn || !lpTranslateOutput) {
      return LINEERR_INVALPARAM;
  }
  dprintf(("lineTranslateAddress %s", lpszAddressIn));
  totalsize = lpTranslateOutput->dwTotalSize;
  if(lpTranslateOutput->dwTotalSize < sizeof(LINETRANSLATEOUTPUT)) {
      lpTranslateOutput->dwNeededSize = sizeof(LINETRANSLATEOUTPUT);
      return 0;
  }
  memset(lpTranslateOutput, 0, totalsize);
  lpTranslateOutput->dwTotalSize   = totalsize;

  return 0;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, tapiRequestMakeCall,
              LPCSTR, lpszDestAddress,
              LPCSTR, lpszAppName,
              LPCSTR, lpszCalledParty,
              LPCSTR, lpszComment)
{
  return LINEERR_OPERATIONFAILED;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION10(DWORD, tapiRequestMediaCall,
              HWND, hWnd,
              WPARAM, wRequestID,
              LPCSTR, lpszDeviceClass,
              LPCSTR, lpDeviceID,
              DWORD, dwSize,
              DWORD, dwSecure,
              LPCSTR, lpszDestAddress,
              LPCSTR, lpszAppName,
              LPCSTR, lpszCalledParty,
              LPCSTR, lpszComment)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, tapiRequestDrop,
              HWND, hWnd,
              WPARAM, wRequestID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineRegisterRequestRecipient,
              HLINEAPP, hLineApp,
              DWORD, dwRegistrationInstance,
              DWORD, dwRequestMode,
              DWORD, bEnable)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, tapiGetLocationInfo,
              LPSTR, lpszCountryCode,
              LPSTR, lpszCityCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineSetCurrentLocation,
              HLINEAPP, hLineApp,
              DWORD, dwLocation)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineSetTollList,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              LPCSTR, lpszAddressIn,
              DWORD, dwTollListOption)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineAccept,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineAddToConference,
              HCALL, hConfCall,
              HCALL, hConsultCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineAnswer,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineBlindTransfer,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineClose,
              HLINE, hLine)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineCompleteCall,
              HCALL, hCall,
              LPDWORD, lpdwCompletionID,
              DWORD, dwCompletionMode,
              DWORD, dwMessageID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineCompleteTransfer,
              HCALL, hCall,
              HCALL, hConsultCall,
              LPHCALL, lphConfCall,
              DWORD, dwTransferMode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineConfigDialog,
              DWORD, dwDeviceID,
              HWND, hwndOwner,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineConfigDialogEdit,
              DWORD, dwDeviceID,
              HWND, hwndOwner,
              LPCSTR, lpszDeviceClass,
              LPVOID, lpDeviceConfigIn,
              DWORD,  dwSize,
              LPVARSTRING, lpDeviceConfigOut)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineDeallocateCall,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineDevSpecific,
              HLINE, hLine,
              DWORD, dwAddressID,
              HCALL, hCall,
              LPVOID, lpParams,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineDevSpecificFeature,
              HLINE, hLine,
              DWORD, dwFeature,
              LPVOID, lpParams,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineDial,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineDrop,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(DWORD, lineForward,
              HLINE, hLine,
              DWORD, bAllAddresses,
              DWORD, dwAddressID,
              LPLINEFORWARDLIST,lpForwardList,
              DWORD,   dwNumRingsNoAnswer,
              LPHCALL, lphConsultCall,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(DWORD, lineGatherDigits,
              HCALL, hCall,
              DWORD, dwDigitModes,
              LPSTR, lpsDigits,
              DWORD, dwNumDigits,
              LPCSTR, lpszTerminationDigits,
              DWORD, dwFirstDigitTimeout,
              DWORD, dwInterDigitTimeout)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineGenerateDigits,
              HCALL, hCall,
              DWORD, dwDigitMode,
              LPCSTR, lpszDigits,
              DWORD, dwDuration)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineGenerateTone,
              HCALL, hCall,
              DWORD, dwToneMode,
              DWORD, dwDuration,
              DWORD, dwNumTones,
              LPLINEGENERATETONE, lpTones)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineGetAddressCaps,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAddressID,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              LPLINEADDRESSCAPS, lpAddressCaps)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineGetAddressID,
              HLINE, hLine,
              LPDWORD, lpdwAddressID,
              DWORD, dwAddressMode,
              LPCSTR, lpsAddress,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetAddressStatus,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPLINEADDRESSSTATUS, lpAddressStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineGetCallInfo,
              HCALL, hCall,
              LPLINECALLINFO, lpCallInfo)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineGetCallStatus,
              HCALL, hCall,
              LPLINECALLSTATUS, lpCallStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineGetConfRelatedCalls,
              HCALL, hCall,
              LPLINECALLLIST, lpCallList)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetDevConfig,
              DWORD, dwDeviceID,
              LPVARSTRING, lpDeviceConfig,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineGetNewCalls,
              HLINE, hLine,
              DWORD, dwAddressID,
              DWORD, dwSelect,
              LPLINECALLLIST, lpCallList)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetIcon,
              DWORD, dwDeviceID,
              LPCSTR, lpszDeviceClass,
              LPHICON, lphIcon)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineGetID,
              HLINE, hLine,
              DWORD, dwAddressID,
              HCALL, hCall,
              DWORD, dwSelect,
              LPVARSTRING, lpDeviceID,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineGetLineDevStatus,
              HLINE, hLine,
              LPLINEDEVSTATUS, lpLineDevStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetNumRings,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPDWORD, lpdwNumRings)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetRequest,
              HLINEAPP, hLineApp,
              DWORD, dwRequestMode,
              LPVOID, lpRequestBuffer)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineGetStatusMessages,
              HLINE, hLine,
              LPDWORD, lpdwLineStates,
              LPDWORD, lpdwAddressStates)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineHandoff,
              HCALL, hCall,
              LPCSTR, lpszFileName,
              DWORD, dwMediaMode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineHold,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineMakeCall,
              HLINE, hLine,
              LPHCALL, lphCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineMonitorDigits,
              HCALL, hCall,
              DWORD, dwDigitModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineMonitorMedia,
              HCALL, hCall,
              DWORD, dwMediaModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineMonitorTones,
              HCALL, hCall,
              LPLINEMONITORTONE, lpToneList,
              DWORD, dwNumEntries)
{
  return LINEERR_OPERATIONFAILED;
}
//******************************************************************************
//******************************************************************************
ODINFUNCTION9(DWORD, lineOpen,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              LPHLINE, lphLine,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              DWORD, dwCallbackInstance,
              DWORD, dwPrivileges,
              DWORD, dwMediaModes,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, linePark,
              HCALL, hCall,
              DWORD, dwParkMode,
              LPCSTR, lpszDirAddress,
              LPVARSTRING, lpNonDirAddress)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, linePickup,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPHCALL, lphCall,
              LPCSTR, lpszDestAddress,
              LPCSTR, lpszGroupID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, linePrepareAddToConference,
              HCALL, hConfCall,
              LPHCALL, lphConsultCall,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineRedirect,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineRemoveFromConference,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineSecureCall,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineSendUserUserInfo,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineSetAppSpecific,
              HCALL, hCall,
              DWORD, dwAppSpecific)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineSetCallParams,
              HCALL, hCall,
              DWORD, dwBearerMode,
              DWORD, dwMinRate,
              DWORD, dwMaxRate,
              LPLINEDIALPARAMS, lpDialParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineSetCallPrivilege,
              HCALL, hCall,
              DWORD, dwCallPrivilege)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineSetDevConfig,
              DWORD, dwDeviceID,
              LPVOID, lpDeviceConfig,
              DWORD,  dwSize,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION12(DWORD, lineSetMediaControl,
               HLINE, hLine,
               DWORD, dwAddressID,
               HCALL, hCall,
               DWORD, dwSelect,
               LPLINEMEDIACONTROLDIGIT, lpDigitList,
               DWORD, dwDigitNumEntries,
               LPLINEMEDIACONTROLMEDIA, lpMediaList,
               DWORD, dwMediaNumEntries,
               LPLINEMEDIACONTROLTONE, lpToneList,
               DWORD, dwToneNumEntries,
               LPLINEMEDIACONTROLCALLSTATE, lpCallStateList,
               DWORD, dwCallStateNumEntries)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineSetMediaMode,
              HCALL, hCall,
              DWORD, dwMediaModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineSetNumRings,
              HLINE, hLine,
              DWORD, dwAddressID,
              DWORD, dwNumRings)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineSetStatusMessages,
              HLINE, hLine,
              DWORD, dwLineStates,
              DWORD, dwAddressStates)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(DWORD, lineSetTerminal,
              HLINE, hLine,
              DWORD, dwAddressID,
              HCALL, hCall,
              DWORD, dwSelect,
              DWORD, dwTerminalModes,
              DWORD, dwTerminalID,
              DWORD, bEnable)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineSetupConference,
              HCALL, hCall,
              HLINE, hLine,
              LPHCALL, lphConfCall,
              LPHCALL, lphConsultCall,
              DWORD, dwNumParties,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineSetupTransfer,
              HCALL, hCall,
              LPHCALL, lphConsultCall,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineSwapHold,
              HCALL, hActiveCall,
              HCALL, hHeldCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineUncompleteCall,
              HLINE, hLine,
              DWORD, dwCompletionID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineUnhold,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, lineUnpark,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPHCALL, lphCall,
              LPCSTR, lpszDestAddress)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, lineReleaseUserUserInfo,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, phoneClose,
              HPHONE, hPhone)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneConfigDialog,
              DWORD, dwDeviceID,
              HWND, hwndOwner,
              LPCSTR, lpszDeviceClass)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneDevSpecific,
              HPHONE, hPhone,
              LPVOID, lpParams,
              DWORD, dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetButtonInfo,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPPHONEBUTTONINFO, lpButtonInfo)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, phoneGetData,
              HPHONE, hPhone,
              DWORD, dwDataID,
              LPVOID, lpData,
              DWORD, dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, phoneGetDevCaps,
              HPHONEAPP, hPhoneApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              LPPHONECAPS, lpPhoneCaps)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, phoneGetDisplay,
              HPHONE, hPhone,
              LPVARSTRING, lpDisplay)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetGain,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              LPDWORD, lpdwGain)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, phoneGetHookSwitch,
              HPHONE, hPhone,
              LPDWORD, lpdwHookSwitchDevs)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetIcon,
              DWORD, dwDeviceID,
              LPCSTR, lpszDeviceClass,
              LPHICON, lphIcon)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetID,
              HPHONE, hPhone,
              LPVARSTRING, lpDeviceID,
              LPCSTR, lpszDeviceClass)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetLamp,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPDWORD, lpdwLampMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetRing,
              HPHONE, hPhone,
              LPDWORD, lpdwRingMode,
              LPDWORD, lpdwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, phoneGetStatus,
              HPHONE, hPhone,
              LPPHONESTATUS, lpPhoneStatus)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, phoneGetStatusMessages,
              HPHONE, hPhone,
              LPDWORD, lpdwPhoneStates,
              LPDWORD, lpdwButtonModes,
              LPDWORD, lpdwButtonStates)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneGetVolume,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              LPDWORD, lpdwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, phoneInitialize,
              LPHPHONEAPP, lphPhoneApp,
              HINSTANCE, hInstance,
              PHONECALLBACK, lpfnCallback,
              LPCSTR, lpszAppName,
              LPDWORD, lpdwNumDevs)
{
  return PHONEERR_NODEVICE;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, phoneNegotiateAPIVersion,
              HPHONEAPP, hPhoneApp,
              DWORD, dwDeviceID,
              DWORD, dwAPILowVersion,
              DWORD, dwAPIHighVersion,
              LPDWORD, lpdwAPIVersion,
              LPPHONEEXTENSIONID, lpExtensionID)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, phoneNegotiateExtVersion,
              HPHONEAPP, hPhoneApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtLowVersion,
              DWORD, dwExtHighVersion,
              LPDWORD, lpdwExtVersion)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(DWORD, phoneOpen,
              HPHONEAPP, hPhoneApp,
              DWORD, dwDeviceID,
              LPHPHONE, lphPhone,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              DWORD, dwCallbackInstance,
              DWORD, dwPrivilege)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetButtonInfo,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPPHONEBUTTONINFO, lpButtonInfo)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, phoneSetData,
              HPHONE, hPhone,
              DWORD, dwDataID,
              LPVOID, lpData,
              DWORD,  dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, phoneSetDisplay,
              HPHONE, hPhone,
              DWORD, dwRow,
              DWORD, dwColumn,
              LPCSTR, lpsDisplay,
              DWORD, dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetGain,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              DWORD, dwGain)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetHookSwitch,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDevs,
              DWORD, dwHookSwitchMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetLamp,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              DWORD, dwLampMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetRing,
              HPHONE, hPhone,
              DWORD, dwRingMode,
              DWORD, dwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(DWORD, phoneSetStatusMessages,
              HPHONE, hPhone,
              DWORD, dwPhoneStates,
              DWORD, dwButtonModes,
              DWORD, dwButtonStates)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, phoneSetVolume,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              DWORD, dwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(DWORD, phoneShutdown,
              HPHONEAPP, hPhoneApp)
{
  return PHONEERR_OPERATIONFAILED;
}


//******************************************************************************
//******************************************************************************
ODINFUNCTION5(DWORD, lineTranslateDialog,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              HWND, hwndOwner,
              LPCSTR, lpszAddressIn)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineGetAppPriority,
              LPCSTR, lpszAppFilename,
              DWORD, dwMediaMode,
              LPLINEEXTENSIONID, lpExtensionID,
              DWORD, dwRequestMode,
              LPVARSTRING, lpExtensionName,
              LPDWORD, lpdwPriority)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(DWORD, lineSetAppPriority,
              LPCSTR, lpszAppFilename,
              DWORD, dwMediaMode,
              LPLINEEXTENSIONID, lpExtensionID,
              DWORD, dwRequestMode,
              LPCSTR, lpszExtensionName,
              DWORD, dwPriority)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(DWORD, lineAddProvider,
              LPCSTR, lpszProviderFilename,
              HWND, hwndOwner,
              LPDWORD, lpdwPermanentProviderID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineConfigProvider,
              HWND, hwndOwner,
              DWORD, dwPermanentProviderID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineRemoveProvider,
              DWORD, dwPermanentProviderID,
              HWND, hwndOwner)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(DWORD, lineGetProviderList,
              DWORD, dwAPIVersion,
              LPLINEPROVIDERLIST, lpProviderList)
{
  return LINEERR_OPERATIONFAILED;
}


