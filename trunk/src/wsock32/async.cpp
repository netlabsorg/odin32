/* $Id: async.cpp,v 1.1 1999-10-20 01:18:28 phaller Exp $ */

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
#include <os2win.h>
//#include <wsock32.h>
#include <misc.h>


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
  ULONG  ul4;
  ULONG  ul5;

} ASYNCREQUEST, *PASYNCREQUEST;



/*****************************************************************************
 * Implementation                                                            *
 *****************************************************************************/

class WSAAsyncWorker
{
  // private members
  PASYNCREQUEST pRequestHead = NULL; // chain root
  PASYNCREQUEST pRequestTail = NULL; // chain root
  TID           tidWorker    = 0;    // worker thread id
  HEV           hevRequest;          // fired upon new request
  HMUTEX        hmtxRequestQueue;    // request queue protection

  // public members
  public:
    WSAAsyncWorker(void);                         // constructor
    ~WSAAsyncWorker();                            // destructor

    PASYNCREQUEST createRequest  (HWND  hwnd,
                                  ULONG ulMessage,
                                  ULONG ul1 = 0,
                                  ULONG ul2 = 0,
                                  ULONG ul3 = 0,
                                  ULONG ul4 = 0,
                                  ULONG ul5 = 0);
    void          pushRequest    (PASYNCREQUEST pRequest); // put request on queue



  // protected members
  protected:
    BOOL          fTerminate = FALSE;                      // got to die ?

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
};


// constructor
WSAAsyncWorker::WSAAsyncWorker(void)
{
  startWorker();
}


// destructor
WSAAsyncWorker::~WSAAsyncWorker(void)
{
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



// start worker thread if necessary
TID WSAAsyncWorker::startWorker(void)
{
  APIRET rc;

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
    tidWorker = _beginthread(WorkerThreadProc,
                             (PVOID)this,
                             4096);
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


// lock double-linked request chain
void WSAAsyncWorker::lockQueue(void)
{
  DosRequestMutex(hmtxRequestQueue, SEM_INDEFINITE_WAIT);
}


// unlock double-linked request chain
void WSAAsyncWorker::unlockQueue(void)
{
  DosReleaseMutex(hmtxRequestQueue);
}


// get request from queue
PASYNCREQUEST WSAAsyncWorker::popRequest(void)
{
  PASYNCREQUEST pLast;

  lockQueue();  // lock queue

  // alter queue
  pLast = pRequestTail;

  if (pRequestTail != NULL)
    if (pRequestTail->pPrev)
    {
      pRequestTail->pPrev->pNext = NULL;  // cut off element
      pRequestTail = pRequestTail->pPrev; // unlink previous element
    }

  unlockQueue();  // unlock queue

  return (pLast); // return element
}


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

    bResult = TRUE; // OK
  }

  unlockQueue();  // unlock queue
  return bResult;
}



void WSAAsyncWorker::asyncGetHostByAddr   (PASYNCREQUEST pRequest)
{
  struct hostent* pHostent;
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct hostent));

  // call API
  pHostent = gethostbyaddr((const char*)pRequest->u1,
                           (int)        pRequest->u2,
                           (int)        pRequest->u3);
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

void WSAAsyncWorker::asyncGetHostByName   (PASYNCREQUEST pRequest)
{
  struct hostent* pHostent;
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct hostent));

  // call API
  pHostent = gethostbyname((const char*)pRequest->u1);
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


void WSAAsyncWorker::asyncGetProtoByName  (PASYNCREQUEST pRequest)
{
  struct protoent* pProtoent;
  USHORT           usLength;
  ULONG            wParam;
  ULONG            lParam;
  USHORT           rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct protoent));

  // call API
  pProtoent = getprotobyname((const char*)pRequest->u1);
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

void WSAAsyncWorker::asyncGetProtoByNumber(PASYNCREQUEST pRequest)
{
  struct protoent* pProtoent;
  USHORT           usLength;
  ULONG            wParam;
  ULONG            lParam;
  USHORT           rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct protoent));

  // call API
  pProtoent = getprotobyname((int)pRequest->u1);
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


void WSAAsyncWorker::asyncGetServByName(PASYNCREQUEST pRequest)
{
  struct servent* pServent;
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct servent));

  // call API
  pServent = getservbyname((const char*)pRequest->u1,
                           (const char*)pRequest->u2);
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


void WSAAsyncWorker::asyncGetServByPort(PASYNCREQUEST pRequest)
{
  struct servent* pServent;
  USHORT          usLength;
  ULONG           wParam;
  ULONG           lParam;
  USHORT          rc;

  // result buffer length
  usLength = min(pRequest->ulBufferLength, sizeof(struct servent));

  // call API
  pServent = getservbyport((int        )pRequest->u1,
                           (const char*)pRequest->u2);
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


// process one request
int WSAAsyncWorker::dispatchRequest(PASYNCREQUEST pRequest)
{
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
  pRequest->ulStatus = RS_BUSY;

  switch(pRequest->ulType)
  {
    case WSAASYNC_TERMINATE:     // internal message
      fTerminate = TRUE;
      return 1;

    case WSAASYNC_GETHOSTBYADDR:
      asyncGetHostByAddr(pRequest->hwnd,
                         pRequest->ulMessage,
                         (char*)pRequest->u1,
                         (int)  pRequest->u2,
                         (int)  pRequest->u3,
                         (char*)pRequest->u4,
                         (int)  pRequest->u5);
      return 1;

    case WSAASYNC_GETHOSTBYNAME:
      asyncGetHostByName(pRequest->hwnd,
                         pRequest->ulMessage,
                         (char*)pRequest->u1,
                         (char*)pRequest->u2,
                         (int)  pRequest->u3);
      break;

    case WSAASYNC_GETPROTOBYNAME:
      asyncGetProtoByName  (pRequest->hwnd,
                            pRequest->ulMessage,
                            (char*)pRequest->u1,
                            (char*)pRequest->u2,
                            (int)  pRequest->u3);
      break;

    case WSAASYNC_GETPROTOBYNUMBER:
      asyncGetProtoByNumber(pRequest->hwnd,
                            pRequest->ulMessage,
                            (int)  pRequest->u1,
                            (char*)pRequest->u2,
                            (int)  pRequest->u3);
      break;

    case WSAASYNC_GETSERVBYNAME:
      asyncGetServByName   (pRequest->hwnd,
                            pRequest->ulMessage,
                            (char*)pRequest->u1,
                            (char*)pRequest->u2,
                            (char*)pRequest->u3,
                            (int)  pRequest->u4);
      break;

    case WSAASYNC_GETSERVBYPORT:
      asyncGetServByPort   (pRequest->hwnd,
                            pRequest->ulMessage,
                            (int)  pRequest->u1,
                            (char*)pRequest->u2,
                            (char*)pRequest->u3,
                            (int)  pRequest->u4);
      break;

    case WSAASYNC_SELECT:
      break;

    default:
      dprintf(("WSOCK32: WSAAsyncWorker::dispatchRequest - invalid request type %d\n",
                pRequest->ulType));
      return 1;
  }

  pRequest->ulStatus = RS_DONE;
  return 0;
}


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
        dispatchRequest(pRequest); // process request
    }
    while (pRequest != NULL)

    // wait for semaphore
    rc = DosWaitEventSem(hevRequest, SEM_INDEFINITE_WAIT);
    rc = DosResetEventSem(hevRequest, &ulPostCount);
  }
  while (fTerminate == FALSE);

  tidWorker = 0; // clear worker thread id
}


// thread procedure
void _System WorkerThreadProc(PVOID pParam)
{
  // convert object pointer
  WSAAsyncWorker* pWSAAsyncWorker = (WSAAsyncWorker*)pParam;
  pWSAAsyncWorker->processingLoop();  // processing loop
}


// the real function calls
ODINFUNCTION5(HANDLE, WSAAsyncGetHostByName, HWND,         hwnd,
                                             unsigned int, wMsg,
                                             const char*,  name,
                                             char*,        buf,
                                             int,          buflen)
{
  PASYNCREQUEST pRequest = wsaWorker->createRequest((HWND) hwnd,
                                                    (PVOID)buf,
                                                    (ULONG)buflen,
                                                    (ULONG)wMsg,
                                                    (ULONG)name);
  wsaWorker->pushRequest(pRequest);
  return (HANDLE)pRequest;
}


