// $Id: dplayx_messages.cpp,v 1.4 2001-03-13 23:13:27 hugh Exp $
/* DirectPlay & DirectPlayLobby messaging implementation
 *
 * Copyright 2000 - Peter Hunnisett
 *
 * <presently under construction - contact hunnise@nortelnetworks.com>
 *
 */
#include <string.h>

#include <odin.h>
#define CINTERFACE

#include "winbase.h"
#include "debugtools.h"

#include "wingdi.h"
#include "winuser.h"
#include "winerror.h"

#include "dplayx_messages.h"
#include "dplay_global.h"
#include "dplayx_global.h"

DEFAULT_DEBUG_CHANNEL(dplay);

#undef  debugstr_guid
#define debugstr_guid(a) a

typedef struct tagMSGTHREADINFO
{
  HANDLE hStart;
  HANDLE hDeath;
  HANDLE hSettingRead;
  HANDLE hNotifyEvent;
} MSGTHREADINFO, *LPMSGTHREADINFO;


static DWORD CALLBACK DPL_MSG_ThreadMain( LPVOID lpContext );
static LPVOID DP_MSG_ExpectReply( IDirectPlay2AImpl* This, LPDPSP_SENDDATA data, 
                                  DWORD dwWaitTime, WORD wReplyCommandId,
                                  LPVOID* lplpReplyMsg, LPDWORD lpdwMsgBodySize );

/* Create the message reception thread to allow the application to receive
 * asynchronous message reception
 */
DWORD CreateLobbyMessageReceptionThread( HANDLE hNotifyEvent, HANDLE hStart,
                                         HANDLE hDeath, HANDLE hConnRead )
{
  DWORD           dwMsgThreadId;
  LPMSGTHREADINFO lpThreadInfo;

  lpThreadInfo = (LPMSGTHREADINFO) HeapAlloc( GetProcessHeap(), 0, sizeof( *lpThreadInfo ) );
  if( lpThreadInfo == NULL )
  {
    return 0;
  }

  /* The notify event may or may not exist. Depends if async comm or not */
  if( hNotifyEvent &&
      !DuplicateHandle( GetCurrentProcess(), hNotifyEvent,
                        GetCurrentProcess(), &lpThreadInfo->hNotifyEvent,
                        0, FALSE, DUPLICATE_SAME_ACCESS ) )
  {
    ERR( "Unable to duplicate event handle\n" );
    goto error;
  }

  /* These 3 handles don't need to be duplicated because we don't keep a
   * reference to them where they're created. They're created specifically
   * for the message thread
   */
  lpThreadInfo->hStart       = hStart;
  lpThreadInfo->hDeath       = hDeath;
  lpThreadInfo->hSettingRead = hConnRead;

  if( !CreateThread( NULL,                  /* Security attribs */
                     0,                     /* Stack */
                     DPL_MSG_ThreadMain,    /* Msg reception function */
                     lpThreadInfo,          /* Msg reception func parameter */
                     0,                     /* Flags */
                     &dwMsgThreadId         /* Updated with thread id */
                   )
    )
  {
    ERR( "Unable to create msg thread\n" );
    goto error;
  }

  /* FIXME: Should I be closing the handle to the thread or does that
            terminate the thread? */

  return dwMsgThreadId;

error:

  HeapFree( GetProcessHeap(), 0, lpThreadInfo );

  return 0;
}

static DWORD CALLBACK DPL_MSG_ThreadMain( LPVOID lpContext )
{
  LPMSGTHREADINFO lpThreadInfo = (LPMSGTHREADINFO)lpContext;
  DWORD dwWaitResult;

  TRACE( "Msg thread created. Waiting on app startup\n" );

  /* Wait to ensure that the lobby application is started w/ 1 min timeout */
  dwWaitResult = WaitForSingleObject( lpThreadInfo->hStart, 10000 /* 10 sec */ );
  if( dwWaitResult == WAIT_TIMEOUT )
  {
    FIXME( "Should signal app/wait creation failure (0x%08lx)\n", dwWaitResult );
    goto end_of_thread;
  }

  /* Close this handle as it's not needed anymore */
  CloseHandle( lpThreadInfo->hStart );
  lpThreadInfo->hStart = 0;

  /* Wait until the lobby knows what it is */
  dwWaitResult = WaitForSingleObject( lpThreadInfo->hSettingRead, INFINITE );
  if( dwWaitResult == WAIT_TIMEOUT )
  {
    ERR( "App Read connection setting timeout fail (0x%08lx)\n", dwWaitResult );
  }

  /* Close this handle as it's not needed anymore */
  CloseHandle( lpThreadInfo->hSettingRead );
  lpThreadInfo->hSettingRead = 0;

  TRACE( "App created && intialized starting main message reception loop\n" );

  for ( ;; )
  {
    MSG lobbyMsg;
    GetMessageW( &lobbyMsg, 0, 0, 0 );
  }

end_of_thread:
  TRACE( "Msg thread exiting!\n" );
  HeapFree( GetProcessHeap(), 0, lpThreadInfo );

  return 0;
}

/* DP messageing stuff */
static HANDLE DP_MSG_BuildAndLinkReplyStruct( IDirectPlay2Impl* This,
                                              LPDP_MSG_REPLY_STRUCT_LIST lpReplyStructList, 
                                              WORD wReplyCommandId );
static LPVOID DP_MSG_CleanReplyStruct( LPDP_MSG_REPLY_STRUCT_LIST lpReplyStructList,
                                       LPVOID* lplpReplyMsg, LPDWORD lpdwMsgBodySize );


static
HANDLE DP_MSG_BuildAndLinkReplyStruct( IDirectPlay2Impl* This,
                                       LPDP_MSG_REPLY_STRUCT_LIST lpReplyStructList, WORD wReplyCommandId )
{
  lpReplyStructList->replyExpected.hReceipt       = CreateEventA( NULL, FALSE, FALSE, NULL );
  lpReplyStructList->replyExpected.wExpectedReply = wReplyCommandId;
  lpReplyStructList->replyExpected.lpReplyMsg     = NULL;
  lpReplyStructList->replyExpected.dwMsgBodySize  = 0;

  /* Insert into the message queue while locked */
  EnterCriticalSection( &This->unk->DP_lock );
    DPQ_INSERT( This->dp2->replysExpected, lpReplyStructList, replysExpected );
  LeaveCriticalSection( &This->unk->DP_lock );

  return lpReplyStructList->replyExpected.hReceipt;
}

static
LPVOID DP_MSG_CleanReplyStruct( LPDP_MSG_REPLY_STRUCT_LIST lpReplyStructList,
                                LPVOID* lplpReplyMsg, LPDWORD lpdwMsgBodySize  )
{
  CloseHandle( lpReplyStructList->replyExpected.hReceipt );

  *lplpReplyMsg    = lpReplyStructList->replyExpected.lpReplyMsg;
  *lpdwMsgBodySize = lpReplyStructList->replyExpected.dwMsgBodySize;

  return lpReplyStructList->replyExpected.lpReplyMsg;
}

HRESULT DP_MSG_SendRequestPlayerId( IDirectPlay2AImpl* This, DWORD dwFlags,
                                    LPDPID lpdpidAllocatedId )
{
  LPVOID                     lpMsg;
  LPDPMSG_REQUESTNEWPLAYERID lpMsgBody;
  DWORD                      dwMsgSize;
  HRESULT                    hr = DP_OK;

  dwMsgSize = This->dp2->spData.dwSPHeaderSize + sizeof( *lpMsgBody );

  lpMsg = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwMsgSize );

  lpMsgBody = (LPDPMSG_REQUESTNEWPLAYERID)( (BYTE*)lpMsg +
                                             This->dp2->spData.dwSPHeaderSize );

  /* Compose dplay message envelope */
  lpMsgBody->envelope.dwMagic    = DPMSGMAGIC_DPLAYMSG;
  lpMsgBody->envelope.wCommandId = DPMSGCMD_REQUESTNEWPLAYERID;
  lpMsgBody->envelope.wVersion   = DPMSGVER_DP6;

  /* Compose the body of the message */
  lpMsgBody->dwFlags = dwFlags;

  /* Send the message */
  {
    DPSP_SENDDATA data;

    data.dwFlags        = DPSEND_GUARANTEED;
    data.idPlayerTo     = 0; /* Name server */
    data.idPlayerFrom   = 0; /* Sending from DP */
    data.lpMessage      = lpMsg;
    data.dwMessageSize  = dwMsgSize;
    data.bSystemMessage = TRUE; /* Allow reply to be sent */
    data.lpISP          = This->dp2->spData.lpISP;

    TRACE( "Asking for player id w/ dwFlags 0x%08lx\n", 
           lpMsgBody->dwFlags );


    DP_MSG_ExpectReply( This, &data, DPMSG_DEFAULT_WAIT_TIME, DPMSGCMD_NEWPLAYERIDREPLY, 
                        &lpMsg, &dwMsgSize );

  }


  /* Need to examine the data and extract the new player id */
  if( !FAILED(hr) )
  {
    LPCDPMSG_NEWPLAYERIDREPLY lpcReply;

    lpcReply = (LPCDPMSG_NEWPLAYERIDREPLY)lpMsg;

    *lpdpidAllocatedId = lpcReply->dpidNewPlayerId;

    TRACE( "Received reply for id = 0x%08lx\n", lpcReply->dpidNewPlayerId );

    /* FIXME: I think that the rest of the message has something to do
     *        with remote data for the player that perhaps I need to setup.
     *        However, with the information that is passed, all that it could
     *        be used for is a standardized intialization value, which I'm 
     *        guessing we can do without. Unless the message content is the same
     *        for several different messages?
     */

    HeapFree( GetProcessHeap(), 0, lpMsg ); 
  }

  return hr;
}

HRESULT DP_MSG_ForwardPlayerCreation( IDirectPlay2AImpl* This, DPID dpidServer )
{
  LPVOID                   lpMsg;
  LPDPMSG_FORWARDADDPLAYER lpMsgBody;
  DWORD                    dwMsgSize;
  HRESULT                  hr = DP_OK;

  dwMsgSize = This->dp2->spData.dwSPHeaderSize + sizeof( *lpMsgBody );

  lpMsg = HeapAlloc( GetProcessHeap(), HEAP_ZERO_MEMORY, dwMsgSize );

  lpMsgBody = (LPDPMSG_FORWARDADDPLAYER)( (BYTE*)lpMsg +
                                          This->dp2->spData.dwSPHeaderSize );

  /* Compose dplay message envelope */
  lpMsgBody->envelope.dwMagic    = DPMSGMAGIC_DPLAYMSG;
  lpMsgBody->envelope.wCommandId = DPMSGCMD_FORWARDADDPLAYER;
  lpMsgBody->envelope.wVersion   = DPMSGVER_DP6;

#if 0
  {
    LPBYTE lpPData;
    DWORD  dwDataSize;
    
    /* SP Player remote data needs to be propagated at some point - is this the point? */
    IDirectPlaySP_GetSPPlayerData( This->dp2->spData.lpISP, dpidServer, (LPVOID*)&lpPData, &dwDataSize, DPSET_REMOTE );
    
    ERR( "Player Data size is 0x%08lx\n"
         "[%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x]\n"
         "[%02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x %02x%02x]\n", 
 
	 dwDataSize,
         lpPData[0], lpPData[1], lpPData[2], lpPData[3], lpPData[4],
	 lpPData[5], lpPData[6], lpPData[7], lpPData[8], lpPData[9],
         lpPData[10], lpPData[11], lpPData[12], lpPData[13], lpPData[14],
	 lpPData[15], lpPData[16], lpPData[17], lpPData[18], lpPData[19],
         lpPData[20], lpPData[21], lpPData[22], lpPData[23], lpPData[24],
	 lpPData[25], lpPData[26], lpPData[27], lpPData[28], lpPData[29],
         lpPData[30], lpPData[31]	 	 
        );
    DebugBreak();
  }
#endif

  /* Compose body of message */
  lpMsgBody->dpidAppServer = dpidServer;
  lpMsgBody->unknown2[0] = 0x0;
  lpMsgBody->unknown2[1] = 0x1c;
  lpMsgBody->unknown2[2] = 0x6c;
  lpMsgBody->unknown2[3] = 0x50;
  lpMsgBody->unknown2[4] = 0x9;

  lpMsgBody->dpidAppServer2 = dpidServer;
  lpMsgBody->unknown3[0] = 0x0;
  lpMsgBody->unknown3[0] = 0x0;
  lpMsgBody->unknown3[0] = 0x20;
  lpMsgBody->unknown3[0] = 0x0;
  lpMsgBody->unknown3[0] = 0x0;
  
  lpMsgBody->dpidAppServer3 = dpidServer;
  lpMsgBody->unknown4[0] =  0x30; 
  lpMsgBody->unknown4[1] =  0xb;
  lpMsgBody->unknown4[2] =  0x0;
  lpMsgBody->unknown4[3] =  0x1e090002;
  lpMsgBody->unknown4[4] =  0x0;
  lpMsgBody->unknown4[5] =  0x0;
  lpMsgBody->unknown4[6] =  0x0;
  lpMsgBody->unknown4[7] =  0x32090002;
  lpMsgBody->unknown4[8] =  0x0;
  lpMsgBody->unknown4[9] =  0x0;
  lpMsgBody->unknown4[10] = 0x0;
  lpMsgBody->unknown4[11] = 0x0;
  lpMsgBody->unknown4[12] = 0x0;

  lpMsgBody->unknown5[0] = 0x0;
  lpMsgBody->unknown5[1] = 0x0;

  /* Send the message */
  {
    DPSP_SENDDATA data;

    data.dwFlags        = DPSEND_GUARANTEED;
    data.idPlayerTo     = 0; /* Name server */
    data.idPlayerFrom   = dpidServer; /* Sending from session server */
    data.lpMessage      = lpMsg;
    data.dwMessageSize  = dwMsgSize;
    data.bSystemMessage = TRUE; /* Allow reply to be sent */
    data.lpISP          = This->dp2->spData.lpISP;

    lpMsg = DP_MSG_ExpectReply( This, &data, 
                                DPMSG_WAIT_60_SECS, 
                                DPMSGCMD_GETNAMETABLEREPLY,
                                &lpMsg, &dwMsgSize );
  }

  /* Need to examine the data and extract the new player id */
  if( lpMsg != NULL )
  {
    FIXME( "Name Table reply received: stub\n" );
  }

  return hr;
}

/* Queue up a structure indicating that we want a reply of type wReplyCommandId. DPlay does
 * not seem to offer any way of uniquely differentiating between replies of the same type
 * relative to the request sent. There is an implicit assumption that there will be no
 * ordering issues on sends and receives from the opposite machine. No wonder MS is not
 * a networking company.
 */
static
LPVOID DP_MSG_ExpectReply( IDirectPlay2AImpl* This, LPDPSP_SENDDATA lpData, 
                           DWORD dwWaitTime, WORD wReplyCommandId,
                           LPVOID* lplpReplyMsg, LPDWORD lpdwMsgBodySize )
{
  HRESULT                  hr;
  HANDLE                   hMsgReceipt;
  DP_MSG_REPLY_STRUCT_LIST replyStructList;
  DWORD                    dwWaitReturn;
 
  /* Setup for receipt */
  hMsgReceipt = DP_MSG_BuildAndLinkReplyStruct( This, &replyStructList, 
                                                wReplyCommandId );

  TRACE( "Sending msg and expecting cmd %u in reply within %lu ticks\n", 
         wReplyCommandId, dwWaitTime );
  hr = (*This->dp2->spData.lpCB->Send)( lpData );

  if( FAILED(hr) )
  {
    ERR( "Request for new playerID send failed: %s\n",
         DPLAYX_HresultToString( hr ) );
    return NULL;
  }

  dwWaitReturn = WaitForSingleObject( hMsgReceipt, dwWaitTime );
  if( dwWaitReturn != WAIT_OBJECT_0 )
  {
    ERR( "Wait failed 0x%08lx\n", dwWaitReturn );
    return NULL;
  }

  /* Clean Up */
  return DP_MSG_CleanReplyStruct( &replyStructList, lplpReplyMsg, lpdwMsgBodySize );
}

/* Determine if there is a matching request for this incomming message and then copy
 * all important data. It is quite silly to have to copy the message, but the documents
 * indicate that a copy is taken. Silly really.
 */
void DP_MSG_ReplyReceived( IDirectPlay2AImpl* This, WORD wCommandId, 
                           LPCVOID lpcMsgBody, DWORD dwMsgBodySize )
{
  LPDP_MSG_REPLY_STRUCT_LIST lpReplyList;

#if 0
  if( wCommandId == DPMSGCMD_FORWARDADDPLAYER )
  {
    DebugBreak();
  }
#endif

  /* Find, and immediately remove (to avoid double triggering), the appropriate entry. Call locked to 
   * avoid problems.
   */
  EnterCriticalSection( &This->unk->DP_lock );
    DPQ_REMOVE_ENTRY( This->dp2->replysExpected, replysExpected, replyExpected.wExpectedReply,\
                     ==, wCommandId, lpReplyList );
  LeaveCriticalSection( &This->unk->DP_lock );  

  if( lpReplyList != NULL )
  {
    lpReplyList->replyExpected.dwMsgBodySize = dwMsgBodySize; 
    lpReplyList->replyExpected.lpReplyMsg = HeapAlloc( GetProcessHeap(),
                                                       HEAP_ZERO_MEMORY,
                                                       dwMsgBodySize );
    CopyMemory( lpReplyList->replyExpected.lpReplyMsg, 
                lpcMsgBody, dwMsgBodySize );

    /* Signal the thread which sent the message that it has a reply */
    SetEvent( lpReplyList->replyExpected.hReceipt );
  }
  else
  {
    ERR( "No receipt event set - only expecting in reply mode\n" );
    DebugBreak();
  }
 
}

void DP_MSG_ErrorReceived( IDirectPlay2AImpl* This, WORD wCommandId,
                           LPCVOID lpMsgBody, DWORD dwMsgBodySize )
{
  LPCDPMSG_FORWARDADDPLAYERNACK lpcErrorMsg;

  lpcErrorMsg = (LPCDPMSG_FORWARDADDPLAYERNACK)lpMsgBody;

  ERR( "Received error message %u. Error is %s\n", 
       wCommandId, DPLAYX_HresultToString( lpcErrorMsg->errorCode) );
  DebugBreak();
}

