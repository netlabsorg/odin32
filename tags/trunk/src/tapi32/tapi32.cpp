/* $Id: tapi32.cpp,v 1.4 1999-08-19 17:05:19 phaller Exp $ */

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
#include <odinwrap.h>
#include <memory.h>
//#include <comtype.h>
#include "misc.h"
#include "tapi32.h"
#include "unicode.h"


ODINDEBUGCHANNEL(TAPI32)

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, tapiRequestMakeCall,
              LPCSTR, lpszDestAddress,
              LPCSTR, lpszAppName,
              LPCSTR, lpszCalledParty,
              LPCSTR, lpszComment)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION10(LONG, tapiRequestMediaCall,
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
ODINFUNCTION2(LONG, tapiRequestDrop,
              HWND, hWnd,
              WPARAM, wRequestID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineRegisterRequestRecipient,
              HLINEAPP, hLineApp,
              DWORD, dwRegistrationInstance,
              DWORD, dwRequestMode,
              DWORD, bEnable)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, tapiGetLocationInfo,
              LPSTR, lpszCountryCode,
              LPSTR, lpszCityCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineSetCurrentLocation,
              HLINEAPP, hLineApp,
              DWORD, dwLocation)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineSetTollList,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              LPCSTR, lpszAddressIn,
              DWORD, dwTollListOption)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(LONG, lineTranslateAddress,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              LPCSTR, lpszAddressIn,
              DWORD, dwCard,
              DWORD, dwTranslateOptions,
              LPLINETRANSLATEOUTPUT, lpTranslateOutput)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetTranslateCaps,
              HLINEAPP, hLineApp,
              DWORD, dwAPIVersion,
              LPLINETRANSLATECAPS, lpTranslateCaps)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineAccept,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineAddToConference,
              HCALL, hConfCall,
              HCALL, hConsultCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineAnswer,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineBlindTransfer,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineClose,
              HLINE, hLine)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineCompleteCall,
              HCALL, hCall,
              LPDWORD, lpdwCompletionID,
              DWORD, dwCompletionMode,
              DWORD, dwMessageID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineCompleteTransfer,
              HCALL, hCall,
              HCALL, hConsultCall,
              LPHCALL, lphConfCall,
              DWORD, dwTransferMode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineConfigDialog,
              DWORD, dwDeviceID,
              HWND, hwndOwner,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LONG, lineConfigDialogEdit,
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
ODINFUNCTION1(LONG, lineDeallocateCall,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineDevSpecific,
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
ODINFUNCTION4(LONG, lineDevSpecificFeature,
              HLINE, hLine,
              DWORD, dwFeature,
              LPVOID, lpParams,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineDial,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineDrop,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(LONG, lineForward,
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
ODINFUNCTION7(LONG, lineGatherDigits,
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
ODINFUNCTION4(LONG, lineGenerateDigits,
              HCALL, hCall,
              DWORD, dwDigitMode,
              LPCSTR, lpszDigits,
              DWORD, dwDuration)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineGenerateTone,
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
ODINFUNCTION6(LONG, lineGetAddressCaps,
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
ODINFUNCTION5(LONG, lineGetAddressID,
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
ODINFUNCTION3(LONG, lineGetAddressStatus,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPLINEADDRESSSTATUS, lpAddressStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineGetCallInfo,
              HCALL, hCall,
              LPLINECALLINFO, lpCallInfo)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineGetCallStatus,
              HCALL, hCall,
              LPLINECALLSTATUS, lpCallStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineGetConfRelatedCalls,
              HCALL, hCall,
              LPLINECALLLIST, lpCallList)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineGetDevCaps,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtVersion,
              LPLINEDEVCAPS, lpLineDevCaps)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetDevConfig,
              DWORD, dwDeviceID,
              LPVARSTRING, lpDeviceConfig,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineGetNewCalls,
              HLINE, hLine,
              DWORD, dwAddressID,
              DWORD, dwSelect,
              LPLINECALLLIST, lpCallList)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetIcon,
              DWORD, dwDeviceID,
              LPCSTR, lpszDeviceClass,
              LPHICON, lphIcon)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LONG, lineGetID,
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
ODINFUNCTION2(LONG, lineGetLineDevStatus,
              HLINE, hLine,
              LPLINEDEVSTATUS, lpLineDevStatus)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetNumRings,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPDWORD, lpdwNumRings)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetRequest,
              HLINEAPP, hLineApp,
              DWORD, dwRequestMode,
              LPVOID, lpRequestBuffer)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineGetStatusMessages,
              HLINE, hLine,
              LPDWORD, lpdwLineStates,
              LPDWORD, lpdwAddressStates)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineHandoff,
              HCALL, hCall,
              LPCSTR, lpszFileName,
              DWORD, dwMediaMode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineHold,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineInitialize,
              LPHLINEAPP, lphLineApp,
              HINSTANCE, hInstance,
              LINECALLBACK, lpfnCallback,
              LPCSTR, lpszAppName,
              LPDWORD, lpdwNumDevs)
{
  return LINEERR_NODEVICE;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineMakeCall,
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
ODINFUNCTION2(LONG, lineMonitorDigits,
              HCALL, hCall,
              DWORD, dwDigitModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineMonitorMedia,
              HCALL, hCall,
              DWORD, dwMediaModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineMonitorTones,
              HCALL, hCall,
              LPLINEMONITORTONE, lpToneList,
              DWORD, dwNumEntries)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LONG, lineNegotiateAPIVersion,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPILowVersion,
              DWORD, dwAPIHighVersion,
              LPDWORD, lpdwAPIVersion,
              LPLINEEXTENSIONID, lpExtensionID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LONG, lineNegotiateExtVersion,
              HLINEAPP, hLineApp,
              DWORD, dwDeviceID,
              DWORD, dwAPIVersion,
              DWORD, dwExtLowVersion,
              DWORD, dwExtHighVersion,
              LPDWORD, lpdwExtVersion)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION9(LONG, lineOpen,
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
ODINFUNCTION4(LONG, linePark,
              HCALL, hCall,
              DWORD, dwParkMode,
              LPCSTR, lpszDirAddress,
              LPVARSTRING, lpNonDirAddress)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, linePickup,
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
ODINFUNCTION3(LONG, linePrepareAddToConference,
              HCALL, hConfCall,
              LPHCALL, lphConsultCall,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineRedirect,
              HCALL, hCall,
              LPCSTR, lpszDestAddress,
              DWORD, dwCountryCode)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineRemoveFromConference,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineSecureCall,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineSendUserUserInfo,
              HCALL, hCall,
              LPCSTR, lpsUserUserInfo,
              DWORD, dwSize)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineSetAppSpecific,
              HCALL, hCall,
              DWORD, dwAppSpecific)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineSetCallParams,
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
ODINFUNCTION2(LONG, lineSetCallPrivilege,
              HCALL, hCall,
              DWORD, dwCallPrivilege)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineSetDevConfig,
              DWORD, dwDeviceID,
              LPVOID, lpDeviceConfig,
              DWORD,  dwSize,
              LPCSTR, lpszDeviceClass)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION12(LONG, lineSetMediaControl,
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
ODINFUNCTION2(LONG, lineSetMediaMode,
              HCALL, hCall,
              DWORD, dwMediaModes)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineSetNumRings,
              HLINE, hLine,
              DWORD, dwAddressID,
              DWORD, dwNumRings)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, lineSetStatusMessages,
              HLINE, hLine,
              DWORD, dwLineStates,
              DWORD, dwAddressStates)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION7(LONG, lineSetTerminal,
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
ODINFUNCTION6(LONG, lineSetupConference,
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
ODINFUNCTION3(LONG, lineSetupTransfer,
              HCALL, hCall,
              LPHCALL, lphConsultCall,
              LPLINECALLPARAMS, lpCallParams)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineShutdown,
              HLINEAPP, hLineApp)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineSwapHold,
              HCALL, hActiveCall,
              HCALL, hHeldCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineUncompleteCall,
              HLINE, hLine,
              DWORD, dwCompletionID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineUnhold,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, lineUnpark,
              HLINE, hLine,
              DWORD, dwAddressID,
              LPHCALL, lphCall,
              LPCSTR, lpszDestAddress)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, lineReleaseUserUserInfo,
              HCALL, hCall)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, phoneClose,
              HPHONE, hPhone)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneConfigDialog,
              DWORD, dwDeviceID,
              HWND, hwndOwner,
              LPCSTR, lpszDeviceClass)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneDevSpecific,
              HPHONE, hPhone,
              LPVOID, lpParams,
              DWORD, dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetButtonInfo,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPPHONEBUTTONINFO, lpButtonInfo)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, phoneGetData,
              HPHONE, hPhone,
              DWORD, dwDataID,
              LPVOID, lpData,
              DWORD, dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, phoneGetDevCaps,
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
ODINFUNCTION2(LONG, phoneGetDisplay,
              HPHONE, hPhone,
              LPVARSTRING, lpDisplay)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetGain,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              LPDWORD, lpdwGain)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, phoneGetHookSwitch,
              HPHONE, hPhone,
              LPDWORD, lpdwHookSwitchDevs)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetIcon,
              DWORD, dwDeviceID,
              LPCSTR, lpszDeviceClass,
              LPHICON, lphIcon)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetID,
              HPHONE, hPhone,
              LPVARSTRING, lpDeviceID,
              LPCSTR, lpszDeviceClass)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetLamp,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPDWORD, lpdwLampMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetRing,
              HPHONE, hPhone,
              LPDWORD, lpdwRingMode,
              LPDWORD, lpdwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, phoneGetStatus,
              HPHONE, hPhone,
              LPPHONESTATUS, lpPhoneStatus)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, phoneGetStatusMessages,
              HPHONE, hPhone,
              LPDWORD, lpdwPhoneStates,
              LPDWORD, lpdwButtonModes,
              LPDWORD, lpdwButtonStates)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneGetVolume,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              LPDWORD, lpdwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, phoneInitialize,
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
ODINFUNCTION6(LONG, phoneNegotiateAPIVersion,
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
ODINFUNCTION6(LONG, phoneNegotiateExtVersion,
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
ODINFUNCTION7(LONG, phoneOpen,
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
ODINFUNCTION3(LONG, phoneSetButtonInfo,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              LPPHONEBUTTONINFO, lpButtonInfo)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, phoneSetData,
              HPHONE, hPhone,
              DWORD, dwDataID,
              LPVOID, lpData,
              DWORD,  dwSize)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, phoneSetDisplay,
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
ODINFUNCTION3(LONG, phoneSetGain,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              DWORD, dwGain)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneSetHookSwitch,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDevs,
              DWORD, dwHookSwitchMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneSetLamp,
              HPHONE, hPhone,
              DWORD, dwButtonLampID,
              DWORD, dwLampMode)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneSetRing,
              HPHONE, hPhone,
              DWORD, dwRingMode,
              DWORD, dwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION4(LONG, phoneSetStatusMessages,
              HPHONE, hPhone,
              DWORD, dwPhoneStates,
              DWORD, dwButtonModes,
              DWORD, dwButtonStates)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION3(LONG, phoneSetVolume,
              HPHONE, hPhone,
              DWORD, dwHookSwitchDev,
              DWORD, dwVolume)
{
  return PHONEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION1(LONG, phoneShutdown,
              HPHONEAPP, hPhoneApp)
{
  return PHONEERR_OPERATIONFAILED;
}


//******************************************************************************
//******************************************************************************
ODINFUNCTION5(LONG, lineTranslateDialog,
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
ODINFUNCTION3(LONG, lineGetCountry,
              DWORD, dwCountryID,
              DWORD, dwAPIVersion,
              LPLINECOUNTRYLIST, lpLineCountryList)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION6(LONG, lineGetAppPriority,
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
ODINFUNCTION6(LONG, lineSetAppPriority,
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
ODINFUNCTION3(LONG, lineAddProvider,
              LPCSTR, lpszProviderFilename,
              HWND, hwndOwner,
              LPDWORD, lpdwPermanentProviderID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineConfigProvider,
              HWND, hwndOwner,
              DWORD, dwPermanentProviderID)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineRemoveProvider,
              DWORD, dwPermanentProviderID,
              HWND, hwndOwner)
{
  return LINEERR_OPERATIONFAILED;
}

//******************************************************************************
//******************************************************************************
ODINFUNCTION2(LONG, lineGetProviderList,
              DWORD, dwAPIVersion,
              LPLINEPROVIDERLIST, lpProviderList)
{
  return LINEERR_OPERATIONFAILED;
}


