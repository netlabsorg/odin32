/* $Id: tapi32.cpp,v 1.1 1999-05-24 20:19:58 ktk Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 */
/*
 * TAPI2032 stubs
 *
 * Copyright 1998 Felix Maschek
 *
 */
#include <os2win.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <winerror.h>
#include <memory.h>
//#include <comtype.h>
#include "misc.h"
#include "tapi32.h"
#include "unicode.h"

//******************************************************************************
//******************************************************************************
LONG WIN32API tapiRequestMakeCall(
    LPCSTR lpszDestAddress,
    LPCSTR lpszAppName,
    LPCSTR lpszCalledParty,
    LPCSTR lpszComment)
{
#ifdef DEBUG
  WriteLog("TAPI32: tapiRequestMakeCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API tapiRequestMediaCall(
    HWND hWnd,
    WPARAM wRequestID,
    LPCSTR lpszDeviceClass,
    LPCSTR lpDeviceID,
    DWORD dwSize,
    DWORD dwSecure,
    LPCSTR lpszDestAddress,
    LPCSTR lpszAppName,
    LPCSTR lpszCalledParty,
    LPCSTR lpszComment)
{
#ifdef DEBUG
  WriteLog("TAPI32: tapiRequestMediaCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API tapiRequestDrop(
    HWND hWnd,
    WPARAM wRequestID)
{
#ifdef DEBUG
  WriteLog("TAPI32: tapiRequestDrop\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineRegisterRequestRecipient(
    HLINEAPP hLineApp,
    DWORD dwRegistrationInstance,
    DWORD dwRequestMode,
    DWORD bEnable)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineRegisterRequestRecipient\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API tapiGetLocationInfo(
    LPSTR lpszCountryCode,
    LPSTR lpszCityCode)
{
#ifdef DEBUG
  WriteLog("TAPI32: tapiGetLocationInfo\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetCurrentLocation(
    HLINEAPP hLineApp,
    DWORD dwLocation)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetCurrentLocation\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetTollList(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    LPCSTR lpszAddressIn,
    DWORD dwTollListOption)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetTollList\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineTranslateAddress(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    LPCSTR lpszAddressIn,
    DWORD dwCard,
    DWORD dwTranslateOptions,
    LPLINETRANSLATEOUTPUT lpTranslateOutput)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineTranslateAddress\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetTranslateCaps(
    HLINEAPP hLineApp,
    DWORD dwAPIVersion,
    LPLINETRANSLATECAPS lpTranslateCaps)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetTranslateCaps\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineAccept(
    HCALL hCall,
    LPCSTR lpsUserUserInfo,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineAccept\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineAddToConference(
    HCALL hConfCall,
    HCALL hConsultCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineAddToConference\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineAnswer(
    HCALL hCall,
    LPCSTR lpsUserUserInfo,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineAnswer\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineBlindTransfer(
    HCALL hCall,
    LPCSTR lpszDestAddress,
    DWORD dwCountryCode)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineBlindTransfer\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineClose(
    HLINE hLine)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineClose\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineCompleteCall(
    HCALL hCall,
    LPDWORD lpdwCompletionID,
    DWORD dwCompletionMode,
    DWORD dwMessageID)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineCompleteCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineCompleteTransfer(
    HCALL hCall,
    HCALL hConsultCall,
    LPHCALL lphConfCall,
    DWORD dwTransferMode)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineCompleteTransfer\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineConfigDialog(
    DWORD dwDeviceID,
    HWND  hwndOwner,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineConfigDialog\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineConfigDialogEdit(
    DWORD   dwDeviceID,
    HWND    hwndOwner,
    LPCSTR  lpszDeviceClass,
    LPVOID  const lpDeviceConfigIn,
    DWORD   dwSize,
    LPVARSTRING lpDeviceConfigOut)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineConfigDialogEdit\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineDeallocateCall(
    HCALL hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineDeallocateCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineDevSpecific(
    HLINE hLine,
    DWORD dwAddressID,
    HCALL hCall,
    LPVOID lpParams,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineDevSpecific\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineDevSpecificFeature(
    HLINE hLine,
    DWORD dwFeature,
    LPVOID lpParams,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineDevSpecificFeature\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineDial(
    HCALL hCall,
    LPCSTR lpszDestAddress,
    DWORD dwCountryCode)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineDial\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineDrop(
    HCALL hCall,
    LPCSTR lpsUserUserInfo,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineDrop\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineForward(
    HLINE hLine,
    DWORD bAllAddresses,
    DWORD dwAddressID,
    LPLINEFORWARDLIST const lpForwardList,
    DWORD dwNumRingsNoAnswer,
    LPHCALL lphConsultCall,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineForward\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGatherDigits(
    HCALL hCall,
    DWORD dwDigitModes,
    LPSTR lpsDigits,
    DWORD dwNumDigits,
    LPCSTR lpszTerminationDigits,
    DWORD dwFirstDigitTimeout,
    DWORD dwInterDigitTimeout)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGatherDigits\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGenerateDigits(
    HCALL hCall,
    DWORD dwDigitMode,
    LPCSTR lpszDigits,
    DWORD dwDuration)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGenerateDigits\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGenerateTone(
    HCALL hCall,
    DWORD dwToneMode,
    DWORD dwDuration,
    DWORD dwNumTones,
    LPLINEGENERATETONE const lpTones)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGenerateTone\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetAddressCaps(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAddressID,
    DWORD dwAPIVersion,
    DWORD dwExtVersion,
    LPLINEADDRESSCAPS lpAddressCaps)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetAddressCaps\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetAddressID(
    HLINE hLine,
    LPDWORD lpdwAddressID,
    DWORD dwAddressMode,
    LPCSTR lpsAddress,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetAddressID\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetAddressStatus(
    HLINE hLine,
    DWORD dwAddressID,
    LPLINEADDRESSSTATUS lpAddressStatus)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetAddressStatus\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetCallInfo(
    HCALL hCall,
    LPLINECALLINFO lpCallInfo)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetCallInfo\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetCallStatus(
    HCALL hCall,
    LPLINECALLSTATUS lpCallStatus)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetCallStatus\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetConfRelatedCalls(
    HCALL hCall,
    LPLINECALLLIST lpCallList)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetConfRelatedCalls\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetDevCaps(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    DWORD dwExtVersion,
    LPLINEDEVCAPS lpLineDevCaps)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetDevCaps\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetDevConfig(
    DWORD dwDeviceID,
    LPVARSTRING lpDeviceConfig,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetDevConfig\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetNewCalls(
    HLINE hLine,
    DWORD dwAddressID,
    DWORD dwSelect,
    LPLINECALLLIST lpCallList)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetNewCalls\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetIcon(
    DWORD dwDeviceID,
    LPCSTR lpszDeviceClass,
    LPHICON lphIcon)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetIcon\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetID(
    HLINE hLine,
    DWORD dwAddressID,
    HCALL hCall,
    DWORD dwSelect,
    LPVARSTRING lpDeviceID,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetID\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetLineDevStatus(
    HLINE hLine,
    LPLINEDEVSTATUS lpLineDevStatus)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetLineDevStatus\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetNumRings(
    HLINE hLine,
    DWORD dwAddressID,
    LPDWORD lpdwNumRings)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetNumRings\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetRequest(
    HLINEAPP hLineApp,
    DWORD dwRequestMode,
    LPVOID lpRequestBuffer)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetRequest\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetStatusMessages(
    HLINE hLine,
    LPDWORD lpdwLineStates,
    LPDWORD lpdwAddressStates)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetStatusMessages\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineHandoff(
    HCALL hCall,
    LPCSTR lpszFileName,
    DWORD dwMediaMode)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineHandoff\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineHold(
    HCALL hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineHold\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineInitialize(
    LPHLINEAPP lphLineApp,
    HINSTANCE hInstance,
    LINECALLBACK lpfnCallback,
    LPCSTR lpszAppName,
    LPDWORD lpdwNumDevs)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineInitialize\n");
#endif
  return LINEERR_NODEVICE;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineMakeCall(
    HLINE hLine,
    LPHCALL lphCall,
    LPCSTR lpszDestAddress,
    DWORD dwCountryCode,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineMakeCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineMonitorDigits(
    HCALL hCall,
    DWORD dwDigitModes)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineMonitorDigits\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineMonitorMedia(
    HCALL hCall,
    DWORD dwMediaModes)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineMonitorMedia\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineMonitorTones(
    HCALL hCall,
    LPLINEMONITORTONE const lpToneList,
    DWORD dwNumEntries)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineMonitorTones\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineNegotiateAPIVersion(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAPILowVersion,
    DWORD dwAPIHighVersion,
    LPDWORD lpdwAPIVersion,
    LPLINEEXTENSIONID lpExtensionID)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineNegotiateAPIVersion\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineNegotiateExtVersion(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    DWORD dwExtLowVersion,
    DWORD dwExtHighVersion,
    LPDWORD lpdwExtVersion)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineNegotiateExtVersion\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineOpen(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    LPHLINE lphLine,
    DWORD dwAPIVersion,
    DWORD dwExtVersion,
    DWORD dwCallbackInstance,
    DWORD dwPrivileges,
    DWORD dwMediaModes,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineOpen\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API linePark(
    HCALL hCall,
    DWORD dwParkMode,
    LPCSTR lpszDirAddress,
    LPVARSTRING lpNonDirAddress)
{
#ifdef DEBUG
  WriteLog("TAPI32: linePark\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API linePickup(
    HLINE hLine,
    DWORD dwAddressID,
    LPHCALL lphCall,
    LPCSTR lpszDestAddress,
    LPCSTR lpszGroupID)
{
#ifdef DEBUG
  WriteLog("TAPI32: linePickup\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API linePrepareAddToConference(
    HCALL            hConfCall,
    LPHCALL          lphConsultCall,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: linePrepareAddToConference\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineRedirect(
    HCALL hCall,
    LPCSTR lpszDestAddress,
    DWORD dwCountryCode)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineRedirect\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineRemoveFromConference(
    HCALL hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineRemoveFromConference\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSecureCall(
    HCALL hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSecureCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSendUserUserInfo(
    HCALL hCall,
    LPCSTR lpsUserUserInfo,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSendUserUserInfo\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetAppSpecific(
    HCALL hCall,
    DWORD dwAppSpecific)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetAppSpecific\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetCallParams(
    HCALL hCall,
    DWORD dwBearerMode,
    DWORD dwMinRate,
    DWORD dwMaxRate,
    LPLINEDIALPARAMS const lpDialParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetCallParams\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetCallPrivilege(
    HCALL hCall,
    DWORD dwCallPrivilege)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetCallPrivilege\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetDevConfig(
    DWORD dwDeviceID,
    LPVOID const lpDeviceConfig,
    DWORD dwSize,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetDevConfig\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetMediaControl(
    HLINE hLine,
    DWORD dwAddressID,
    HCALL hCall,
    DWORD dwSelect,
    LPLINEMEDIACONTROLDIGIT const lpDigitList,
    DWORD dwDigitNumEntries,
    LPLINEMEDIACONTROLMEDIA const lpMediaList,
    DWORD dwMediaNumEntries,
    LPLINEMEDIACONTROLTONE const lpToneList,
    DWORD dwToneNumEntries,
    LPLINEMEDIACONTROLCALLSTATE const lpCallStateList,
    DWORD dwCallStateNumEntries)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetMediaControl\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetMediaMode(
    HCALL hCall,
    DWORD dwMediaModes)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetMediaMode\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetNumRings(
    HLINE hLine,
    DWORD dwAddressID,
    DWORD dwNumRings)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetNumRings\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetStatusMessages(
    HLINE hLine,
    DWORD dwLineStates,
    DWORD dwAddressStates)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetStatusMessages\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetTerminal(
    HLINE hLine,
    DWORD dwAddressID,
    HCALL hCall,
    DWORD dwSelect,
    DWORD dwTerminalModes,
    DWORD dwTerminalID,
    DWORD  bEnable)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetTerminal\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetupConference(
    HCALL hCall,
    HLINE hLine,
    LPHCALL lphConfCall,
    LPHCALL lphConsultCall,
    DWORD dwNumParties,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetupConference\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetupTransfer(
    HCALL hCall,
    LPHCALL lphConsultCall,
    LPLINECALLPARAMS const lpCallParams)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetupTransfer\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineShutdown(
    HLINEAPP hLineApp)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineShutdown\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSwapHold(
    HCALL hActiveCall,
    HCALL hHeldCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSwapHold\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineUncompleteCall(
    HLINE hLine,
    DWORD dwCompletionID)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineUncompleteCall\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineUnhold(
    HCALL hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineUnhold\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineUnpark(
    HLINE hLine,
    DWORD dwAddressID,
    LPHCALL lphCall,
    LPCSTR lpszDestAddress)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineUnpark\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineReleaseUserUserInfo(
    HCALL  hCall)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineReleaseUserUserInfo\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneClose(
    HPHONE hPhone)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneClose\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneConfigDialog(
    DWORD dwDeviceID,
    HWND  hwndOwner,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneConfigDialog\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneDevSpecific(
    HPHONE hPhone,
    LPVOID lpParams,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneDevSpecific\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetButtonInfo(
    HPHONE hPhone,
    DWORD dwButtonLampID,
    LPPHONEBUTTONINFO lpButtonInfo)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetButtonInfo\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetData(
    HPHONE hPhone,
    DWORD dwDataID,
    LPVOID lpData,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetData\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetDevCaps(
    HPHONEAPP hPhoneApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    DWORD dwExtVersion,
    LPPHONECAPS lpPhoneCaps)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetDevCaps\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetDisplay(
    HPHONE hPhone,
    LPVARSTRING lpDisplay)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetDisplay\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetGain(
    HPHONE hPhone,
    DWORD dwHookSwitchDev,
    LPDWORD lpdwGain)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetGain\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetHookSwitch(
    HPHONE hPhone,
    LPDWORD lpdwHookSwitchDevs)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetHookSwitch\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetIcon(
    DWORD dwDeviceID,
    LPCSTR lpszDeviceClass,
    LPHICON lphIcon)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetIcon\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetID(
    HPHONE hPhone,
    LPVARSTRING lpDeviceID,
    LPCSTR lpszDeviceClass)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetID\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetLamp(
    HPHONE hPhone,
    DWORD dwButtonLampID,
    LPDWORD lpdwLampMode)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetLamp\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetRing(
    HPHONE hPhone,
    LPDWORD lpdwRingMode,
    LPDWORD lpdwVolume)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetRing\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetStatus(
    HPHONE hPhone,
    LPPHONESTATUS lpPhoneStatus)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetStatus\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetStatusMessages(
    HPHONE hPhone,
    LPDWORD lpdwPhoneStates,
    LPDWORD lpdwButtonModes,
    LPDWORD lpdwButtonStates)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetStatusMessages\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneGetVolume(
    HPHONE hPhone,
    DWORD dwHookSwitchDev,
    LPDWORD lpdwVolume)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneGetVolume\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneInitialize(
    LPHPHONEAPP   lphPhoneApp,
    HINSTANCE     hInstance,
    PHONECALLBACK lpfnCallback,
    LPCSTR lpszAppName,
    LPDWORD       lpdwNumDevs)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneInitialize\n");
#endif
  return PHONEERR_NODEVICE;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneNegotiateAPIVersion(
    HPHONEAPP hPhoneApp,
    DWORD dwDeviceID,
    DWORD dwAPILowVersion,
    DWORD dwAPIHighVersion,
    LPDWORD lpdwAPIVersion,
    LPPHONEEXTENSIONID lpExtensionID)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneNegotiateAPIVersion\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneNegotiateExtVersion(
    HPHONEAPP hPhoneApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    DWORD dwExtLowVersion,
    DWORD dwExtHighVersion,
    LPDWORD lpdwExtVersion)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneNegotiateExtVersion\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneOpen(
    HPHONEAPP hPhoneApp,
    DWORD dwDeviceID,
    LPHPHONE lphPhone,
    DWORD dwAPIVersion,
    DWORD dwExtVersion,
    DWORD dwCallbackInstance,
    DWORD dwPrivilege)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneOpen\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetButtonInfo(
    HPHONE hPhone,
    DWORD dwButtonLampID,
    LPPHONEBUTTONINFO const lpButtonInfo)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetButtonInfo\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetData(
    HPHONE hPhone,
    DWORD dwDataID,
    LPVOID const lpData,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetData\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetDisplay(
    HPHONE hPhone,
    DWORD dwRow,
    DWORD dwColumn,
    LPCSTR lpsDisplay,
    DWORD dwSize)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetDisplay\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetGain(
    HPHONE hPhone,
    DWORD  dwHookSwitchDev,
    DWORD  dwGain)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetGain\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetHookSwitch(
    HPHONE hPhone,
    DWORD  dwHookSwitchDevs,
    DWORD  dwHookSwitchMode)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetHookSwitch\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetLamp(
    HPHONE hPhone,
    DWORD  dwButtonLampID,
    DWORD  dwLampMode)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetLamp\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetRing(
    HPHONE hPhone,
    DWORD  dwRingMode,
    DWORD  dwVolume)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetRing\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetStatusMessages(
    HPHONE hPhone,
    DWORD  dwPhoneStates,
    DWORD  dwButtonModes,
    DWORD  dwButtonStates)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetStatusMessages\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneSetVolume(
    HPHONE hPhone,
    DWORD  dwHookSwitchDev,
    DWORD  dwVolume)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneSetVolume\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API phoneShutdown(
    HPHONEAPP hPhoneApp)
{
#ifdef DEBUG
  WriteLog("TAPI32: phoneShutdown\n");
#endif
  return PHONEERR_OPERATIONFAILED;
}


//******************************************************************************
//******************************************************************************
LONG WIN32API lineTranslateDialog(
    HLINEAPP hLineApp,
    DWORD dwDeviceID,
    DWORD dwAPIVersion,
    HWND hwndOwner,
    LPCSTR lpszAddressIn)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineTranslateDialog\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetCountry(
    DWORD dwCountryID,
    DWORD dwAPIVersion,
    LPLINECOUNTRYLIST lpLineCountryList)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetCountry\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetAppPriority(
    LPCSTR lpszAppFilename,
    DWORD dwMediaMode,
    LPLINEEXTENSIONID lpExtensionID,
    DWORD dwRequestMode,
    LPVARSTRING lpExtensionName,
    LPDWORD lpdwPriority)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetAppPriority\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineSetAppPriority(
    LPCSTR lpszAppFilename,
    DWORD dwMediaMode,
    LPLINEEXTENSIONID lpExtensionID,
    DWORD dwRequestMode,
    LPCSTR lpszExtensionName,
    DWORD dwPriority)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineSetAppPriority\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineAddProvider(
    LPCSTR lpszProviderFilename,
    HWND hwndOwner,
    LPDWORD lpdwPermanentProviderID)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineAddProvider\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineConfigProvider(
    HWND hwndOwner,
    DWORD dwPermanentProviderID)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineConfigProvider\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineRemoveProvider(
    DWORD dwPermanentProviderID,
    HWND hwndOwner)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineRemoveProvider\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
LONG WIN32API lineGetProviderList(
    DWORD dwAPIVersion,
    LPLINEPROVIDERLIST lpProviderList)
{
#ifdef DEBUG
  WriteLog("TAPI32: lineGetProviderList\n");
#endif
  return LINEERR_OPERATIONFAILED;
}

