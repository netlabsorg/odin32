/*
 * TAPI definitions
 * 
 * Copyright (c) 1999 Andreas Mohr
 */

#ifndef __WINE_TAPI_H
#define __WINE_TAPI_H

#ifndef TAPI_CURRENT_VERSION
#define TAPI_CURRENT_VERSION 0x00020000
#endif

#include "windef.h"

#ifdef __cplusplus
extern "C" {
#endif /* defined(__cplusplus) */

typedef HANDLE HCALL, *LPHCALL;
typedef HANDLE HLINE, *LPHLINE;
typedef HANDLE HLINEAPP, *LPHLINEAPP;
typedef HANDLE HPHONE, *LPHPHONE;
typedef HANDLE HPHONEAPP, *LPHPHONEAPP;

/* FIXME: bogus codes !! */
#define TAPIERR_REQUESTFAILED          20

typedef struct lineaddresscaps_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwLineDeviceID;
    DWORD dwAddressSize;
    DWORD dwAddressOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
    DWORD dwAddressSharing;
    DWORD dwAddressStates;
    DWORD dwCallInfoStates;
    DWORD dwCallerIDFlags;
    DWORD dwCalledIDFlags;
    DWORD dwConnectedIDFlags;
    DWORD dwRedirectionIDFlags;
    DWORD dwRedirectingIDFlags;
    DWORD dwCallStates;
    DWORD dwDialToneModes;
    DWORD dwBusyModes;
    DWORD dwSpecialInfo;
    DWORD dwDisconnectModes;
    DWORD dwMaxNumActiveCalls;
    DWORD dwMaxNumOnHoldCalls;
    DWORD dwMaxNumOnHoldPendingCalls;
    DWORD dwMaxNumConference;
    DWORD dwMaxNumTransConf;
    DWORD dwAddrCapFlags;
    DWORD dwCallFeatures;
    DWORD dwRemoveFromConfCaps;
    DWORD dwRemoveFromConfState;
    DWORD dwTransferModes;
    DWORD dwParkModes;
    DWORD dwForwardModes;
    DWORD dwMaxForwardEntries;
    DWORD dwMaxSpecificEntries;
    DWORD dwMinFwdNumRings;
    DWORD dwMaxFwdNumRings;
    DWORD dwMaxCallCompletions;
    DWORD dwCallCompletionConds;
    DWORD dwCallCompletionModes;
    DWORD dwNumCompletionMessages;
    DWORD dwCompletionMsgTextEntrySize;
    DWORD dwCompletionMsgTextSize;
    DWORD dwCompletionMsgTextOffset;
    DWORD dwAddressFeatures;
} LINEADDRESSCAPS, *LPLINEADDRESSCAPS;

typedef struct lineaddressstatus_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwNumInUse;
    DWORD dwNumActiveCalls;
    DWORD dwNumOnHoldCalls;
    DWORD dwNumOnHoldPendCalls;
    DWORD dwAddressFeatures;
    DWORD dwNumRingsNoAnswer;
    DWORD dwForwardNumEntries;
    DWORD dwForwardSize;
    DWORD dwForwardOffset;
    DWORD dwTerminalModesSize;
    DWORD dwTerminalModesOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} LINEADDRESSSTATUS, *LPLINEADDRESSSTATUS;

typedef struct linedialparams_tag {
    DWORD dwDialPause;
    DWORD dwDialSpeed;
    DWORD dwDigitDuration;
    DWORD dwWaitForDialtone;
} LINEDIALPARAMS, *LPLINEDIALPARAMS;

typedef struct linecallinfo_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    HLINE hLine;
    DWORD dwLineDeviceID;
    DWORD dwAddressID;
    DWORD dwBearerMode;
    DWORD dwRate;
    DWORD dwMediaMode;
    DWORD dwAppSpecific;
    DWORD dwCallID;
    DWORD dwRelatedCallID;
    DWORD dwCallParamFlags;
    DWORD dwCallStates;
    DWORD dwMonitorDigitModes;
    DWORD dwMonitorMediaModes;
    LINEDIALPARAMS DialParams;
    DWORD dwOrigin;
    DWORD dwReason;
    DWORD dwCompletionID;
    DWORD dwNumOwners;
    DWORD dwNumMonitors;
    DWORD dwCountryCode;
    DWORD dwTrunk;
    DWORD dwCallerIDFlags;
    DWORD dwCallerIDSize;
    DWORD dwCallerIDOffset;
    DWORD dwCallerIDNameSize;
    DWORD dwCallerIDNameOffset;
    DWORD dwCalledIDFlags;
    DWORD dwCalledIDSize;
    DWORD dwCalledIDOffset;
    DWORD dwCalledIDNameSize;
    DWORD dwCalledIDNameOffset;
    DWORD dwConnectedIDFlags;
    DWORD dwConnectedIDSize;
    DWORD dwConnectedIDOffset;
    DWORD dwConnectedIDNameSize;
    DWORD dwConnectedIDNameOffset;
    DWORD dwRedirectionIDFlags;
    DWORD dwRedirectionIDSize;
    DWORD dwRedirectionIDOffset;
    DWORD dwRedirectionIDNameSize;
    DWORD dwRedirectionIDNameOffset;
    DWORD dwRedirectingIDFlags;
    DWORD dwRedirectingIDSize;
    DWORD dwRedirectingIDOffset;
    DWORD dwRedirectingIDNameSize;
    DWORD dwRedirectingIDNameOffset;
    DWORD dwAppNameSize;
    DWORD dwAppNameOffset;
    DWORD dwDisplayableAddressSize;
    DWORD dwDisplayableAddressOffset;
    DWORD dwCalledPartySize;
    DWORD dwCalledPartyOffset;
    DWORD dwCommentSize;
    DWORD dwCommentOffset;
    DWORD dwDisplaySize;
    DWORD dwDisplayOffset;
    DWORD dwUserUserInfoSize;
    DWORD dwUserUserInfoOffset;
    DWORD dwHighLevelCompSize;
    DWORD dwHighLevelCompOffset;
    DWORD dwLowLevelCompSize;
    DWORD dwLowLevelCompOffset;
    DWORD dwChargingInfoSize;
    DWORD dwChargingInfoOffset;
    DWORD dwTerminalModesSize;
    DWORD dwTerminalModesOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} LINECALLINFO, *LPLINECALLINFO;

typedef struct linecalllist_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwCallsNumEntries;
    DWORD dwCallsSize;
    DWORD dwCallsOffset;
} LINECALLLIST, *LPLINECALLLIST;

typedef struct linecallparams_tag {
    DWORD dwTotalSize;
    DWORD dwBearerMode;
    DWORD dwMinRate;
    DWORD dwMaxRate;
    DWORD dwMediaMode;
    DWORD dwCallParamFlags;
    DWORD dwAddressMode;
    DWORD dwAddressID;
    LINEDIALPARAMS DialParams;
    DWORD dwOrigAddressSize;
    DWORD dwOrigAddressOffset;
    DWORD dwDisplayableAddressSize;
    DWORD dwDisplayableAddressOffset;
    DWORD dwCalledPartySize;
    DWORD dwCalledPartyOffset;
    DWORD dwCommentSize;
    DWORD dwCommentOffset;
    DWORD dwUserUserInfoSize;
    DWORD dwUserUserInfoOffset;
    DWORD dwHighLevelCompSize;
    DWORD dwHighLevelCompOffset;
    DWORD dwLowLevelCompSize;
    DWORD dwLowLevelCompOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} LINECALLPARAMS, *LPLINECALLPARAMS;

typedef struct linecallstatus_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwCallState;
    DWORD dwCallStateMode;
    DWORD dwCallPrivilege;
    DWORD dwCallFeatures;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} LINECALLSTATUS, *LPLINECALLSTATUS;

typedef struct linecardentry_tag
{
    DWORD       dwPermanentCardID;
    DWORD       dwCardNameSize;
    DWORD       dwCardNameOffset;
    DWORD       dwCardNumberDigits;
    DWORD       dwSameAreaRuleSize;
    DWORD       dwSameAreaRuleOffset;
    DWORD       dwLongDistanceRuleSize;
    DWORD       dwLongDistanceRuleOffset;
    DWORD       dwInternationalRuleSize; 
    DWORD       dwInternationalRuleOffset;
    DWORD       dwOptions;                
} LINECARDENTRY, *LPLINECARDENTRY;

typedef struct linecountrylist_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwNumCountries;
    DWORD dwCountryListSize;
    DWORD dwCountryListOffset;
} LINECOUNTRYLIST, *LPLINECOUNTRYLIST;

typedef struct linecountryentry_tag {
    DWORD dwCountryID;
    DWORD dwCountryCode;
    DWORD dwNextCountryID;
    DWORD dwCountryNameSize;
    DWORD dwCountryNameOffset;
    DWORD dwSameAreaRuleSize;
    DWORD dwSameAreaRuleOffset;
    DWORD dwLongDistanceRuleSize;
    DWORD dwLongDistanceRuleOffset;
    DWORD dwInternationalRuleSize;
    DWORD dwInternationalRuleOffset;
} LINECOUNTRYENTRY, *LPLINECOUNTRYENTRY;

typedef struct linedevcaps_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwProviderInfoSize;
    DWORD dwProviderInfoOffset;
    DWORD dwSwitchInfoSize;
    DWORD dwSwitchInfoOffset;
    DWORD dwPermanentLineID;
    DWORD dwLineNameSize;
    DWORD dwLineNameOffset;
    DWORD dwStringFormat;
    DWORD dwAddressModes;
    DWORD dwNumAddresses;
    DWORD dwBearerModes;
    DWORD dwMaxRate;
    DWORD dwMediaModes;
    DWORD dwGenerateToneModes;
    DWORD dwGenerateToneMaxNumFreq;
    DWORD dwGenerateDigitModes;
    DWORD dwMonitorToneMaxNumFreq;
    DWORD dwMonitorToneMaxNumEntries;
    DWORD dwMonitorDigitModes;
    DWORD dwGatherDigitsMinTimeout;
    DWORD dwGatherDigitsMaxTimeout;
    DWORD dwMedCtlDigitMaxListSize;
    DWORD dwMedCtlMediaMaxListSize;
    DWORD dwMedCtlToneMaxListSize;
    DWORD dwMedCtlCallStateMaxListSize;
    DWORD dwDevCapFlags;
    DWORD dwMaxNumActiveCalls;
    DWORD dwAnswerMode;
    DWORD dwRingModes;
    DWORD dwLineStates;
    DWORD dwUUIAcceptSize;
    DWORD dwUUIAnswerSize;
    DWORD dwUUIMakeCallSize;
    DWORD dwUUIDropSize;
    DWORD dwUUISendUserUserInfoSize;
    DWORD dwUUICallInfoSize;
    LINEDIALPARAMS MinDialParams;
    LINEDIALPARAMS MaxDialParams;
    LINEDIALPARAMS DefaultDialParams;
    DWORD dwNumTerminals;
    DWORD dwTerminalCapsSize;
    DWORD dwTerminalCapsOffset;
    DWORD dwTerminalTextEntrySize;
    DWORD dwTerminalTextSize;
    DWORD dwTerminalTextOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
    DWORD dwLineFeatures;
} LINEDEVCAPS, *LPLINEDEVCAPS;

typedef struct linedevstatus_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwNumOpens;
    DWORD dwOpenMediaModes;
    DWORD dwNumActiveCalls;
    DWORD dwNumOnHoldCalls;
    DWORD dwNumOnHoldPendingCalls;
    DWORD dwLineFeatures;
    DWORD dwNumCallCompletion;
    DWORD dwRingMode;
    DWORD dwSignalLevel;
    DWORD dwBatteryLevel;
    DWORD dwRoamMode;
    DWORD dwDevStatusFlags;
    DWORD dwTerminalModesSize;
    DWORD dwTerminalModesOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} LINEDEVSTATUS, *LPLINEDEVSTATUS;

typedef struct lineextensionid_tag {
    DWORD dwExtensionID0;
    DWORD dwExtensionID1;
    DWORD dwExtensionID2;
    DWORD dwExtensionID3;
} LINEEXTENSIONID, *LPLINEEXTENSIONID;

typedef struct lineforward_tag {
    DWORD dwForwardMode;
    DWORD dwCallerAddressSize;
    DWORD dwCallerAddressOffset;
    DWORD dwDestCountryCode;
    DWORD dwDestAddressSize;
    DWORD dwDestAddressOffset;
} LINEFORWARD, *LPLINEFORWARD;

typedef struct lineforwardlist_tag {
    DWORD dwTotalSize;
    DWORD dwNumEntries;
    LINEFORWARD ForwardList[1];
} LINEFORWARDLIST, *LPLINEFORWARDLIST;
    
typedef struct linegeneratetone_tag {
    DWORD dwFrequency;
    DWORD dwCadenceOn;
    DWORD dwCadenceOff;
    DWORD dwVolume;
} LINEGENERATETONE, *LPLINEGENERATETONE;

typedef struct linemediacontrolcallstate_tag {
    DWORD dwCallStates;
    DWORD dwMediaControl;
} LINEMEDIACONTROLCALLSTATE, *LPLINEMEDIACONTROLCALLSTATE;

typedef struct linemediacontroldigit_tag {
    DWORD dwDigit;
    DWORD dwDigitModes;
    DWORD dwMediaControl;
} LINEMEDIACONTROLDIGIT, *LPLINEMEDIACONTROLDIGIT;

typedef struct linemediacontrolmedia_tag {
    DWORD dwMediaModes;
    DWORD dwDuration;
    DWORD dwMediaControl;
} LINEMEDIACONTROLMEDIA, *LPLINEMEDIACONTROLMEDIA;

typedef struct linemediacontroltone_tag {
    DWORD dwAppSpecific;
    DWORD dwDuration;
    DWORD dwFrequency1;
    DWORD dwFrequency2;
    DWORD dwFrequency3;
    DWORD dwMediaControl;
} LINEMEDIACONTROLTONE, *LPLINEMEDIACONTROLTONE;

typedef struct linemonitortone_tag {
    DWORD dwAppSpecific;
    DWORD dwDuration;
    DWORD dwFrequency1;
    DWORD dwFrequency2;
    DWORD dwFrequency3;
} LINEMONITORTONE, *LPLINEMONITORTONE;

typedef struct lineproviderlist_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwNumProviders;
    DWORD dwProviderListSize;
    DWORD dwProviderListOffset;
} LINEPROVIDERLIST, *LPLINEPROVIDERLIST;

typedef struct linetranslatecaps_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwNumLocations;
    DWORD dwLocationListSize;
    DWORD dwLocationListOffset;
    DWORD dwCurrentLocationID;
    DWORD dwNumCards;
    DWORD dwCardListSize;
    DWORD dwCardListOffset;
    DWORD dwCurrentPreferredCardID;
} LINETRANSLATECAPS, *LPLINETRANSLATECAPS;

typedef struct linetranslateoutput_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwDialableStringSize;
    DWORD dwDialableStringOffset;
    DWORD dwDisplayableStringSize;
    DWORD dwDisplayableStringOffset;
    DWORD dwCurrentCountry;
    DWORD dwDestCountry;
    DWORD dwTranslateResults;
} LINETRANSLATEOUTPUT, *LPLINETRANSLATEOUTPUT;

typedef struct linelocationentry_tag
{
    DWORD       dwPermanentLocationID;
    DWORD       dwLocationNameSize;
    DWORD       dwLocationNameOffset;
    DWORD       dwCountryCode;
    DWORD       dwCityCodeSize;
    DWORD       dwCityCodeOffset;
    DWORD       dwPreferredCardID;
    DWORD       dwLocalAccessCodeSize;         
    DWORD       dwLocalAccessCodeOffset;       
    DWORD       dwLongDistanceAccessCodeSize;  
    DWORD       dwLongDistanceAccessCodeOffset;
    DWORD       dwTollPrefixListSize;   
    DWORD       dwTollPrefixListOffset; 
    DWORD       dwCountryID;            
    DWORD       dwOptions;              
    DWORD       dwCancelCallWaitingSize;
    DWORD       dwCancelCallWaitingOffset;
} LINELOCATIONENTRY, *LPLINELOCATIONENTRY;

typedef void (* CALLBACK LINECALLBACK)(DWORD, DWORD, DWORD, DWORD, DWORD, DWORD);

typedef struct _PHONEAPP {
  int dummy;
} PHONEAPP, *LPPHONEAPP;

typedef struct _PHONE {
    DWORD dwRingMode;
    DWORD dwVolume;
} PHONE, *LPPHONE;

typedef struct phonebuttoninfo_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwButtonMode;
    DWORD dwButtonFunction;
    DWORD dwButtonTextSize;
    DWORD dwButtonTextOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
    DWORD dwButtonState;
} PHONEBUTTONINFO, *LPPHONEBUTTONINFO;

typedef struct phonecaps_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwProviderInfoSize;
    DWORD dwProviderInfoOffset;
    DWORD dwPhoneInfoSize;
    DWORD dwPhoneInfoOffset;
    DWORD dwPermanentPhoneID;
    DWORD dwPhoneNameSize;
    DWORD dwPhoneNameOffset;
    DWORD dwStringFormat;
    DWORD dwPhoneStates;
    DWORD dwHookSwitchDevs;
    DWORD dwHandsetHookSwitchModes;
    DWORD dwSpeakerHookSwitchModes;
    DWORD dwHeadsetHookSwitchModes;
    DWORD dwVolumeFlags;
    DWORD dwGainFlags;
    DWORD dwDisplayNumRows;
    DWORD dwDisplayNumColumns;
    DWORD dwNumRingModes;
    DWORD dwNumButtonLamps;
    DWORD dwButtonModesSize;
    DWORD dwButtonModesOffset;
    DWORD dwButtonFunctionsSize;
    DWORD dwButtonFunctionsOffset;
    DWORD dwLampModesSize;
    DWORD dwLampModesOffset;
    DWORD dwNumSetData;
    DWORD dwSetDataSize;
    DWORD dwSetDataOffset;
    DWORD dwNumGetData;
    DWORD dwGetDataSize;
    DWORD dwGetDataOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} PHONECAPS, *LPPHONECAPS;

typedef struct phoneextensionid_tag {
    DWORD dwExtensionID0;
    DWORD dwExtensionID1;
    DWORD dwExtensionID2;
    DWORD dwExtensionID3;
} PHONEEXTENSIONID, *LPPHONEEXTENSIONID;

typedef struct phonestatus_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwStatusFlags;
    DWORD dwNumOwners;
    DWORD dwRingMOde;
    DWORD dwRingVolume;
    DWORD dwHandsetHookSwitchMode;
    DWORD dwHandsetVolume;
    DWORD dwHandsetGain;
    DWORD dwSpeakerHookSwitchMode;
    DWORD dwSpeakerVolume;
    DWORD dwSpeakerGain;
    DWORD dwHeadsetHookSwitchMode;
    DWORD dwHeadsetVolume;
    DWORD dwHeadsetGain;
    DWORD dwDisplaySize;
    DWORD dwDisplayOffset;
    DWORD dwLampModesSize;
    DWORD dwLampModesOffset;
    DWORD dwOwnerNameSize;
    DWORD dwOwnerNameOffset;
    DWORD dwDevSpecificSize;
    DWORD dwDevSpecificOffset;
} PHONESTATUS, *LPPHONESTATUS;

typedef void (* CALLBACK PHONECALLBACK)(HANDLE, DWORD, DWORD, DWORD, DWORD, DWORD);

typedef struct varstring_tag {
    DWORD dwTotalSize;
    DWORD dwNeededSize;
    DWORD dwUsedSize;
    DWORD dwStringFormat;
    DWORD dwStringSize;
    DWORD dwStringOffset;
} VARSTRING, *LPVARSTRING;

/* line functions */
DWORD WINAPI lineAccept(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineAddProvider(LPCSTR,HWND,LPDWORD);
DWORD WINAPI lineAddToConference(HCALL,HCALL);
DWORD WINAPI lineAnswer(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineBlindTransfer(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineClose(HLINE);
DWORD WINAPI lineCompleteCall(HCALL,LPDWORD,DWORD,DWORD);
DWORD WINAPI lineCompleteTransfer(HCALL,HCALL,LPHCALL,DWORD);
DWORD WINAPI lineConfigDialog(DWORD,HWND,LPCSTR);
DWORD WINAPI lineConfigDialogEdit(DWORD,HWND,LPCSTR,LPVOID, DWORD,LPVARSTRING);
DWORD WINAPI lineConfigProvider(HWND,DWORD);
DWORD WINAPI lineDeallocateCall(HCALL);
DWORD WINAPI lineDevSpecific(HLINE,DWORD,HCALL,LPVOID,DWORD);
DWORD WINAPI lineDevSpecificFeature(HLINE,DWORD,LPVOID,DWORD);
DWORD WINAPI lineDial(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineDrop(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineForward(HLINE,DWORD,DWORD,LPLINEFORWARDLIST,DWORD,LPHCALL,LPLINECALLPARAMS);
DWORD WINAPI lineGatherDigits(HCALL,DWORD,LPSTR,DWORD,LPCSTR,DWORD,DWORD);
DWORD WINAPI lineGenerateDigits(HCALL,DWORD,LPCSTR,DWORD);
DWORD WINAPI lineGenerateTone(HCALL,DWORD,DWORD,DWORD,LPLINEGENERATETONE);
DWORD WINAPI lineGetAddressCaps(HLINEAPP,DWORD,DWORD,DWORD,DWORD,LPLINEADDRESSCAPS);
DWORD WINAPI lineGetAddressID(HLINE,LPDWORD,DWORD,LPCSTR,DWORD);
DWORD WINAPI lineGetAddressStatus(HLINE,DWORD,LPLINEADDRESSSTATUS);
DWORD WINAPI lineGetAppPriority(LPCSTR,DWORD,LPLINEEXTENSIONID,DWORD,LPVARSTRING,LPDWORD);
DWORD WINAPI lineGetCallInfo(HCALL,LPLINECALLINFO);
DWORD WINAPI lineGetCallStatus(HCALL,LPLINECALLSTATUS);
DWORD WINAPI lineGetConfRelatedCalls(HCALL,LPLINECALLLIST);
DWORD WINAPI lineGetCountry(DWORD,DWORD,LPLINECOUNTRYLIST);
DWORD WINAPI lineGetDevCaps(HLINEAPP,DWORD,DWORD,DWORD,LPLINEDEVCAPS);
DWORD WINAPI lineGetDevConfig(DWORD,LPVARSTRING,LPCSTR);
DWORD WINAPI lineGetID(HLINE,DWORD,HCALL,DWORD,LPVARSTRING,LPCSTR);
DWORD WINAPI lineGetIcon(DWORD,LPCSTR,HICON *);
DWORD WINAPI lineGetLineDevStatus(HLINE,LPLINEDEVSTATUS);
DWORD WINAPI lineGetNewCalls(HLINE,DWORD,DWORD,LPLINECALLLIST);
DWORD WINAPI lineGetNumRings(HLINE,DWORD,LPDWORD);
DWORD WINAPI lineGetProviderList(DWORD dwAPIVersion,LPLINEPROVIDERLIST);
DWORD WINAPI lineGetRequest(HLINEAPP,DWORD,LPVOID);
DWORD WINAPI lineGetStatusMessages(HLINE,LPDWORD,LPDWORD);
DWORD WINAPI lineGetTranslateCaps(HLINEAPP,DWORD,LPLINETRANSLATECAPS);
DWORD WINAPI lineHandoff(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineHold(HCALL);
DWORD WINAPI lineInitialize(LPHLINEAPP,HINSTANCE,LINECALLBACK,LPCSTR,LPDWORD);
DWORD WINAPI lineMakeCall(HLINE,LPHCALL,LPCSTR,DWORD,LPLINECALLPARAMS);
DWORD WINAPI lineMonitorDigits(HCALL,DWORD);
DWORD WINAPI lineMonitorMedia(HCALL,DWORD);
DWORD WINAPI lineMonitorTones(HCALL,LPLINEMONITORTONE,DWORD);
DWORD WINAPI lineNegotiateAPIVersion(HLINEAPP,DWORD,DWORD,DWORD,LPDWORD,LPLINEEXTENSIONID);
DWORD WINAPI lineNegotiateExtVersion(HLINEAPP,DWORD,DWORD,DWORD,DWORD,LPDWORD);
DWORD WINAPI lineOpen(HLINEAPP,DWORD,LPHLINE,DWORD,DWORD,DWORD,DWORD,DWORD,LPLINECALLPARAMS);
DWORD WINAPI linePark(HCALL,DWORD,LPCSTR,LPVARSTRING);
DWORD WINAPI linePickup(HLINE,DWORD,LPHCALL,LPCSTR,LPCSTR);
DWORD WINAPI linePrepareAddToConference(HCALL,LPHCALL,LPLINECALLPARAMS);
DWORD WINAPI lineRedirect(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineRegisterRequestRecipient(HLINEAPP,DWORD,DWORD,DWORD);
DWORD WINAPI lineReleaseUserUserInfo(HCALL);
DWORD WINAPI lineRemoveFromConference(HCALL);
DWORD WINAPI lineRemoveProvider(DWORD,HWND);
DWORD WINAPI lineSecureCall(HCALL);
DWORD WINAPI lineSendUserUserInfo(HCALL,LPCSTR,DWORD);
DWORD WINAPI lineSetAppPriority(LPCSTR,DWORD,LPLINEEXTENSIONID,DWORD,LPCSTR,DWORD);
DWORD WINAPI lineSetAppSpecific(HCALL,DWORD);
DWORD WINAPI lineSetCallParams(HCALL,DWORD,DWORD,DWORD,LPLINEDIALPARAMS);
DWORD WINAPI lineSetCallPrivilege(HCALL,DWORD);
DWORD WINAPI lineSetCurrentLocation(HLINEAPP,DWORD);
DWORD WINAPI lineSetDevConfig(DWORD,LPVOID,DWORD,LPCSTR);
DWORD WINAPI lineSetMediaControl(HLINE,DWORD,HCALL,DWORD,LPLINEMEDIACONTROLDIGIT,DWORD,LPLINEMEDIACONTROLMEDIA,DWORD,LPLINEMEDIACONTROLTONE,DWORD,LPLINEMEDIACONTROLCALLSTATE,DWORD);
DWORD WINAPI lineSetMediaMode(HCALL,DWORD);
DWORD WINAPI lineSetNumRings(HLINE,DWORD,DWORD);
DWORD WINAPI lineSetStatusMessages(HLINE,DWORD,DWORD);
DWORD WINAPI lineSetTerminal(HLINE,DWORD,HCALL,DWORD,DWORD,DWORD,DWORD);
DWORD WINAPI lineSetTollList(HLINEAPP,DWORD,LPCSTR,DWORD);
DWORD WINAPI lineSetupConference(HCALL,HLINE,LPHCALL,LPHCALL,DWORD,LPLINECALLPARAMS);
DWORD WINAPI lineSetupTransfer(HCALL,LPHCALL,LPLINECALLPARAMS);
DWORD WINAPI lineShutdown(HLINEAPP);
DWORD WINAPI lineSwapHold(HCALL,HCALL);
DWORD WINAPI lineTranslateAddress(HLINEAPP,DWORD,DWORD,LPCSTR,DWORD,DWORD,LPLINETRANSLATEOUTPUT);
DWORD WINAPI lineTranslateDialog(HLINEAPP,DWORD,DWORD,HWND,LPCSTR);
DWORD WINAPI lineUncompleteCall(HLINE,DWORD);
DWORD WINAPI lineUnHold(HCALL);
DWORD WINAPI lineUnpark(HLINE,DWORD,LPHCALL,LPCSTR);

/* phone functions */
DWORD WINAPI phoneClose(HPHONE);
DWORD WINAPI phoneConfigDialog(DWORD,HWND,LPCSTR);
DWORD WINAPI phoneDevSpecific(HPHONE,LPVOID,DWORD);
DWORD WINAPI phoneGetButtonInfo(HPHONE,DWORD,LPPHONEBUTTONINFO);
DWORD WINAPI phoneGetData(HPHONE,DWORD,LPVOID,DWORD);
DWORD WINAPI phoneGetDevCaps(HPHONEAPP,DWORD,DWORD,DWORD,LPPHONECAPS);
DWORD WINAPI phoneGetDisplay(HPHONE,LPVARSTRING);
DWORD WINAPI phoneGetGain(HPHONE,DWORD,LPDWORD);
DWORD WINAPI phoneGetHookSwitch(HPHONE,LPDWORD);
DWORD WINAPI phoneGetID(HPHONE,LPVARSTRING,LPCSTR);
DWORD WINAPI phoneGetIcon(DWORD,LPCSTR,HICON *);
DWORD WINAPI phoneGetLamp(HPHONE,DWORD,LPDWORD);
DWORD WINAPI phoneGetRing(HPHONE,LPDWORD,LPDWORD);
DWORD WINAPI phoneGetStatus(HPHONE,LPPHONESTATUS);
DWORD WINAPI phoneGetStatusMessages(HPHONE,LPDWORD,LPDWORD,LPDWORD);
DWORD WINAPI phoneGetVolume(HPHONE,DWORD,LPDWORD);
DWORD WINAPI phoneInitialize(LPHPHONEAPP,HINSTANCE,PHONECALLBACK,LPCSTR,LPDWORD);
DWORD WINAPI phoneNegotiateAPIVersion(HPHONEAPP,DWORD,DWORD,DWORD,LPDWORD,LPPHONEEXTENSIONID);
DWORD WINAPI phoneNegotiateExtVersion(HPHONEAPP,DWORD,DWORD,DWORD,DWORD,LPDWORD);
DWORD WINAPI phoneOpen(HPHONEAPP,DWORD,LPHPHONE,DWORD,DWORD,DWORD,DWORD);
DWORD WINAPI phoneSetButtonInfo(HPHONE,DWORD,LPPHONEBUTTONINFO);
DWORD WINAPI phoneSetData(HPHONE,DWORD,LPVOID,DWORD);
DWORD WINAPI phoneSetDisplay(HPHONE,DWORD,DWORD,LPCSTR,DWORD);
DWORD WINAPI phoneSetGain(HPHONE,DWORD,DWORD);
DWORD WINAPI phoneSetHookSwitch(HPHONE,DWORD,DWORD);
DWORD WINAPI phoneSetLamp(HPHONE,DWORD,DWORD);
DWORD WINAPI phoneSetRing(HPHONE,DWORD,DWORD);
DWORD WINAPI phoneSetStatusMessages(HPHONE,DWORD,DWORD,DWORD);
DWORD WINAPI phoneSetVolume(HPHONE,DWORD,DWORD);
DWORD WINAPI phoneShutdown(HPHONEAPP);

/* "assisted" functions */
DWORD WINAPI tapiGetLocationInfo(LPSTR,LPSTR);
DWORD WINAPI tapiRequestMakeCall(LPCSTR,LPCSTR,LPCSTR,LPCSTR);

#define LINEERR_ALLOCATED                       0x80000001
#define LINEERR_BADDEVICEID                     0x80000002
#define LINEERR_BEARERMODEUNAVAIL               0x80000003
#define LINEERR_CALLUNAVAIL                     0x80000005
#define LINEERR_COMPLETIONOVERRUN               0x80000006
#define LINEERR_CONFERENCEFULL                  0x80000007
#define LINEERR_DIALBILLING                     0x80000008
#define LINEERR_DIALDIALTONE                    0x80000009
#define LINEERR_DIALPROMPT                      0x8000000A
#define LINEERR_DIALQUIET                       0x8000000B
#define LINEERR_INCOMPATIBLEAPIVERSION          0x8000000C
#define LINEERR_INCOMPATIBLEEXTVERSION          0x8000000D
#define LINEERR_INIFILECORRUPT                  0x8000000E
#define LINEERR_INUSE                           0x8000000F
#define LINEERR_INVALADDRESS                    0x80000010
#define LINEERR_INVALADDRESSID                  0x80000011
#define LINEERR_INVALADDRESSMODE                0x80000012
#define LINEERR_INVALADDRESSSTATE               0x80000013
#define LINEERR_INVALAPPHANDLE                  0x80000014
#define LINEERR_INVALAPPNAME                    0x80000015
#define LINEERR_INVALBEARERMODE                 0x80000016
#define LINEERR_INVALCALLCOMPLMODE              0x80000017
#define LINEERR_INVALCALLHANDLE                 0x80000018
#define LINEERR_INVALCALLPARAMS                 0x80000019
#define LINEERR_INVALCALLPRIVILEGE              0x8000001A
#define LINEERR_INVALCALLSELECT                 0x8000001B
#define LINEERR_INVALCALLSTATE                  0x8000001C
#define LINEERR_INVALCALLSTATELIST              0x8000001D
#define LINEERR_INVALCARD                       0x8000001E
#define LINEERR_INVALCOMPLETIONID               0x8000001F
#define LINEERR_INVALCONFCALLHANDLE             0x80000020
#define LINEERR_INVALCONSULTCALLHANDLE          0x80000021
#define LINEERR_INVALCOUNTRYCODE                0x80000022
#define LINEERR_INVALDEVICECLASS                0x80000023
#define LINEERR_INVALDEVICEHANDLE               0x80000024
#define LINEERR_INVALDIALPARAMS                 0x80000025
#define LINEERR_INVALDIGITLIST                  0x80000026
#define LINEERR_INVALDIGITMODE                  0x80000027
#define LINEERR_INVALDIGITS                     0x80000028
#define LINEERR_INVALEXTVERSION                 0x80000029
#define LINEERR_INVALGROUPID                    0x8000002A
#define LINEERR_INVALLINEHANDLE                 0x8000002B
#define LINEERR_INVALLINESTATE                  0x8000002C
#define LINEERR_INVALLOCATION                   0x8000002D
#define LINEERR_INVALMEDIALIST                  0x8000002E
#define LINEERR_INVALMEDIAMODE                  0x8000002F
#define LINEERR_INVALMESSAGEID                  0x80000030
#define LINEERR_INVALPARAM                      0x80000032
#define LINEERR_INVALPARKID                     0x80000033
#define LINEERR_INVALPARKMODE                   0x80000034
#define LINEERR_INVALPOINTER                    0x80000035
#define LINEERR_INVALPRIVSELECT                 0x80000036
#define LINEERR_INVALRATE                       0x80000037
#define LINEERR_INVALREQUESTMODE                0x80000038
#define LINEERR_INVALTERMINALID                 0x80000039
#define LINEERR_INVALTERMINALMODE               0x8000003A
#define LINEERR_INVALTIMEOUT                    0x8000003B
#define LINEERR_INVALTONE                       0x8000003C
#define LINEERR_INVALTONELIST                   0x8000003D
#define LINEERR_INVALTONEMODE                   0x8000003E
#define LINEERR_INVALTRANSFERMODE               0x8000003F
#define LINEERR_LINEMAPPERFAILED                0x80000040
#define LINEERR_NOCONFERENCE                    0x80000041
#define LINEERR_NODEVICE                        0x80000042
#define LINEERR_NODRIVER                        0x80000043
#define LINEERR_NOMEM                           0x80000044
#define LINEERR_NOREQUEST                       0x80000045
#define LINEERR_NOTOWNER                        0x80000046
#define LINEERR_NOTREGISTERED                   0x80000047
#define LINEERR_OPERATIONFAILED                 0x80000048
#define LINEERR_OPERATIONUNAVAIL                0x80000049
#define LINEERR_RATEUNAVAIL                     0x8000004A
#define LINEERR_RESOURCEUNAVAIL                 0x8000004B
#define LINEERR_REQUESTOVERRUN                  0x8000004C
#define LINEERR_STRUCTURETOOSMALL               0x8000004D
#define LINEERR_TARGETNOTFOUND                  0x8000004E
#define LINEERR_TARGETSELF                      0x8000004F
#define LINEERR_UNINITIALIZED                   0x80000050
#define LINEERR_USERUSERINFOTOOBIG              0x80000051
#define LINEERR_REINIT                          0x80000052
#define LINEERR_ADDRESSBLOCKED                  0x80000053
#define LINEERR_BILLINGREJECTED                 0x80000054
#define LINEERR_INVALFEATURE                    0x80000055
#define LINEERR_NOMULTIPLEINSTANCE              0x80000056
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEERR_INVALAGENTID                    0x80000057
#define LINEERR_INVALAGENTGROUP                 0x80000058
#define LINEERR_INVALPASSWORD                   0x80000059
#define LINEERR_INVALAGENTSTATE                 0x8000005A
#define LINEERR_INVALAGENTACTIVITY              0x8000005B
#define LINEERR_DIALVOICEDETECT                 0x8000005C
#endif

#define PHONEERR_ALLOCATED                      0x90000001
#define PHONEERR_BADDEVICEID                    0x90000002
#define PHONEERR_INCOMPATIBLEAPIVERSION         0x90000003
#define PHONEERR_INCOMPATIBLEEXTVERSION         0x90000004
#define PHONEERR_INIFILECORRUPT                 0x90000005
#define PHONEERR_INUSE                          0x90000006
#define PHONEERR_INVALAPPHANDLE                 0x90000007
#define PHONEERR_INVALAPPNAME                   0x90000008
#define PHONEERR_INVALBUTTONLAMPID              0x90000009
#define PHONEERR_INVALBUTTONMODE                0x9000000A
#define PHONEERR_INVALBUTTONSTATE               0x9000000B
#define PHONEERR_INVALDATAID                    0x9000000C
#define PHONEERR_INVALDEVICECLASS               0x9000000D
#define PHONEERR_INVALEXTVERSION                0x9000000E
#define PHONEERR_INVALHOOKSWITCHDEV             0x9000000F
#define PHONEERR_INVALHOOKSWITCHMODE            0x90000010
#define PHONEERR_INVALLAMPMODE                  0x90000011
#define PHONEERR_INVALPARAM                     0x90000012
#define PHONEERR_INVALPHONEHANDLE               0x90000013
#define PHONEERR_INVALPHONESTATE                0x90000014
#define PHONEERR_INVALPOINTER                   0x90000015
#define PHONEERR_INVALPRIVILEGE                 0x90000016
#define PHONEERR_INVALRINGMODE                  0x90000017
#define PHONEERR_NODEVICE                       0x90000018
#define PHONEERR_NODRIVER                       0x90000019
#define PHONEERR_NOMEM                          0x9000001A
#define PHONEERR_NOTOWNER                       0x9000001B
#define PHONEERR_OPERATIONFAILED                0x9000001C
#define PHONEERR_OPERATIONUNAVAIL               0x9000001D
#define PHONEERR_RESOURCEUNAVAIL                0x9000001F
#define PHONEERR_REQUESTOVERRUN                 0x90000020
#define PHONEERR_STRUCTURETOOSMALL              0x90000021
#define PHONEERR_UNINITIALIZED                  0x90000022
#define PHONEERR_REINIT                         0x90000023


#define LINEMEDIACONTROL_NONE                   0x00000001
#define LINEMEDIACONTROL_START                  0x00000002
#define LINEMEDIACONTROL_RESET                  0x00000004
#define LINEMEDIACONTROL_PAUSE                  0x00000008
#define LINEMEDIACONTROL_RESUME                 0x00000010
#define LINEMEDIACONTROL_RATEUP                 0x00000020
#define LINEMEDIACONTROL_RATEDOWN               0x00000040
#define LINEMEDIACONTROL_RATENORMAL             0x00000080
#define LINEMEDIACONTROL_VOLUMEUP               0x00000100
#define LINEMEDIACONTROL_VOLUMEDOWN             0x00000200
#define LINEMEDIACONTROL_VOLUMENORMAL           0x00000400

#define LINEMEDIAMODE_UNKNOWN                   0x00000002
#define LINEMEDIAMODE_INTERACTIVEVOICE          0x00000004
#define LINEMEDIAMODE_AUTOMATEDVOICE            0x00000008
#define LINEMEDIAMODE_DATAMODEM                 0x00000010
#define LINEMEDIAMODE_G3FAX                     0x00000020
#define LINEMEDIAMODE_TDD                       0x00000040
#define LINEMEDIAMODE_G4FAX                     0x00000080
#define LINEMEDIAMODE_DIGITALDATA               0x00000100
#define LINEMEDIAMODE_TELETEX                   0x00000200
#define LINEMEDIAMODE_VIDEOTEX                  0x00000400
#define LINEMEDIAMODE_TELEX                     0x00000800
#define LINEMEDIAMODE_MIXED                     0x00001000
#define LINEMEDIAMODE_ADSI                      0x00002000
#define LINEMEDIAMODE_VOICEVIEW                 0x00004000
#define LAST_LINEMEDIAMODE                      0x00004000

#define LINEOFFERINGMODE_ACTIVE                 0x00000001      
#define LINEOFFERINGMODE_INACTIVE               0x00000002      

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEOPENOPTION_SINGLEADDRESS            0x80000000      
#define LINEOPENOPTION_PROXY                    0x40000000      
#endif

#define LINEPARKMODE_DIRECTED                   0x00000001
#define LINEPARKMODE_NONDIRECTED                0x00000002

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEPROXYREQUEST_SETAGENTGROUP          0x00000001      
#define LINEPROXYREQUEST_SETAGENTSTATE          0x00000002      
#define LINEPROXYREQUEST_SETAGENTACTIVITY       0x00000003      
#define LINEPROXYREQUEST_GETAGENTCAPS           0x00000004      
#define LINEPROXYREQUEST_GETAGENTSTATUS         0x00000005      
#define LINEPROXYREQUEST_AGENTSPECIFIC          0x00000006      
#define LINEPROXYREQUEST_GETAGENTACTIVITYLIST   0x00000007      
#define LINEPROXYREQUEST_GETAGENTGROUPLIST      0x00000008      
#endif

#define LINEREMOVEFROMCONF_NONE                 0x00000001
#define LINEREMOVEFROMCONF_LAST                 0x00000002
#define LINEREMOVEFROMCONF_ANY                  0x00000003

#define LINEREQUESTMODE_MAKECALL                0x00000001
#define LINEREQUESTMODE_MEDIACALL               0x00000002
#define LINEREQUESTMODE_DROP                    0x00000004
#define LAST_LINEREQUESTMODE                    LINEREQUESTMODE_MEDIACALL

#define LINEROAMMODE_UNKNOWN                    0x00000001
#define LINEROAMMODE_UNAVAIL                    0x00000002
#define LINEROAMMODE_HOME                       0x00000004
#define LINEROAMMODE_ROAMA                      0x00000008
#define LINEROAMMODE_ROAMB                      0x00000010

#define LINESPECIALINFO_NOCIRCUIT               0x00000001
#define LINESPECIALINFO_CUSTIRREG               0x00000002
#define LINESPECIALINFO_REORDER                 0x00000004
#define LINESPECIALINFO_UNKNOWN                 0x00000008
#define LINESPECIALINFO_UNAVAIL                 0x00000010

#define LINETERMDEV_PHONE                       0x00000001
#define LINETERMDEV_HEADSET                     0x00000002
#define LINETERMDEV_SPEAKER                     0x00000004

#define LINETERMMODE_BUTTONS                    0x00000001
#define LINETERMMODE_LAMPS                      0x00000002
#define LINETERMMODE_DISPLAY                    0x00000004
#define LINETERMMODE_RINGER                     0x00000008
#define LINETERMMODE_HOOKSWITCH                 0x00000010
#define LINETERMMODE_MEDIATOLINE                0x00000020
#define LINETERMMODE_MEDIAFROMLINE              0x00000040
#define LINETERMMODE_MEDIABIDIRECT              0x00000080

#define LINETERMSHARING_PRIVATE                 0x00000001
#define LINETERMSHARING_SHAREDEXCL              0x00000002
#define LINETERMSHARING_SHAREDCONF              0x00000004

#define LINETOLLLISTOPTION_ADD                  0x00000001
#define LINETOLLLISTOPTION_REMOVE               0x00000002

#define LINETONEMODE_CUSTOM                     0x00000001
#define LINETONEMODE_RINGBACK                   0x00000002
#define LINETONEMODE_BUSY                       0x00000004
#define LINETONEMODE_BEEP                       0x00000008
#define LINETONEMODE_BILLING                    0x00000010

#define LINETRANSFERMODE_TRANSFER               0x00000001
#define LINETRANSFERMODE_CONFERENCE             0x00000002

#define LINETRANSLATEOPTION_CARDOVERRIDE        0x00000001
#define LINETRANSLATEOPTION_CANCELCALLWAITING   0x00000002      
#define LINETRANSLATEOPTION_FORCELOCAL          0x00000004      
#define LINETRANSLATEOPTION_FORCELD             0x00000008      

#define LINETRANSLATERESULT_CANONICAL           0x00000001
#define LINETRANSLATERESULT_INTERNATIONAL       0x00000002
#define LINETRANSLATERESULT_LONGDISTANCE        0x00000004
#define LINETRANSLATERESULT_LOCAL               0x00000008
#define LINETRANSLATERESULT_INTOLLLIST          0x00000010
#define LINETRANSLATERESULT_NOTINTOLLLIST       0x00000020
#define LINETRANSLATERESULT_DIALBILLING         0x00000040
#define LINETRANSLATERESULT_DIALQUIET           0x00000080
#define LINETRANSLATERESULT_DIALDIALTONE        0x00000100
#define LINETRANSLATERESULT_DIALPROMPT          0x00000200
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINETRANSLATERESULT_VOICEDETECT         0x00000400      
#endif


#define LINE_ADDRESSSTATE                       0L
#define LINE_CALLINFO                           1L
#define LINE_CALLSTATE                          2L
#define LINE_CLOSE                              3L
#define LINE_DEVSPECIFIC                        4L
#define LINE_DEVSPECIFICFEATURE                 5L
#define LINE_GATHERDIGITS                       6L
#define LINE_GENERATE                           7L
#define LINE_LINEDEVSTATE                       8L
#define LINE_MONITORDIGITS                      9L
#define LINE_MONITORMEDIA                       10L
#define LINE_MONITORTONE                        11L
#define LINE_REPLY                              12L
#define LINE_REQUEST                            13L
#define PHONE_BUTTON                            14L
#define PHONE_CLOSE                             15L
#define PHONE_DEVSPECIFIC                       16L
#define PHONE_REPLY                             17L
#define PHONE_STATE                             18L
#define LINE_CREATE                             19L       
#define PHONE_CREATE                            20L       

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINE_AGENTSPECIFIC                      21L       
#define LINE_AGENTSTATUS                        22L       
#define LINE_APPNEWCALL                         23L       
#define LINE_PROXYREQUEST                       24L       
#define LINE_REMOVE                             25L       
#define PHONE_REMOVE                            26L       
#endif


#define INITIALIZE_NEGOTIATION                  0xFFFFFFFFL

#define LINEADDRCAPFLAGS_FWDNUMRINGS            0x00000001
#define LINEADDRCAPFLAGS_PICKUPGROUPID          0x00000002
#define LINEADDRCAPFLAGS_SECURE                 0x00000004
#define LINEADDRCAPFLAGS_BLOCKIDDEFAULT         0x00000008
#define LINEADDRCAPFLAGS_BLOCKIDOVERRIDE        0x00000010
#define LINEADDRCAPFLAGS_DIALED                 0x00000020
#define LINEADDRCAPFLAGS_ORIGOFFHOOK            0x00000040
#define LINEADDRCAPFLAGS_DESTOFFHOOK            0x00000080
#define LINEADDRCAPFLAGS_FWDCONSULT             0x00000100
#define LINEADDRCAPFLAGS_SETUPCONFNULL          0x00000200
#define LINEADDRCAPFLAGS_AUTORECONNECT          0x00000400
#define LINEADDRCAPFLAGS_COMPLETIONID           0x00000800
#define LINEADDRCAPFLAGS_TRANSFERHELD           0x00001000
#define LINEADDRCAPFLAGS_TRANSFERMAKE           0x00002000
#define LINEADDRCAPFLAGS_CONFERENCEHELD         0x00004000
#define LINEADDRCAPFLAGS_CONFERENCEMAKE         0x00008000
#define LINEADDRCAPFLAGS_PARTIALDIAL            0x00010000
#define LINEADDRCAPFLAGS_FWDSTATUSVALID         0x00020000
#define LINEADDRCAPFLAGS_FWDINTEXTADDR          0x00040000
#define LINEADDRCAPFLAGS_FWDBUSYNAADDR          0x00080000
#define LINEADDRCAPFLAGS_ACCEPTTOALERT          0x00100000
#define LINEADDRCAPFLAGS_CONFDROP               0x00200000
#define LINEADDRCAPFLAGS_PICKUPCALLWAIT         0x00400000
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEADDRCAPFLAGS_PREDICTIVEDIALER       0x00800000
#define LINEADDRCAPFLAGS_QUEUE                  0x01000000
#define LINEADDRCAPFLAGS_ROUTEPOINT             0x02000000
#define LINEADDRCAPFLAGS_HOLDMAKESNEW           0x04000000
#define LINEADDRCAPFLAGS_NOINTERNALCALLS        0x08000000
#define LINEADDRCAPFLAGS_NOEXTERNALCALLS        0x10000000
#define LINEADDRCAPFLAGS_SETCALLINGID           0x20000000
#endif

#define LINEADDRESSMODE_ADDRESSID               0x00000001
#define LINEADDRESSMODE_DIALABLEADDR            0x00000002

#define LINEADDRESSSHARING_PRIVATE              0x00000001
#define LINEADDRESSSHARING_BRIDGEDEXCL          0x00000002
#define LINEADDRESSSHARING_BRIDGEDNEW           0x00000004
#define LINEADDRESSSHARING_BRIDGEDSHARED        0x00000008
#define LINEADDRESSSHARING_MONITORED            0x00000010

#define LINEADDRESSSTATE_OTHER                  0x00000001
#define LINEADDRESSSTATE_DEVSPECIFIC            0x00000002
#define LINEADDRESSSTATE_INUSEZERO              0x00000004
#define LINEADDRESSSTATE_INUSEONE               0x00000008
#define LINEADDRESSSTATE_INUSEMANY              0x00000010
#define LINEADDRESSSTATE_NUMCALLS               0x00000020
#define LINEADDRESSSTATE_FORWARD                0x00000040
#define LINEADDRESSSTATE_TERMINALS              0x00000080
#define LINEADDRESSSTATE_CAPSCHANGE             0x00000100

#define LINEADDRFEATURE_FORWARD                 0x00000001
#define LINEADDRFEATURE_MAKECALL                0x00000002
#define LINEADDRFEATURE_PICKUP                  0x00000004
#define LINEADDRFEATURE_SETMEDIACONTROL         0x00000008
#define LINEADDRFEATURE_SETTERMINAL             0x00000010
#define LINEADDRFEATURE_SETUPCONF               0x00000020
#define LINEADDRFEATURE_UNCOMPLETECALL          0x00000040
#define LINEADDRFEATURE_UNPARK                  0x00000080
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEADDRFEATURE_PICKUPHELD              0x00000100
#define LINEADDRFEATURE_PICKUPGROUP             0x00000200
#define LINEADDRFEATURE_PICKUPDIRECT            0x00000400
#define LINEADDRFEATURE_PICKUPWAITING           0x00000800
#define LINEADDRFEATURE_FORWARDFWD              0x00001000
#define LINEADDRFEATURE_FORWARDDND              0x00002000
#endif

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEAGENTFEATURE_SETAGENTGROUP          0x00000001
#define LINEAGENTFEATURE_SETAGENTSTATE          0x00000002
#define LINEAGENTFEATURE_SETAGENTACTIVITY       0x00000004
#define LINEAGENTFEATURE_AGENTSPECIFIC          0x00000008
#define LINEAGENTFEATURE_GETAGENTACTIVITYLIST   0x00000010
#define LINEAGENTFEATURE_GETAGENTGROUP          0x00000020

#define LINEAGENTSTATE_LOGGEDOFF                0x00000001
#define LINEAGENTSTATE_NOTREADY                 0x00000002
#define LINEAGENTSTATE_READY                    0x00000004
#define LINEAGENTSTATE_BUSYACD                  0x00000008
#define LINEAGENTSTATE_BUSYINCOMING             0x00000010
#define LINEAGENTSTATE_BUSYOUTBOUND             0x00000020
#define LINEAGENTSTATE_BUSYOTHER                0x00000040
#define LINEAGENTSTATE_WORKINGAFTERCALL         0x00000080
#define LINEAGENTSTATE_UNKNOWN                  0x00000100
#define LINEAGENTSTATE_UNAVAIL                  0x00000200

#define LINEAGENTSTATUS_GROUP                   0x00000001
#define LINEAGENTSTATUS_STATE                   0x00000002
#define LINEAGENTSTATUS_NEXTSTATE               0x00000004
#define LINEAGENTSTATUS_ACTIVITY                0x00000008
#define LINEAGENTSTATUS_ACTIVITYLIST            0x00000010
#define LINEAGENTSTATUS_GROUPLIST               0x00000020
#define LINEAGENTSTATUS_CAPSCHANGE              0x00000040
#define LINEAGENTSTATUS_VALIDSTATES             0x00000080
#define LINEAGENTSTATUS_VALIDNEXTSTATES         0x00000100
#endif


#define LINEANSWERMODE_NONE                     0x00000001
#define LINEANSWERMODE_DROP                     0x00000002
#define LINEANSWERMODE_HOLD                     0x00000004

#define LINEFEATURE_DEVSPECIFIC                 0x00000001
#define LINEFEATURE_DEVSPECIFICFEAT             0x00000002
#define LINEFEATURE_FORWARD                     0x00000004
#define LINEFEATURE_MAKECALL                    0x00000008
#define LINEFEATURE_SETMEDIACONTROL             0x00000010
#define LINEFEATURE_SETTERMINAL                 0x00000020
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEFEATURE_SETDEVSTATUS                0x00000040
#define LINEFEATURE_FORWARDFWD                  0x00000080
#define LINEFEATURE_FORWARDDND                  0x00000100
#endif

#define LINEBEARERMODE_VOICE                    0x00000001
#define LINEBEARERMODE_SPEECH                   0x00000002
#define LINEBEARERMODE_MULTIUSE                 0x00000004
#define LINEBEARERMODE_DATA                     0x00000008
#define LINEBEARERMODE_ALTSPEECHDATA            0x00000010
#define LINEBEARERMODE_NONCALLSIGNALING         0x00000020
#define LINEBEARERMODE_PASSTHROUGH              0x00000040
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEBEARERMODE_RESTRICTEDDATA           0x00000080
#endif

#define LINEBUSYMODE_STATION                    0x00000001
#define LINEBUSYMODE_TRUNK                      0x00000002
#define LINEBUSYMODE_UNKNOWN                    0x00000004
#define LINEBUSYMODE_UNAVAIL                    0x00000008

#define LINECALLCOMPLCOND_BUSY                  0x00000001
#define LINECALLCOMPLCOND_NOANSWER              0x00000002

#define LINECALLCOMPLMODE_CAMPON                0x00000001
#define LINECALLCOMPLMODE_CALLBACK              0x00000002
#define LINECALLCOMPLMODE_INTRUDE               0x00000004
#define LINECALLCOMPLMODE_MESSAGE               0x00000008

#define LINECALLFEATURE_ACCEPT                  0x00000001
#define LINECALLFEATURE_ADDTOCONF               0x00000002
#define LINECALLFEATURE_ANSWER                  0x00000004
#define LINECALLFEATURE_BLINDTRANSFER           0x00000008
#define LINECALLFEATURE_COMPLETECALL            0x00000010
#define LINECALLFEATURE_COMPLETETRANSF          0x00000020
#define LINECALLFEATURE_DIAL                    0x00000040
#define LINECALLFEATURE_DROP                    0x00000080
#define LINECALLFEATURE_GATHERDIGITS            0x00000100
#define LINECALLFEATURE_GENERATEDIGITS          0x00000200
#define LINECALLFEATURE_GENERATETONE            0x00000400
#define LINECALLFEATURE_HOLD                    0x00000800
#define LINECALLFEATURE_MONITORDIGITS           0x00001000
#define LINECALLFEATURE_MONITORMEDIA            0x00002000
#define LINECALLFEATURE_MONITORTONES            0x00004000
#define LINECALLFEATURE_PARK                    0x00008000
#define LINECALLFEATURE_PREPAREADDCONF          0x00010000
#define LINECALLFEATURE_REDIRECT                0x00020000
#define LINECALLFEATURE_REMOVEFROMCONF          0x00040000
#define LINECALLFEATURE_SECURECALL              0x00080000
#define LINECALLFEATURE_SENDUSERUSER            0x00100000
#define LINECALLFEATURE_SETCALLPARAMS           0x00200000
#define LINECALLFEATURE_SETMEDIACONTROL         0x00400000
#define LINECALLFEATURE_SETTERMINAL             0x00800000
#define LINECALLFEATURE_SETUPCONF               0x01000000
#define LINECALLFEATURE_SETUPTRANSFER           0x02000000
#define LINECALLFEATURE_SWAPHOLD                0x04000000
#define LINECALLFEATURE_UNHOLD                  0x08000000
#define LINECALLFEATURE_RELEASEUSERUSERINFO     0x10000000
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLFEATURE_SETTREATMENT            0x20000000
#define LINECALLFEATURE_SETQOS                  0x40000000
#define LINECALLFEATURE_SETCALLDATA             0x80000000
#endif

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLFEATURE2_NOHOLDCONFERENCE       0x00000001
#define LINECALLFEATURE2_ONESTEPTRANSFER        0x00000002
#define LINECALLFEATURE2_COMPLCAMPON            0x00000004
#define LINECALLFEATURE2_COMPLCALLBACK          0x00000008
#define LINECALLFEATURE2_COMPLINTRUDE           0x00000010
#define LINECALLFEATURE2_COMPLMESSAGE           0x00000020
#define LINECALLFEATURE2_TRANSFERNORM           0x00000040
#define LINECALLFEATURE2_TRANSFERCONF           0x00000080
#define LINECALLFEATURE2_PARKDIRECT             0x00000100
#define LINECALLFEATURE2_PARKNONDIRECT          0x00000200
#endif

#define LINECALLINFOSTATE_OTHER                 0x00000001
#define LINECALLINFOSTATE_DEVSPECIFIC           0x00000002
#define LINECALLINFOSTATE_BEARERMODE            0x00000004
#define LINECALLINFOSTATE_RATE                  0x00000008
#define LINECALLINFOSTATE_MEDIAMODE             0x00000010
#define LINECALLINFOSTATE_APPSPECIFIC           0x00000020
#define LINECALLINFOSTATE_CALLID                0x00000040
#define LINECALLINFOSTATE_RELATEDCALLID         0x00000080
#define LINECALLINFOSTATE_ORIGIN                0x00000100
#define LINECALLINFOSTATE_REASON                0x00000200
#define LINECALLINFOSTATE_COMPLETIONID          0x00000400
#define LINECALLINFOSTATE_NUMOWNERINCR          0x00000800
#define LINECALLINFOSTATE_NUMOWNERDECR          0x00001000
#define LINECALLINFOSTATE_NUMMONITORS           0x00002000
#define LINECALLINFOSTATE_TRUNK                 0x00004000
#define LINECALLINFOSTATE_CALLERID              0x00008000
#define LINECALLINFOSTATE_CALLEDID              0x00010000
#define LINECALLINFOSTATE_CONNECTEDID           0x00020000
#define LINECALLINFOSTATE_REDIRECTIONID         0x00040000
#define LINECALLINFOSTATE_REDIRECTINGID         0x00080000
#define LINECALLINFOSTATE_DISPLAY               0x00100000
#define LINECALLINFOSTATE_USERUSERINFO          0x00200000
#define LINECALLINFOSTATE_HIGHLEVELCOMP         0x00400000
#define LINECALLINFOSTATE_LOWLEVELCOMP          0x00800000
#define LINECALLINFOSTATE_CHARGINGINFO          0x01000000
#define LINECALLINFOSTATE_TERMINAL              0x02000000
#define LINECALLINFOSTATE_DIALPARAMS            0x04000000
#define LINECALLINFOSTATE_MONITORMODES          0x08000000
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLINFOSTATE_TREATMENT             0x10000000
#define LINECALLINFOSTATE_QOS                   0x20000000
#define LINECALLINFOSTATE_CALLDATA              0x40000000
#endif

#define LINECALLORIGIN_OUTBOUND                 0x00000001
#define LINECALLORIGIN_INTERNAL                 0x00000002
#define LINECALLORIGIN_EXTERNAL                 0x00000004
#define LINECALLORIGIN_UNKNOWN                  0x00000010
#define LINECALLORIGIN_UNAVAIL                  0x00000020
#define LINECALLORIGIN_CONFERENCE               0x00000040
#define LINECALLORIGIN_INBOUND                  0x00000080

#define LINECALLPARAMFLAGS_SECURE               0x00000001
#define LINECALLPARAMFLAGS_IDLE                 0x00000002
#define LINECALLPARAMFLAGS_BLOCKID              0x00000004
#define LINECALLPARAMFLAGS_ORIGOFFHOOK          0x00000008
#define LINECALLPARAMFLAGS_DESTOFFHOOK          0x00000010
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLPARAMFLAGS_NOHOLDCONFERENCE     0x00000020
#define LINECALLPARAMFLAGS_PREDICTIVEDIAL       0x00000040
#define LINECALLPARAMFLAGS_ONESTEPTRANSFER      0x00000080
#endif

#define LINECALLPARTYID_BLOCKED                 0x00000001
#define LINECALLPARTYID_OUTOFAREA               0x00000002
#define LINECALLPARTYID_NAME                    0x00000004
#define LINECALLPARTYID_ADDRESS                 0x00000008
#define LINECALLPARTYID_PARTIAL                 0x00000010
#define LINECALLPARTYID_UNKNOWN                 0x00000020
#define LINECALLPARTYID_UNAVAIL                 0x00000040

#define LINECALLPRIVILEGE_NONE                  0x00000001
#define LINECALLPRIVILEGE_MONITOR               0x00000002
#define LINECALLPRIVILEGE_OWNER                 0x00000004

#define LINECALLREASON_DIRECT                   0x00000001
#define LINECALLREASON_FWDBUSY                  0x00000002
#define LINECALLREASON_FWDNOANSWER              0x00000004
#define LINECALLREASON_FWDUNCOND                0x00000008
#define LINECALLREASON_PICKUP                   0x00000010
#define LINECALLREASON_UNPARK                   0x00000020
#define LINECALLREASON_REDIRECT                 0x00000040
#define LINECALLREASON_CALLCOMPLETION           0x00000080
#define LINECALLREASON_TRANSFER                 0x00000100
#define LINECALLREASON_REMINDER                 0x00000200
#define LINECALLREASON_UNKNOWN                  0x00000400
#define LINECALLREASON_UNAVAIL                  0x00000800
#define LINECALLREASON_INTRUDE                  0x00001000
#define LINECALLREASON_PARKED                   0x00002000
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLREASON_CAMPEDON                 0x00004000
#define LINECALLREASON_ROUTEREQUEST             0x00008000
#endif

#define LINECALLSELECT_LINE                     0x00000001
#define LINECALLSELECT_ADDRESS                  0x00000002
#define LINECALLSELECT_CALL                     0x00000004

#define LINECALLSTATE_IDLE                      0x00000001
#define LINECALLSTATE_OFFERING                  0x00000002
#define LINECALLSTATE_ACCEPTED                  0x00000004
#define LINECALLSTATE_DIALTONE                  0x00000008
#define LINECALLSTATE_DIALING                   0x00000010
#define LINECALLSTATE_RINGBACK                  0x00000020
#define LINECALLSTATE_BUSY                      0x00000040
#define LINECALLSTATE_SPECIALINFO               0x00000080
#define LINECALLSTATE_CONNECTED                 0x00000100
#define LINECALLSTATE_PROCEEDING                0x00000200
#define LINECALLSTATE_ONHOLD                    0x00000400
#define LINECALLSTATE_CONFERENCED               0x00000800
#define LINECALLSTATE_ONHOLDPENDCONF            0x00001000
#define LINECALLSTATE_ONHOLDPENDTRANSFER        0x00002000
#define LINECALLSTATE_DISCONNECTED              0x00004000
#define LINECALLSTATE_UNKNOWN                   0x00008000

#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECALLTREATMENT_SILENCE               0x00000001
#define LINECALLTREATMENT_RINGBACK              0x00000002
#define LINECALLTREATMENT_BUSY                  0x00000003
#define LINECALLTREATMENT_MUSIC                 0x00000004
#endif

#define LINECARDOPTION_PREDEFINED               0x00000001
#define LINECARDOPTION_HIDDEN                   0x00000002

#define LINECONNECTEDMODE_ACTIVE                0x00000001
#define LINECONNECTEDMODE_INACTIVE              0x00000002
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINECONNECTEDMODE_ACTIVEHELD            0x00000004
#define LINECONNECTEDMODE_INACTIVEHELD          0x00000008
#define LINECONNECTEDMODE_CONFIRMED             0x00000010
#endif

#define LINEDEVCAPFLAGS_CROSSADDRCONF           0x00000001
#define LINEDEVCAPFLAGS_HIGHLEVCOMP             0x00000002
#define LINEDEVCAPFLAGS_LOWLEVCOMP              0x00000004
#define LINEDEVCAPFLAGS_MEDIACONTROL            0x00000008
#define LINEDEVCAPFLAGS_MULTIPLEADDR            0x00000010
#define LINEDEVCAPFLAGS_CLOSEDROP               0x00000020
#define LINEDEVCAPFLAGS_DIALBILLING             0x00000040
#define LINEDEVCAPFLAGS_DIALQUIET               0x00000080
#define LINEDEVCAPFLAGS_DIALDIALTONE            0x00000100

#define LINEDEVSTATE_OTHER                      0x00000001
#define LINEDEVSTATE_RINGING                    0x00000002
#define LINEDEVSTATE_CONNECTED                  0x00000004
#define LINEDEVSTATE_DISCONNECTED               0x00000008
#define LINEDEVSTATE_MSGWAITON                  0x00000010
#define LINEDEVSTATE_MSGWAITOFF                 0x00000020
#define LINEDEVSTATE_INSERVICE                  0x00000040
#define LINEDEVSTATE_OUTOFSERVICE               0x00000080
#define LINEDEVSTATE_MAINTENANCE                0x00000100
#define LINEDEVSTATE_OPEN                       0x00000200
#define LINEDEVSTATE_CLOSE                      0x00000400
#define LINEDEVSTATE_NUMCALLS                   0x00000800
#define LINEDEVSTATE_NUMCOMPLETIONS             0x00001000
#define LINEDEVSTATE_TERMINALS                  0x00002000
#define LINEDEVSTATE_ROAMMODE                   0x00004000
#define LINEDEVSTATE_BATTERY                    0x00008000
#define LINEDEVSTATE_SIGNAL                     0x00010000
#define LINEDEVSTATE_DEVSPECIFIC                0x00020000
#define LINEDEVSTATE_REINIT                     0x00040000
#define LINEDEVSTATE_LOCK                       0x00080000
#define LINEDEVSTATE_CAPSCHANGE                 0x00100000
#define LINEDEVSTATE_CONFIGCHANGE               0x00200000
#define LINEDEVSTATE_TRANSLATECHANGE            0x00400000
#define LINEDEVSTATE_COMPLCANCEL                0x00800000
#define LINEDEVSTATE_REMOVED                    0x01000000

#define LINEDEVSTATUSFLAGS_CONNECTED            0x00000001
#define LINEDEVSTATUSFLAGS_MSGWAIT              0x00000002
#define LINEDEVSTATUSFLAGS_INSERVICE            0x00000004
#define LINEDEVSTATUSFLAGS_LOCKED               0x00000008

#define LINEDIALTONEMODE_NORMAL                 0x00000001
#define LINEDIALTONEMODE_SPECIAL                0x00000002
#define LINEDIALTONEMODE_INTERNAL               0x00000004
#define LINEDIALTONEMODE_EXTERNAL               0x00000008
#define LINEDIALTONEMODE_UNKNOWN                0x00000010
#define LINEDIALTONEMODE_UNAVAIL                0x00000020
    
#define LINEDIGITMODE_PULSE                     0x00000001
#define LINEDIGITMODE_DTMF                      0x00000002
#define LINEDIGITMODE_DTMFEND                   0x00000004
    
#define LINEDISCONNECTMODE_NORMAL               0x00000001
#define LINEDISCONNECTMODE_UNKNOWN              0x00000002
#define LINEDISCONNECTMODE_REJECT               0x00000004
#define LINEDISCONNECTMODE_PICKUP               0x00000008
#define LINEDISCONNECTMODE_FORWARDED            0x00000010
#define LINEDISCONNECTMODE_BUSY                 0x00000020
#define LINEDISCONNECTMODE_NOANSWER             0x00000040
#define LINEDISCONNECTMODE_BADADDRESS           0x00000080
#define LINEDISCONNECTMODE_UNREACHABLE          0x00000100
#define LINEDISCONNECTMODE_CONGESTION           0x00000200
#define LINEDISCONNECTMODE_INCOMPATIBLE         0x00000400
#define LINEDISCONNECTMODE_UNAVAIL              0x00000800
#define LINEDISCONNECTMODE_NODIALTONE           0x00001000
#if (TAPI_CURRENT_VERSION >= 0x00020000)
#define LINEDISCONNECTMODE_NUMBERCHANGED        0x00002000
#define LINEDISCONNECTMODE_OUTOFORDER           0x00004000
#define LINEDISCONNECTMODE_TEMPFAILURE          0x00008000
#define LINEDISCONNECTMODE_QOSUNAVAIL           0x00010000
#define LINEDISCONNECTMODE_BLOCKED              0x00020000
#define LINEDISCONNECTMODE_DONOTDISTURB         0x00040000
#endif

#define STRINGFORMAT_ASCII                      0x00000001
#define STRINGFORMAT_DBCS                       0x00000002
#define STRINGFORMAT_UNICODE                    0x00000003
#define STRINGFORMAT_BINARY                     0x00000004

#ifdef __cplusplus
} /* extern "C" */
#endif /* defined(__cplusplus) */

#endif /* __WINE_TAPI_H */
