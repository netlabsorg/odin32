/* $Id: async.cpp,v 1.1 1999-12-07 20:19:55 achimha Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Asynchronous Winsock code
 *
 * Copyright 1999 Patrick Haller
 *
 *
 * Remark:
 * - do NOT call WSASetLastError in the async worker thread, since there we don't
 *   know anything about the caller thread.
 *
 * Idea is to separate requests:
 * - all database requests and DNS requests are handled by one thread
 * - asynchronous selects are each handled by its own thread
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#define INCL_DOSERRORS
#define INCL_DOSSEMAPHORES
#include <os2.h>

#include <types.h>
#include <utils.h>

#include <netdb.h>
#include <nerrno.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

#include <wsock32const.h>

#include <process.h>
#include <string.h>
#include <stdlib.h>

#include <misc.h>


/*****************************************************************************
 * Definitions                                                               *
 *****************************************************************************/

ODINDEBUGCHANNEL(WSOCK32-ASYNC)


#define ERROR_SUCCESS 0


typedef enum tagSocketStatus
{
  SOCKSTAT_UNKNOWN = 0,     // unknown socket state
  SOCKSTAT_NORMAL,          // normal socket state
  SOCKSTAT_CONNECT_PENDING  // non-blocking connect attempted
};

typedef struct tagSocketAsync
{
  int hSocket; /* operating system socket descriptor */
  int iStatus;

  int iEventMaskAllowed; // bits of valid socket events are set to 1
} SOCKETASYNC, *PSOCKETASYNC;


// prototype of the OS/2 select!
int _System os2_select(int* socket,
                       int  fd_read,
                       int  fd_write,
                       int  fd_exception,
                       long timeout);

int _System bsd_select(int,
                       fd_set *,
                       fd_set *,
                       fd_set *,
                       struct timeval*);

// wsock32.cpp: error code translation
int iTranslateSockErrToWSock(int iError);


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

// request types
enum tagAsyncRequestType
{
  WSAASYNC_TERMINATE,            // internal message
  WSAASYNC_GETHOSTBYADDR,
  WSAASYNC_GETHOSTBYNAME,
  WSAASYNC_GETPROTOBYNAME,
  WSAASYNC_GETPROTOBYNUMBER,
  WSAASYNC_GETSERVBYNAME,
  WSAASYNC_GETSERVBYPORT,
  WSAASYNC_SELECT
};


// request states
enum tagAsyncRequestStates
{
  RS_WAITING,    // request is waiting in queue
  RS_BUSY,       // request is currently being serviced
  RS_CANCELLED,  // request has been cancelled
  RS_DONE
};


typedef struct tagAsyncRequest
{
  struct tagAsyncRequest* pPrev; // pointer to previous request
  struct tagAsyncRequest* pNext; // pointer to next request

  ULONG  ulType;                 // type of request
  ULONG  ulState;                // state of request
  PWSOCKTHREADDATA pwstd;        // save pointer of caller thread's wsock data

  HWND   hwnd;                   // window handle to post message to
  ULONG  ulMessage;              // message to post

  PVOID  pBuffer;                // result buffer
  ULONG  ulBufferLength;         // length of the return buffer

  ULONG  ul1;                    // multi purpose parameters
  ULONG  ul2;
  ULONG  ul3;

} ASYNCREQUEST, *PASYNCREQUEST;


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

void _Optlink WorkerThreadProc(void* pParam);
BOOL _System PostMessageA(HWND hwnd, UINT ulMessage, WPARAM wParam, LPARAM lParam);


/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

class WSAAsyncWorker
{
  // protected members
  protected:
    PASYNCREQUEST pRequestHead;        // chain root
    PASYNCREQUEST pRequestTail;        // chain root
    TID           tidWorker;           // worker thread id
    HEV           hevRequest;          // fired upon new request
    HMTX          hmtxRequestQueue;    // request queue protection
    BOOL          fTerminate;          // got to die ?
    BOOL          fBlocking;           // currently busy ?

    TID           startWorker    (void);                   // start worker thread
    void          processingLoop (void);                   // "work"
    int           dispatchRequest(PASYNCREQUEST pRequest); // complete request
    PASYNCREQUEST popRequest     (void);                   // get one request from queue
    BOOL          deleteRequest  (PASYNCREQUEST pRequest); // remove particular request

    void          lockQueue      (void);                   // enter mutex
    void          unlockQueue    (void);                   // leave mutex

    // the real worker methods
    void          asyncGetHostByAddr   (PASYNCREQUEST pRequest);
    void          asyncGetHostByName   (PASYNCREQUEST pRequest);
    void          asyncGetProtoByName  (PASYNCREQUEST pRequest);
    void          asyncGetProtoByNumber(PASYNCREQUEST pRequest);
    void          asyncGetServByName   (PASYNCREQUEST pRequest);
    void          asyncGetServByPort   (PASYNCREQUEST pRequest);
    void          asyncSelect          (PASYNCREQUEST pRequest);

  // public members
  public:
    WSAAsyncWorker(void);                         // constructor
    ~WSAAsyncWorker();                            // destructor

    PASYNCREQUEST createRequest  (ULONG ulType,
                                  HWND  hwnd,
                                  ULONG ulMessage,
                                  PVOID pBuffer,
                                  ULONG ulBufferLength,
                                  ULONG ul1 = 0,
                                  ULONG ul2 = 0,
                                  ULONG ul3 = 0);

    void          pushRequest    (PASYNCREQUEST pRequest); // put request on queue
    BOOL          cancelAsyncRequest   (PASYNCREQUEST pRequest);
    BOOL          isBlocking     (void) {return fBlocking;}

    // the thread procedure
    friend void _Optlink WorkerThreadProc(void* pParam);
};


/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

//static WSAAsyncWorker* wsaWorker = NULL;
static WSAAsyncWorker* wsaWorker = new WSAAsyncWorker();


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// constructor
WSAAsyncWorker::WSAAsyncWorker(void)
{
  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::WSAAsyncWorker\n"));

  pRequestHead = NULL;  // chain root
  pRequestTail = NULL;  // chain root
  tidWorker    = 0;     // worker thread id
  fTerminate   = FALSE; // got to die ?

  startWorker();
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// destructor
WSAAsyncWorker::~WSAAsyncWorker(void)
{
  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::~WSAAsyncWorker (%08xh)\n",
           this));

  // remove all requests
  while (popRequest() != NULL);

  // just in case ... terminate worker thread
  if (tidWorker != 0)
  {
    fTerminate = TRUE;
    DosPostEventSem(hevRequest);
  }

  DosCloseEventSem(hevRequest);
  DosCloseMutexSem(hmtxRequestQueue);
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// start worker thread if necessary
TID WSAAsyncWorker::startWorker(void)
{
  APIRET rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::startWorker (%08xh)\n",
           this));

  if (tidWorker == 0)
  {
    // create semaphores
    rc = DosCreateEventSem(NULL,   // unnamed
                           &hevRequest,
                           0,      // unshared
                           FALSE); // reset
    if (rc != NO_ERROR)
    {
	  dprintf(("WSOCK32: WSAAsyncWorker::startWorker - DosCreateEventSem = %08xh\n",
                rc));

      return 0;
    }

    rc = DosCreateMutexSem(NULL,   // unnamed
                           &hmtxRequestQueue,
                           0,      // unshared
                           FALSE); // unowned
    if (rc != NO_ERROR)
    {
	  dprintf(("WSOCK32: WSAAsyncWorker::startWorker - DosCreateMutexSem = %08xh\n",
                rc));

      DosCloseEventSem(hevRequest);
      return 0;
    }

    // create thread
#if defined(__IBMCPP__)
    tidWorker = _beginthread(WorkerThreadProc,
                             NULL,
                             16384,
                             (PVOID)this);
#else
    tidWorker = _beginthread(WorkerThreadProc,
                             16384,
                             (PVOID)this);
#endif
    if (tidWorker == -1)
    {
      // cancel the whole thing
      tidWorker        = 0;
      DosCloseEventSem(hevRequest);
      DosCloseMutexSem(hmtxRequestQueue);
    }
  }

  return tidWorker; // thread already running
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// lock double-linked request chain
void WSAAsyncWorker::lockQueue(void)
{
  DosRequestMutexSem(hmtxRequestQueue, SEM_INDEFINITE_WAIT);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// unlock double-linked request chain
void WSAAsyncWorker::unlockQueue(void)
{
  DosReleaseMutexSem(hmtxRequestQueue);
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// get request from queue
PASYNCREQUEST WSAAsyncWorker::popRequest(void)
{
  PASYNCREQUEST pLast;

  lockQueue();  // lock queue

  // alter queue
  pLast = pRequestTail;

  if (pRequestTail != NULL)
  {
    if (pRequestTail->pPrev)
    {
      pRequestTail->pPrev->pNext = NULL;  // cut off element
      pRequestTail = pRequestTail->pPrev; // unlink previous element
    }
    else
    {
      // this is the last request on the queue
      pRequestTail = NULL;
      pRequestHead = NULL;
    }
  }

  unlockQueue();  // unlock queue

  return (pLast); // return element
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// insert request into queue
void WSAAsyncWorker::pushRequest(PASYNCREQUEST pNew)
{
  if (pNew == NULL) // ensure proper parameters
    return;

  lockQueue();  // lock queue

  // alter queue
  if (pRequestHead == NULL)
  {
    // first element in queue
    pRequestHead = pNew;
    pRequestTail = pNew;
    pNew->pPrev  = NULL;
    pNew->pNext  = NULL;
  }
  else
  {
    // chain in request
    pNew->pPrev         = NULL;
    pNew->pNext         = pRequestHead;
    pRequestHead->pPrev = pNew;
    pRequestHead        = pNew;
  }

  // queue debug
  {
    PASYNCREQUEST pTemp;

    dprintf(("WSOCK32: WSAAsyncWorker pRequest  type      state     hwnd      message\n"));
    for (pTemp = pRequestHead;
         pTemp;
         pTemp = pTemp->pNext)
      dprintf(("WSOCK32: WSAAsyncWorker %08xh %08xh %08xh %08xh %08xh",
               pTemp,
               pTemp->ulType,
               pTemp->ulState,
               pTemp->hwnd,
               pTemp->ulMessage));
  }

  // trigger thread!
  DosPostEventSem(hevRequest);

  unlockQueue();  // unlock queue
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// delete particular request from queue
BOOL WSAAsyncWorker::deleteRequest(PASYNCREQUEST pDelete)
{
  PASYNCREQUEST pRequest;      // for verification
  BOOL          bResult = FALSE;

  lockQueue();  // lock queue

  // find request (optional, just for verification)
  for (pRequest = pRequestHead;
       pRequest != NULL;
       pRequest = pRequest->pNext)
    if (pRequest == pDelete)
      break;

  if (pRequest == pDelete) // if request has been found
  {
    // is request the head of the list ?
    if (pDelete == pRequestHead)
    {
      pRequestHead = pDelete->pNext;
      if (pRequestHead == NULL) // last element ?
        pRequestTail = NULL;
      else
        pRequestHead->pPrev = NULL;
    }
    else
      // if request the last in the list ?
      if (pDelete == pRequestTail)
      {
        pRequestTail = pDelete->pPrev;
        if (pRequestTail == NULL) // last element ?
          pRequestHead = NULL;
        else
          pRequestTail->pNext = NULL;
      }
      else
        // request is somewhere in the middle of the list
        {
          if (pDelete->pPrev != NULL)
            pDelete->pPrev->pNext = pDelete->pNext;

          if (pDelete->pNext != NULL)
            pDelete->pNext->pPrev = pDelete->pPrev;
        }

    delete pDelete; // free the memory
    bResult = TRUE; // OK
  }

  unlockQueue();  // unlock queue
  return bResult;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

BOOL WSAAsyncWorker::cancelAsyncRequest(PASYNCREQUEST pRequest)
{
  PASYNCREQUEST pRequestTemp;
  BOOL          rc = TRUE;

  lockQueue();

  // verify pRequest
  // find request (optional, just for verification)
  for (pRequestTemp = pRequestHead;
       pRequestTemp != NULL;
       pRequestTemp = pRequestTemp->pNext)
    if (pRequestTemp == pRequest)
      break;

  // is request in queue ?
  if (pRequestTemp == pRequest)
  {
    // is it busy?
    if (pRequest->ulState != RS_BUSY)
    {
      // if not: set RS_CANCELLED
      pRequest->ulState = RS_CANCELLED;
    }
    else
    {
      // if busy: ???
      dprintf(("WSOCK32:Async: WSAAsyncWorker::cancelAsyncRequest(%08xh, %08xh) how to cancel?\n",
               this,
               pRequest));
      rc = FALSE;
    }
  }
  else
    rc = FALSE;

  unlockQueue();
  return rc;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

PASYNCREQUEST WSAAsyncWorker::createRequest  (ULONG ulType,
                                              HWND  hwnd,
                                              ULONG ulMessage,
                                              PVOID pBuffer,
                                              ULONG ulBufferLength,
                                              ULONG ul1,
                                              ULONG ul2,
                                              ULONG ul3)
{
  PASYNCREQUEST pNew = new ASYNCREQUEST();

  if (pNew != NULL) // check for proper allocation
  {
    // fill the structure
    pNew->pPrev          = NULL;
    pNew->pNext          = NULL;
    pNew->ulType         = ulType;
    pNew->ulState        = RS_WAITING;
    pNew->hwnd           = hwnd;
    pNew->ulMessage      = ulMessage;
    pNew->pBuffer        = pBuffer;
    pNew->ulBufferLength = ulBufferLength;
    pNew->ul1            = ul1;
    pNew->ul2            = ul2;
    pNew->ul3            = ul3;

    // save caller thread's wsock data
    pNew->pwstd          = iQueryWsockThreadData();
  }

  dprintf(("WSOCK32: WSAAsyncWorker:createRequest = %08xh\n",
           pNew));

  return pNew;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetHostByAddr   (PASYNCREQUEST pRequest)
{
  struct hostent* pHostent;
  USHORT          usLength = sizeof(struct Whostent);
  USHORT          rc;
  struct Whostent* pwhostent = (struct Whostent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetHostByAddr (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Whostent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pHostent = gethostbyaddr((char*)pRequest->ul1,
                             (int)        pRequest->ul2,
                             (int)        pRequest->ul3);
    if (pHostent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // translate result to Wsock32-style structure
      pwhostent->h_name      = pHostent->h_name;
      pwhostent->h_aliases   = pHostent->h_aliases;
      pwhostent->h_addrtype  = pHostent->h_addrtype;
      pwhostent->h_length    = pHostent->h_length;
      pwhostent->h_addr_list = pHostent->h_addr_list;
      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetHostByName   (PASYNCREQUEST pRequest)
{
  struct hostent*  pHostent;
  USHORT           usLength = sizeof(struct Whostent);
  USHORT           rc;
  struct Whostent* pwhostent = (struct Whostent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetHostByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Whostent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pHostent = gethostbyname((char*)pRequest->ul1);
    if (pHostent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // translate result to Wsock32-style structure
      pwhostent->h_name      = pHostent->h_name;
      pwhostent->h_aliases   = pHostent->h_aliases;
      pwhostent->h_addrtype  = pHostent->h_addrtype;
      pwhostent->h_length    = pHostent->h_length;
      pwhostent->h_addr_list = pHostent->h_addr_list;
      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetProtoByName  (PASYNCREQUEST pRequest)
{
  struct protoent* pProtoent;
  USHORT           usLength = sizeof(struct Wprotoent);
  USHORT           rc;
  struct Wprotoent* pwprotoent= (struct Wprotoent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetProtoByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Wprotoent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pProtoent = getprotobyname((char*)pRequest->ul1);
    if (pProtoent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // build result buffer
      pwprotoent->p_name    = pProtoent->p_name;
      pwprotoent->p_aliases = pProtoent->p_aliases;
      pwprotoent->p_proto   = pProtoent->p_proto;
      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetProtoByNumber(PASYNCREQUEST pRequest)
{
  struct protoent* pProtoent;
  USHORT           usLength  = sizeof(struct Wprotoent);
  USHORT           rc;
  struct Wprotoent* pwprotoent= (struct Wprotoent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetProtoByNumber (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Wprotoent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pProtoent = getprotobyname(( char*)pRequest->ul1);
    if (pProtoent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // build result buffer
      pwprotoent->p_name    = pProtoent->p_name;
      pwprotoent->p_aliases = pProtoent->p_aliases;
      pwprotoent->p_proto   = pProtoent->p_proto;
      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetServByName(PASYNCREQUEST pRequest)
{
  struct servent* pServent;
  USHORT          usLength = sizeof(struct Wservent);
  USHORT          rc;
  struct Wservent* pwservent= (struct Wservent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetServByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Wservent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pServent = getservbyname((char*)pRequest->ul1,
                             (char*)pRequest->ul2);
    if (pServent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // build result buffer
      pwservent->s_name    = pServent->s_name;
      pwservent->s_aliases = pServent->s_aliases;
      pwservent->s_port    = pServent->s_port;
      pwservent->s_proto   = pServent->s_proto;
      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

void WSAAsyncWorker::asyncGetServByPort(PASYNCREQUEST pRequest)
{
  struct servent* pServent;
  USHORT          usLength = sizeof(struct Wservent);
  USHORT          rc;
  struct Wservent* pwservent= (struct Wservent*)pRequest->pBuffer;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetServByPort (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  if (pRequest->ulBufferLength < sizeof(struct Whostent))
  {
    rc = WSAEINVAL;
    //WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // call API
    pServent = getservbyport((int        )pRequest->ul1,
                             (char*)pRequest->ul2);
    if (pServent == NULL) // error ?
    {
      // build error return code
      rc = iTranslateSockErrToWSock(sock_errno());
      //WSASetLastError(rc);
    }
    else
    {
      // build result buffer
      pwservent->s_name    = pServent->s_name;
      pwservent->s_aliases = pServent->s_aliases;
      pwservent->s_port    = pServent->s_port;
      pwservent->s_proto   = pServent->s_proto;

      rc = 0;
    }
  }

  // post result
  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)pRequest,
               (LPARAM)(rc << 16 | usLength));

  // M$ says, if PostMessageA fails, spin as long as window exists
}



/*****************************************************************************
 * Name      : WSAAsyncWorker::asyncSelect
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

typedef int SOCKET;

void WSAAsyncWorker::asyncSelect(PASYNCREQUEST pRequest)
{
  ULONG           wParam;
  ULONG           lParam;
  int             irc;
  int             iUnknown;

  SOCKET          sockWin;
  ULONG           ulEvent;
  USHORT          usResult = 0;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncSelect (%08xh, %08xh) not correctly implemented\n",
           this,
           pRequest));


  // setup variables
  sockWin = (SOCKET)pRequest->ul1;
  ulEvent = (ULONG) pRequest->ul2;

  //@@@PH to do
  // 1. automatically set socket to non-blocking mode
  // 2. loop until WSAAsyncSelect(s,hwnd,0,0) ?


  //@@@PH how to implement other events?


  // finally do the select!
#ifdef BSDSELECT
  irc = os2_select(&sockWin,
                   (ulEvent & FD_READ)  ? 1 : 0,
                   (ulEvent & FD_WRITE) ? 1 : 0,
                   (ulEvent & FD_OOB)   ? 1 : 0,
                   10000);              // @@@PH timeout
#else

  // BSD implementation
  fd_set fds_read,     *pfds_read      = &fds_read;
  fd_set fds_write,    *pfds_write     = &fds_write;
  fd_set fds_exception,*pfds_exception = &fds_exception;
  struct timeval tv;

  FD_ZERO(&fds_read);      FD_SET(sockWin,&fds_read);
  FD_ZERO(&fds_write);     FD_SET(sockWin,&fds_write);
  FD_ZERO(&fds_exception); FD_SET(sockWin,&fds_exception);

  tv.tv_sec  = 10;
  tv.tv_usec = 0;

  if (!(ulEvent & FD_READ))  pfds_read      = NULL;
  if (!(ulEvent & FD_WRITE)) pfds_write     = NULL;
  if (!(ulEvent & FD_OOB))   pfds_exception = NULL;

  irc = bsd_select(sockWin+1,
                   pfds_read,
                   pfds_write,
                   pfds_exception,
                   &tv);
#endif

  if (irc < 0)                                          /* an error occurred */
  {
    lParam = sock_errno();                          /* raise error condition */
    if (lParam == SOCENOTSOCK)
    {
      usResult = FD_CLOSE;
      lParam   = 0;
    }
  }
  else
    if (irc == 0)                                      /* this means timeout */
    {
      lParam = iTranslateSockErrToWSock(WSAETIMEDOUT);/* raise error condition */
    }
    else
    {
      //@@@PH check the socket for any event and report!
      usResult = 0;

      // readiness for reading bytes ?
/*
      if (FD_ISSET(sockWin, pfds_read))      usResult |= FD_READ;
      if (FD_ISSET(sockWin, pfds_write))     usResult |= FD_WRITE;
      if (FD_ISSET(sockWin, pfds_exception)) usResult |= FD_OOB;
*/

/*
#define FD_READ    0x01
#define FD_WRITE   0x02
#define FD_OOB     0x04
#define FD_ACCEPT  0x08
#define FD_CONNECT 0x10
#define FD_CLOSE   0x20
*/

      irc = ioctl(sockWin, FIONREAD, (char *)&iUnknown, sizeof(iUnknown));
      if ( (irc == 0) && (iUnknown > 0))
         usResult |= FD_READ | FD_CONNECT;
    }

  // post result
  irc = (LPARAM)((lParam << 16) | usResult);
  dprintf(("WSOCK32:asyncSelect() posting %08xh hwnd%08xh, msg=%08xh, wparam=%08xh, lparam=%08xh\n",
           pRequest,
           pRequest->hwnd,
           pRequest->ulMessage,
           sockWin,
           irc));

  PostMessageA(pRequest->hwnd,
               pRequest->ulMessage,
               (WPARAM)sockWin,
               (LPARAM)irc);

  // M$ says, if PostMessageA fails, spin as long as window exists
}



/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// process one request
int WSAAsyncWorker::dispatchRequest(PASYNCREQUEST pRequest)
{
  int rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::dispatchRequest (%08xh, %08xh)\n",
           this,
           pRequest));

  // check request state first
  switch(pRequest->ulState)
  {
    case RS_WAITING: // OK, proceed
      break;

    case RS_BUSY:    // oops, shouldn't happen
      dprintf(("WSOCK32: WSAAsyncWorker::dispatchRequest - got already busy request %08xh\n",
                pRequest));
      return 1;

    case RS_CANCELLED: // don't service request
      // request has been removed from queue already
      return 1;
  }

  // OK, servicing request
  pRequest->ulState = RS_BUSY;

  switch(pRequest->ulType)
  {
    case WSAASYNC_TERMINATE:     // internal message
      fTerminate = TRUE;
      return 1;

    case WSAASYNC_GETHOSTBYADDR:    asyncGetHostByAddr   (pRequest); rc = 1; break;
    case WSAASYNC_GETHOSTBYNAME:    asyncGetHostByName   (pRequest); rc = 1; break;
    case WSAASYNC_GETPROTOBYNAME:   asyncGetProtoByName  (pRequest); rc = 1; break;
    case WSAASYNC_GETPROTOBYNUMBER: asyncGetProtoByNumber(pRequest); rc = 1; break;
    case WSAASYNC_GETSERVBYNAME:    asyncGetServByName   (pRequest); rc = 1; break;
    case WSAASYNC_GETSERVBYPORT:    asyncGetServByPort   (pRequest); rc = 1; break;
    case WSAASYNC_SELECT:           asyncSelect          (pRequest); rc = 1; break;

    default:
      dprintf(("WSOCK32: WSAAsyncWorker::dispatchRequest - invalid request type %d\n",
                pRequest->ulType));
      rc = 1; break;
  }

  pRequest->ulState = RS_DONE;
  return rc;
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// process all requests in queue until termination
void WSAAsyncWorker::processingLoop(void)
{
  PASYNCREQUEST pRequest;
  APIRET        rc;
  ULONG         ulPostCount;

  do
  {
    // work as long as there are requests
    do
    {
      pRequest = popRequest();     // get request from queue
      if (pRequest != NULL)
      {
        dispatchRequest(pRequest); // process request
        delete pRequest;           // free the memory
      }
    }
    while (pRequest != NULL);

    // wait for semaphore
    fBlocking         = FALSE;
    rc = DosWaitEventSem(hevRequest, SEM_INDEFINITE_WAIT);
    fBlocking         = TRUE;
    rc = DosResetEventSem(hevRequest, &ulPostCount);
  }
  while (fTerminate == FALSE);

  tidWorker = 0; // clear worker thread id
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// thread procedure
void _Optlink WorkerThreadProc(void* pParam)
{
  // convert object pointer
  WSAAsyncWorker* pWSAAsyncWorker = (WSAAsyncWorker*)pParam;
  pWSAAsyncWorker->processingLoop();  // processing loop
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION5(HANDLE, WSAAsyncGetHostByName, HWND,         hwnd,
                                             unsigned int, wMsg,
                                             const char*,  name,
                                             char*,        buf,
                                             int,          buflen)
{
  dprintf(("name = %s\n", name));
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETHOSTBYNAME,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)name);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION7(HANDLE, WSAAsyncGetHostByAddr, HWND,         hwnd,
                                             unsigned int, wMsg,
                                             const char*,  addr,
                                             int,          len,
                                             int,          type,
                                             char*,        buf,
                                             int,          buflen)
{
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETHOSTBYADDR,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)addr,
                                                    (ULONG)len,
                                                    (ULONG)type);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION6(HANDLE, WSAAsyncGetServByName, HWND,         hwnd,
                                             unsigned int, wMsg,
                                             const char*,  name,
                                             const char*,  proto,
                                             char*,        buf,
                                             int,          buflen)
{
  dprintf(("name = %s, proto = %s\n", name, proto));
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETSERVBYNAME,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)name,
                                                    (ULONG)proto);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION6(HANDLE, WSAAsyncGetServByPort, HWND,         hwnd,
                                             unsigned int, wMsg,
                                             int,          port,
                                             const char*,  proto,
                                             char*,        buf,
                                             int,          buflen)
{
  dprintf(("proto = %s\n", proto));
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETSERVBYPORT,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)port,
                                                    (ULONG)proto);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION5(HANDLE, WSAAsyncGetProtoByName, HWND,         hwnd,
                                              unsigned int, wMsg,
                                              const char*,  name,
                                              char*,        buf,
                                              int,          buflen)
{
  dprintf(("name = %s\n", name));
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETPROTOBYNAME,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)name);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls
ODINFUNCTION5(HANDLE, WSAAsyncGetProtoByNumber, HWND,         hwnd,
                                              unsigned int, wMsg,
                                              int,          number,
                                              char*,        buf,
                                              int,          buflen)
{
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_GETPROTOBYNUMBER,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)number);
  if (pRequest != NULL)
  {
    // success
    wsaWorker->pushRequest(pRequest);
    WSASetLastError(ERROR_SUCCESS);
    return (HANDLE)pRequest;
  }
  else
  {
    // error
    WSASetLastError(WSAENOBUFS);
    return 0;
  }
}


/*****************************************************************************
 * Name      :
 * Purpose   : cancel a queued or busy request
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION1(int, WSACancelAsyncRequest, HANDLE, hAsyncTaskHandle)
{
  PASYNCREQUEST pRequest = (PASYNCREQUEST)hAsyncTaskHandle;
  BOOL          rc;

  // remove request from queue
  rc = wsaWorker->cancelAsyncRequest(pRequest);
  if (rc == TRUE)
  {
    WSASetLastError(ERROR_SUCCESS);
    return ERROR_SUCCESS; // success
  }
  else
  {
    // error
    WSASetLastError(WSAEINVAL);
    return (SOCKET_ERROR);
  }
}


/*****************************************************************************
 * Name      : WSAIsBlocking
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION0(BOOL, WSAIsBlocking)
{
  return(wsaWorker->isBlocking());
}


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/
// the real function calls

ODINFUNCTION4(int, WSAAsyncSelect,SOCKET,       s,
                                  HWND,         hwnd,
                                  unsigned int, wMsg,
                                  long,         lEvent)
{
  // @@@PH 1999/11/21 implementation is completely wrong!
  // handle async select in a completely different way:
  // one thread per socket!
  PASYNCREQUEST pRequest = wsaWorker->createRequest(WSAASYNC_SELECT,
                                                    (HWND) hwnd,
                                                    (ULONG)wMsg,
                                                    (PVOID)NULL,
                                                    (ULONG)0,
                                                    (ULONG)s,
                                                    (ULONG)lEvent);
  if (pRequest != NULL) // request is OK ?
  {
    wsaWorker->pushRequest(pRequest);

    // WSAAsyncSelect() can only fail if the flags specified don't match
    WSASetLastError(ERROR_SUCCESS);
    return ERROR_SUCCESS;
  }
  else
  {
    // error !
    WSASetLastError(WSAEINVAL);
    return SOCKET_ERROR;
  }
}

