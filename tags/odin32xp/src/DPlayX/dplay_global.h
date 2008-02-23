// $Id: dplay_global.h,v 1.3 2001-03-13 23:13:28 hugh Exp $
#ifndef __WINE_DPLAY_GLOBAL_INCLUDED
#define __WINE_DPLAY_GLOBAL_INCLUDED

#include "dplaysp.h"
#include "dplayx_queue.h"

extern HRESULT DPL_EnumAddress( LPDPENUMADDRESSCALLBACK lpEnumAddressCallback,
                                LPCVOID lpAddress, DWORD dwAddressSize,
                                LPVOID lpContext );

extern DWORD DP_CalcSessionDescSize( LPCDPSESSIONDESC2 lpSessDesc, BOOL bAnsi );

/*****************************************************************************
 * Predeclare the interface implementation structures
 */
typedef struct IDirectPlay2Impl IDirectPlay2AImpl;
typedef struct IDirectPlay2Impl IDirectPlay2Impl;
typedef struct IDirectPlay3Impl IDirectPlay3AImpl;
typedef struct IDirectPlay3Impl IDirectPlay3Impl;
typedef struct IDirectPlay4Impl IDirectPlay4AImpl;
typedef struct IDirectPlay4Impl IDirectPlay4Impl;

typedef struct tagDirectPlayIUnknownData
{
  ULONG             ulObjRef;
  CRITICAL_SECTION  DP_lock;
} DirectPlayIUnknownData;

typedef struct tagEnumSessionAsyncCallbackData
{
  LPSPINITDATA lpSpData;
  GUID         requestGuid;
  DWORD        dwEnumSessionFlags;
  DWORD        dwTimeout;
  HANDLE       hSuicideRequest;
} EnumSessionAsyncCallbackData;

typedef struct tagDP_MSG_REPLY_STRUCT
{
  HANDLE hReceipt;
  WORD   wExpectedReply;
  LPVOID lpReplyMsg;
  DWORD  dwMsgBodySize;
  /* FIXME: Is the message header required as well? */
} DP_MSG_REPLY_STRUCT, *LPDP_MSG_REPLY_STRUCT;

typedef struct tagDP_MSG_REPLY_STRUCT_LIST
{
  DPQ_ENTRY(tagDP_MSG_REPLY_STRUCT_LIST) replysExpected;
  DP_MSG_REPLY_STRUCT replyExpected;
} DP_MSG_REPLY_STRUCT_LIST, *LPDP_MSG_REPLY_STRUCT_LIST;

struct PlayerData
{
  /* Individual player information */
  DPID dpid;

  DPNAME name;
  HANDLE hEvent;

  ULONG  uRef;  /* What is the reference count on this data? */

  /* View of local data */
  LPVOID lpLocalData;
  DWORD  dwLocalDataSize;

  /* View of remote data */
  LPVOID lpRemoteData;
  DWORD  dwRemoteDataSize;

  /* SP data on a per player basis */
  LPVOID lpSPPlayerData;

  DWORD  dwFlags; /* Special remarks about the type of player */
};
typedef struct PlayerData* lpPlayerData;

struct PlayerList
{
  DPQ_ENTRY(PlayerList) players;

  lpPlayerData lpPData;
};
typedef struct PlayerList* lpPlayerList;

struct GroupData
{
  /* Internal information */
  DPID parent; /* If parent == 0 it's a top level group */

  ULONG uRef; /* Reference count */

  DPQ_HEAD(GroupList)  groups;  /* A group has [0..n] groups */
  DPQ_HEAD(PlayerList) players; /* A group has [0..n] players */

  DPID idGroupOwner; /* ID of player who owns the group */

  DWORD dwFlags; /* Flags describing anything special about the group */

  DPID   dpid;
  DPNAME name;

  /* View of local data */
  LPVOID lpLocalData;
  DWORD  dwLocalDataSize;

  /* View of remote data */
  LPVOID lpRemoteData;
  DWORD  dwRemoteDataSize;
};
typedef struct GroupData  GroupData;
typedef struct GroupData* lpGroupData;

struct GroupList
{
  DPQ_ENTRY(GroupList) groups;

  lpGroupData lpGData;
};
typedef struct GroupList* lpGroupList;

struct DPMSG
{
  DPQ_ENTRY( DPMSG ) msgs;
  DPMSG_GENERIC* msg;
};
typedef struct DPMSG* LPDPMSG;

/* Contains all dp1 and dp2 data members */
typedef struct tagDirectPlay2Data
{
  BOOL   bConnectionOpen;

  /* For async EnumSessions requests */
  HANDLE hEnumSessionThread;
  HANDLE hKillEnumSessionThreadEvent;

  LPVOID lpNameServerData; /* DPlay interface doesn't know contents */

  BOOL bHostInterface; /* Did this interface create the session */

  lpGroupData lpSysGroup; /* System group with _everything_ in it */

  LPDPSESSIONDESC2 lpSessionDesc;

  /* I/O Msg queues */
  DPQ_HEAD( DPMSG ) receiveMsgs; /* Msg receive queue */
  DPQ_HEAD( DPMSG ) sendMsgs;    /* Msg send pending queue */

  /* Information about the service provider active on this connection */
  SPINITDATA spData;

  /* Our service provider */
  HMODULE hServiceProvider;

  BOOL bConnectionInitialized;

  /* Expected messages queue */
  DPQ_HEAD( tagDP_MSG_REPLY_STRUCT_LIST ) replysExpected;
} DirectPlay2Data;

typedef struct tagDirectPlay3Data
{
  BOOL dummy;
} DirectPlay3Data;
typedef struct tagDirectPlay4Data
{
  BOOL dummy;
} DirectPlay4Data;

#define DP_IMPL_FIELDS \
  ULONG ulInterfaceRef; \
  DirectPlayIUnknownData*  unk; \
  DirectPlay2Data*         dp2; \
  DirectPlay3Data*         dp3; \
  DirectPlay4Data*         dp4;

struct IDirectPlay2Impl
{
  ICOM_VFIELD(IDirectPlay2);
  DP_IMPL_FIELDS
};

struct IDirectPlay3Impl
{
  ICOM_VFIELD(IDirectPlay3);
  DP_IMPL_FIELDS
};

struct IDirectPlay4Impl
{
  ICOM_VFIELD(IDirectPlay4);
  DP_IMPL_FIELDS
};

/* Forward declarations of virtual tables */
extern ICOM_VTABLE(IDirectPlay2) directPlay2AVT;
extern ICOM_VTABLE(IDirectPlay3) directPlay3AVT;
extern ICOM_VTABLE(IDirectPlay4) directPlay4AVT;

extern ICOM_VTABLE(IDirectPlay2) directPlay2WVT;
extern ICOM_VTABLE(IDirectPlay3) directPlay3WVT;
extern ICOM_VTABLE(IDirectPlay4) directPlay4WVT;


HRESULT DP_HandleMessage( IDirectPlay2Impl* This, LPCVOID lpMessageBody,
                          DWORD  dwMessageBodySize, LPCVOID lpMessageHeader,
                          WORD wCommandId, WORD wVersion,
                          LPVOID* lplpReply, LPDWORD lpdwMsgSize );

/* DP SP external interfaces into DirectPlay */
extern HRESULT DP_GetSPPlayerData( IDirectPlay2Impl* lpDP, DPID idPlayer, LPVOID* lplpData );
extern HRESULT DP_SetSPPlayerData( IDirectPlay2Impl* lpDP, DPID idPlayer, LPVOID lpData );

/* DP external interfaces to call into DPSP interface */
extern LPVOID DPSP_CreateSPPlayerData(void);

#endif /* __WINE_DPLAY_GLOBAL_INCLUDED */
