/* $Id: async.cpp,v 1.4 1999-10-20 11:04:12 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Asynchronous Winsock code
 *
 * Copyright 1999 Patrick Haller
 *
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

#include <netdb.h>
#include <nerrno.h>
#include <sys/socket.h>
#include <wsock32const.h>

#include <process.h>
#include <string.h>
#include <stdlib.h>

#include <misc.h>


/*****************************************************************************
 * Definitions                                                               *
 *****************************************************************************/

ODINDEBUGCHANNEL(WSOCK32-ASYNC)



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

void _System SetLastError(int iError);
int  _System GetLastError(void);
#define WSASetLastError(a) SetLastError(a)


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
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetHostByAddr (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct hostent));

  // call API
  pHostent = gethostbyaddr((const char*)pRequest->ul1,
                           (int)        pRequest->ul2,
                           (int)        pRequest->ul3);
  if (pHostent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pHostent,
            usLength);
    rc = 0;
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
  struct hostent* pHostent;
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetHostByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct hostent));

  // call API
  pHostent = gethostbyname((const char*)pRequest->ul1);
  if (pHostent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pHostent,
            usLength);
    rc = 0;
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
  USHORT           usLength;
  ULONG            wParam;
  ULONG            lParam;
  USHORT           rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetProtoByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct protoent));

  // call API
  pProtoent = getprotobyname((const char*)pRequest->ul1);
  if (pProtoent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pProtoent,
            usLength);
    rc = 0;
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
  USHORT           usLength;
  ULONG            wParam;
  ULONG            lParam;
  USHORT           rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetProtoByNumber (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct protoent));

  // call API
  pProtoent = getprotobyname((const char*)pRequest->ul1);
  if (pProtoent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pProtoent,
            usLength);
    rc = 0;
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
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetServByName (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct servent));

  // call API
  pServent = getservbyname((const char*)pRequest->ul1,
                           (const char*)pRequest->ul2);
  if (pServent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pServent,
            usLength);
    rc = 0;
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
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  dprintf(("WSOCK32-ASYNC: WSAAsyncWorker::asyncGetServByPort (%08xh, %08xh)\n",
           this,
           pRequest));

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct servent));

  // call API
  pServent = getservbyport((int        )pRequest->ul1,
                           (const char*)pRequest->ul2);
  if (pServent == NULL) // error ?
  {
    rc = sock_errno();   // assuming OS/2 return codes are
    WSASetLastError(rc); // same as Winsock return codes
  }
  else
  {
    // build result buffer
    memcpy (pRequest->pBuffer,
            pServent,
            usLength);
    rc = 0;
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
  fBlocking         = TRUE;

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

//    case WSAASYNC_SELECT:
//      break;

    default:
      dprintf(("WSOCK32: WSAAsyncWorker::dispatchRequest - invalid request type %d\n",
                pRequest->ulType));
      rc = 1; break;
  }

  pRequest->ulState = RS_DONE;
  fBlocking         = FALSE;
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
    rc = DosWaitEventSem(hevRequest, SEM_INDEFINITE_WAIT);
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
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
    return 0; // success
  else
  {
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
