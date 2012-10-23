// $Id: dplaysp.h,v 1.3 2001-03-13 23:13:28 hugh Exp $
#ifndef __WINE_DIRECT_PLAY_SP_H
#define __WINE_DIRECT_PLAY_SP_H

#include "dplay.h"
#include "dplobby.h"

/* GUID for IDirectPlaySP  {0C9F6360-CC61-11cf-ACEC-00AA006886E3} */
DEFINE_GUID(IID_IDirectPlaySP, 0xc9f6360, 0xcc61, 0x11cf, 0xac, 0xec, 0x0, 0xaa, 0x0, 0x68, 0x86, 0xe3);
typedef struct IDirectPlaySP IDirectPlaySP, *LPDIRECTPLAYSP;


typedef BOOL (CALLBACK* LPENUMMRUCALLBACK)( LPCVOID lpData,
                                            DWORD  dwDataSize,
                                            LPVOID lpContext );

/* For SP. Top 16 bits is dplay, bottom 16 is SP */
#define DPSP_MAJORVERSION 0x00060000
#define DPSP_DX5VERSION   0x00050000
#define DPSP_DX3VERSION   0x00040000

#define DPSP_MAJORVERSIONMASK 0xFFFF0000
#define DPSP_MINORVERSIONMASK 0x0000FFFF


/* Some flags */
#define DPLAYI_PLAYER_SYSPLAYER      0x00000001
#define DPLAYI_PLAYER_NAMESRVR       0x00000002
#define DPLAYI_PLAYER_PLAYERINGROUP  0x00000004
#define DPLAYI_PLAYER_PLAYERLOCAL    0x00000008
#define DPLAYI_GROUP_SYSGROUP        0x00000020
#define DPLAYI_GROUP_DPLAYOWNS       0x00000040
#define DPLAYI_PLAYER_APPSERVER      0x00000080
#define DPLAYI_GROUP_HIDDEN          0x00000400

/* Define the COM interface */
#define ICOM_INTERFACE IDirectPlaySP
#define IDirectPlaySP_METHODS \
   ICOM_METHOD5(HRESULT,AddMRUEntry, LPCWSTR,lpSection, LPCWSTR,lpKey, LPCVOID,lpData, DWORD,dwDataSize, DWORD,dwMaxEntries ) \
   ICOM_METHOD6(HRESULT,CreateAddress, REFGUID,guidSP, REFGUID,guidDataType, LPCVOID,lpData, DWORD,dwDataSize, LPVOID,lpAddress,LPDWORD,lpdwAddressSize) \
   ICOM_METHOD4(HRESULT,EnumAddress, LPDPENUMADDRESSCALLBACK,lpEnumAddressCallback, LPCVOID,lpAddress, DWORD,dwAddressSize, LPVOID,lpContext ) \
   ICOM_METHOD4(HRESULT,EnumMRUEntries, LPCWSTR,lpSection, LPCWSTR,lpKey, LPENUMMRUCALLBACK,lpEnumMRUCallback, LPVOID,lpContext ) \
   ICOM_METHOD2(HRESULT,GetPlayerFlags,       DPID,idPlayer, LPDWORD,lpdwPlayerFlags ) \
   ICOM_METHOD4(HRESULT,GetSPPlayerData,      DPID,idPlayer, LPVOID*,lplpData, LPDWORD,lpdwDataSize, DWORD,dwFlags ) \
   ICOM_METHOD3(HRESULT,HandleMessage,        LPVOID,lpMessageBody, DWORD,dwMessageBodySize, LPVOID,lpMessageHeader ) \
   ICOM_METHOD4(HRESULT,SetSPPlayerData,      DPID,idPlayer, LPVOID,lpData, DWORD,dwDataSize, DWORD,dwFlags ) \
   ICOM_METHOD4(HRESULT,CreateCompoundAddress, LPCDPCOMPOUNDADDRESSELEMENT,lpElements, DWORD,dwElementCount, LPVOID,lpAddress, LPDWORD,lpdwAddressSize ) \
   ICOM_METHOD3(HRESULT,GetSPData,                LPVOID*,lplpData, LPDWORD,dwDataSize, DWORD,dwFlags ) \
   ICOM_METHOD3(HRESULT,SetSPData,                LPVOID,lpData, DWORD,dwDataSize, DWORD,dwFlags ) \
   ICOM_METHOD2(VOID,SendComplete,              LPVOID,arg1, DWORD,arg2 )

#define IDirectPlaySP_IMETHODS \
   IUnknown_IMETHODS \
   IDirectPlaySP_METHODS

ICOM_DEFINE(IDirectPlaySP,IUnknown)
#undef ICOM_INTERFACE

/* NOTE: The microsoft provided header file doesn't have these access
 * functions
 */
/*** IUnknown methods ***/
#define IDirectPlaySP_QueryInterface(p,a,b)        ICOM_CALL2(QueryInterface,p,a,b)
#define IDirectPlaySP_AddRef(p)                    ICOM_CALL (AddRef,p)
#define IDirectPlaySP_Release(p)                   ICOM_CALL (Release,p)
/*** IDirectPlaySP methods ***/
#define IDirectPlaySP_AddMRUEntry(p,a,b,c,d,e)     ICOM_CALL5(AddMRUEntry,p,a,b,c,d,e)
#define IDirectPlaySP_CreateAddress(p,a,b,c,d,e,f) ICOM_CALL6(CreateAddress,p,a,b,c,d,e,f)
#define IDirectPlaySP_EnumAddress(p,a,b,c,d)       ICOM_CALL4(EnumAddress,p,a,b,c,d)
#define IDirectPlaySP_EnumMRUEntries(p,a,b,c,d)    ICOM_CALL4(EnumMRUEntries,p,a,b,c,d)
#define IDirectPlaySP_GetPlayerFlags(p,a,b)        ICOM_CALL2(GetPlayerFlags,p,a,b)
#define IDirectPlaySP_GetSPPlayerData(p,a,b,c,d)   ICOM_CALL4(GetSPPlayerData,p,a,b,c,d)
#define IDirectPlaySP_HandleMessage(p,a,b,c)       ICOM_CALL3(HandleMessage,p,a,b,c)
#define IDirectPlaySP_SetSPPlayerData(p,a,b,c,d)   ICOM_CALL4(SetSPPlayerData,p,a,b,c,d)
#define IDirectPlaySP_CreateCompoundAddress(p,a,b,c,d)  ICOM_CALL4(CreateCompoundAddress,p,a,b,c,d)
#define IDirectPlaySP_GetSPData(p,a,b,c)           ICOM_CALL3(GetSPData,p,a,b,c)
#define IDirectPlaySP_SetSPData(p,a,b,c)           ICOM_CALL3(SetSPData,p,a,b,c)
#define IDirectPlaySP_SendComplete(p,a,b)          ICOM_CALL2(SendComplete,p,a,b)

/* SP Callback stuff */

typedef struct tagDPSP_ADDPLAYERTOGROUPDATA
{
  DPID           idPlayer;
  DPID           idGroup;
  IDirectPlaySP* lpISP;
} DPSP_ADDPLAYERTOGROUPDATA, *LPDPSP_ADDPLAYERTOGROUPDATA;

typedef struct tagDPSP_CLOSEDATA
{
  IDirectPlaySP* lpISP;
} DPSP_CLOSEDATA, *LPDPSP_CLOSEDATA;

typedef struct tagDPSP_CREATEGROUPDATA
{
  DPID           idGroup;
  DWORD          dwFlags;
  LPVOID         lpSPMessageHeader;
  IDirectPlaySP* lpISP;
} DPSP_CREATEGROUPDATA, *LPDPSP_CREATEGROUPDATA;

typedef struct tagDPSP_CREATEPLAYERDATA
{
  DPID           idPlayer;
  DWORD          dwFlags;
  LPVOID         lpSPMessageHeader;
  IDirectPlaySP* lpISP;
} DPSP_CREATEPLAYERDATA, *LPDPSP_CREATEPLAYERDATA;

typedef struct tagDPSP_DELETEGROUPDATA
{
  DPID           idGroup;
  DWORD          dwFlags;
  IDirectPlaySP* lpISP;
} DPSP_DELETEGROUPDATA, *LPDPSP_DELETEGROUPDATA;

typedef struct tagDPSP_DELETEPLAYERDATA
{
  DPID           idPlayer;
  DWORD          dwFlags;
  IDirectPlaySP* lpISP;
} DPSP_DELETEPLAYERDATA, *LPDPSP_DELETEPLAYERDATA;

typedef struct tagDPSP_ENUMSESSIONSDATA
{
  LPVOID         lpMessage;
  DWORD          dwMessageSize;
  IDirectPlaySP* lpISP;
  BOOL           bReturnStatus;
} DPSP_ENUMSESSIONSDATA, *LPDPSP_ENUMSESSIONSDATA;

typedef struct _DPSP_GETADDRESSDATA
{
  DPID           idPlayer;
  DWORD          dwFlags;
  LPDPADDRESS    lpAddress;
  LPDWORD        lpdwAddressSize;
  IDirectPlaySP* lpISP;
} DPSP_GETADDRESSDATA, *LPDPSP_GETADDRESSDATA;

typedef struct tagDPSP_GETADDRESSCHOICESDATA
{
  LPDPADDRESS    lpAddress;
  LPDWORD        lpdwAddressSize;
  IDirectPlaySP* lpISP;
} DPSP_GETADDRESSCHOICESDATA, *LPDPSP_GETADDRESSCHOICESDATA;

typedef struct tagDPSP_GETCAPSDATA
{
  DPID           idPlayer;
  LPDPCAPS       lpCaps;
  DWORD          dwFlags;
  IDirectPlaySP* lpISP;
} DPSP_GETCAPSDATA, *LPDPSP_GETCAPSDATA;

typedef struct tagDPSP_OPENDATA
{
  BOOL           bCreate;
  LPVOID         lpSPMessageHeader;
  IDirectPlaySP* lpISP;
  BOOL           bReturnStatus;
  DWORD          dwOpenFlags;
  DWORD          dwSessionFlags;
} DPSP_OPENDATA, *LPDPSP_OPENDATA;

typedef struct tagDPSP_REMOVEPLAYERFROMGROUPDATA
{
  DPID           idPlayer;
  DPID           idGroup;
  IDirectPlaySP* lpISP;
} DPSP_REMOVEPLAYERFROMGROUPDATA, *LPDPSP_REMOVEPLAYERFROMGROUPDATA;

typedef struct tagDPSP_REPLYDATA
{
  LPVOID         lpSPMessageHeader;
  LPVOID         lpMessage;
  DWORD          dwMessageSize;
  DPID           idNameServer;
  IDirectPlaySP* lpISP;
} DPSP_REPLYDATA, *LPDPSP_REPLYDATA;

typedef struct tagDPSP_SENDDATA
{
  DWORD          dwFlags;
  DPID           idPlayerTo;
  DPID           idPlayerFrom;
  LPVOID         lpMessage;
  DWORD          dwMessageSize;
  BOOL           bSystemMessage;
  IDirectPlaySP* lpISP;
} DPSP_SENDDATA, *LPDPSP_SENDDATA;

typedef struct tagDPSP_SENDTOGROUPDATA
{
  DWORD          dwFlags;
  DPID           idGroupTo;
  DPID           idPlayerFrom;
  LPVOID         lpMessage;
  DWORD          dwMessageSize;
  IDirectPlaySP* lpISP;
} DPSP_SENDTOGROUPDATA, *LPDPSP_SENDTOGROUPDATA;

typedef struct tagDPSP_SENDEXDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idPlayerTo;
  DPID           idPlayerFrom;
  LPSGBUFFER     lpSendBuffers;
  DWORD          cBuffers;
  DWORD          dwMessageSize;
  DWORD          dwPriority;
  DWORD          dwTimeout;
  LPVOID         lpDPContext;
  LPDWORD        lpdwSPMsgID;
  BOOL           bSystemMessage;
} DPSP_SENDEXDATA, *LPDPSP_SENDEXDATA;

typedef struct tagDPSP_SENDTOGROUPEXDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idGroupTo;
  DPID           idPlayerFrom;
  LPSGBUFFER     lpSendBuffers;
  DWORD          cBuffers;
  DWORD          dwMessageSize;
  DWORD          dwPriority;
  DWORD          dwTimeout;
  LPVOID         lpDPContext;
  LPDWORD        lpdwSPMsgID;
} DPSP_SENDTOGROUPEXDATA, *LPDPSP_SENDTOGROUPEXDATA;

typedef struct tagDPSP_GETMESSAGEQUEUEDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  DPID           idFrom;
  DPID           idTo;
  LPDWORD        lpdwNumMsgs;
  LPDWORD        lpdwNumBytes;
} DPSP_GETMESSAGEQUEUEDATA, *LPDPSP_GETMESSAGEQUEUEDATA;

#define DPCANCELSEND_PRIORITY 0x00000001
#define DPCANCELSEND_ALL      0x00000002

typedef struct tagDPSP_CANCELDATA
{
  IDirectPlaySP* lpISP;
  DWORD          dwFlags;
  LPRGLPVOID     lprglpvSPMsgID;
  DWORD          cSPMsgID;
  DWORD          dwMinPriority;
  DWORD          dwMaxPriority;
} DPSP_CANCELDATA, *LPDPSP_CANCELDATA;

typedef struct tagDPSP_SHUTDOWNDATA
{
  IDirectPlaySP* lpISP;
} DPSP_SHUTDOWNDATA, *LPDPSP_SHUTDOWNDATA;


/* Prototypes returned by SPInit */
typedef HRESULT (WINAPI *LPDPSP_CREATEPLAYER)(LPDPSP_CREATEPLAYERDATA);
typedef HRESULT (WINAPI *LPDPSP_DELETEPLAYER)(LPDPSP_DELETEPLAYERDATA);
typedef HRESULT (WINAPI *LPDPSP_SEND)(LPDPSP_SENDDATA);
typedef HRESULT (WINAPI *LPDPSP_ENUMSESSIONS)(LPDPSP_ENUMSESSIONSDATA);
typedef HRESULT (WINAPI *LPDPSP_REPLY)(LPDPSP_REPLYDATA);
typedef HRESULT (WINAPI *LPDPSP_SHUTDOWN)(void);
typedef HRESULT (WINAPI *LPDPSP_CREATEGROUP)(LPDPSP_CREATEGROUPDATA);
typedef HRESULT (WINAPI *LPDPSP_DELETEGROUP)(LPDPSP_DELETEGROUPDATA);
typedef HRESULT (WINAPI *LPDPSP_ADDPLAYERTOGROUP)(LPDPSP_ADDPLAYERTOGROUPDATA);
typedef HRESULT (WINAPI *LPDPSP_REMOVEPLAYERFROMGROUP)(LPDPSP_REMOVEPLAYERFROMGROUPDATA);
typedef HRESULT (WINAPI *LPDPSP_GETCAPS)(LPDPSP_GETCAPSDATA);
typedef HRESULT (WINAPI *LPDPSP_GETADDRESS)(LPDPSP_GETADDRESSDATA);
typedef HRESULT (WINAPI *LPDPSP_GETADDRESSCHOICES)(LPDPSP_GETADDRESSCHOICESDATA);
typedef HRESULT (WINAPI *LPDPSP_OPEN)(LPDPSP_OPENDATA);
typedef HRESULT (WINAPI *LPDPSP_CLOSE)(void);
typedef HRESULT (WINAPI *LPDPSP_SENDTOGROUP)(LPDPSP_SENDTOGROUPDATA);
typedef HRESULT (WINAPI *LPDPSP_SHUTDOWNEX)(LPDPSP_SHUTDOWNDATA);
typedef HRESULT (WINAPI *LPDPSP_CLOSEEX)(LPDPSP_CLOSEDATA);
typedef HRESULT (WINAPI *LPDPSP_SENDEX)(LPDPSP_SENDEXDATA);
typedef HRESULT (WINAPI *LPDPSP_SENDTOGROUPEX)(LPDPSP_SENDTOGROUPEXDATA);
typedef HRESULT (WINAPI *LPDPSP_CANCEL)(LPDPSP_CANCELDATA);
typedef HRESULT (WINAPI *LPDPSP_GETMESSAGEQUEUE)(LPDPSP_GETMESSAGEQUEUEDATA);


typedef struct tagDPSP_SPCALLBACKS
{
    DWORD                        dwSize;
    DWORD                        dwVersion;

    LPDPSP_ENUMSESSIONS          EnumSessions;          /* Must be provided */
    LPDPSP_REPLY                 Reply;                 /* Must be provided */
    LPDPSP_SEND                  Send;                  /* Must be provided */
    LPDPSP_ADDPLAYERTOGROUP      AddPlayerToGroup;      /* Optional */
    LPDPSP_CLOSE                 Close;                 /* Optional */
    LPDPSP_CREATEGROUP           CreateGroup;           /* Optional */
    LPDPSP_CREATEPLAYER          CreatePlayer;          /* Optional */
    LPDPSP_DELETEGROUP           DeleteGroup;           /* Optional */
    LPDPSP_DELETEPLAYER          DeletePlayer;          /* Optional */
    LPDPSP_GETADDRESS            GetAddress;            /* Optional */
    LPDPSP_GETCAPS               GetCaps;               /* Optional */
    LPDPSP_OPEN                  Open;                  /* Optional */
    LPDPSP_REMOVEPLAYERFROMGROUP RemovePlayerFromGroup; /* Optional */
    LPDPSP_SENDTOGROUP           SendToGroup;           /* Optional */
    LPDPSP_SHUTDOWN              Shutdown;              /* Optional */

    LPDPSP_CLOSEEX               CloseEx;               /* Optional */
    LPDPSP_SHUTDOWNEX            ShutdownEx;            /* Optional */
    LPDPSP_GETADDRESSCHOICES     GetAddressChoices;     /* Optional */

    LPDPSP_SENDEX                SendEx;                /* Optional */
    LPDPSP_SENDTOGROUPEX         SendToGroupEx;         /* Optional */
    LPDPSP_CANCEL                Cancel;                /* Optional */
    LPDPSP_GETMESSAGEQUEUE       GetMessageQueue;       /* Optional */
} DPSP_SPCALLBACKS, *LPDPSP_SPCALLBACKS;

typedef struct tagSPINITDATA
{
    LPDPSP_SPCALLBACKS  lpCB;
    IDirectPlaySP*      lpISP;
    LPWSTR              lpszName;
    LPGUID              lpGuid;
    DWORD               dwReserved1;
    DWORD               dwReserved2;
    DWORD               dwSPHeaderSize;
    LPDPADDRESS         lpAddress;
    DWORD               dwAddressSize;
    DWORD               dwSPVersion;
} SPINITDATA, *LPSPINITDATA;

typedef HRESULT (WINAPI *LPDPSP_SPINIT)(LPSPINITDATA);

/* This variable is exported from the DLL at ordinal 6 to be accessed by the
 * SP directly
 */
//extern __declspec(dllimport) DWORD gdwDPlaySPRefCount;
extern DWORD gdwDPlaySPRefCount;

#endif

