/*
 * KERNEL32: I/O completion ports.
 *
 * A completion port is a counting semaphore (its handle is the port handle)
 * plus a FIFO of completion packets.  File handles are associated with a port
 * by CreateIoCompletionPort; overlapped ReadFile/WriteFile on such a handle
 * complete synchronously (see hmfile.cpp) and then call OdinIocpComplete,
 * which queues a packet for the port exactly as Windows does.  Threads pick
 * the packets up with GetQueuedCompletionStatus, and PostQueuedCompletionStatus
 * queues user packets (java.nio uses it to wake and stop its event threads).
 *
 * Not supported: overlapped socket I/O (nothing posts packets for sockets), the
 * concurrency limit (NumberOfConcurrentThreads is ignored).
 */
#include <os2win.h>
#include <stdlib.h>
#include <string.h>
#include "odintrace.h"

typedef struct IocpPacket {
    DWORD              bytes;
    DWORD              key;
    LPOVERLAPPED       ov;
    DWORD              status;
    struct IocpPacket *next;
} IocpPacket;

typedef struct IocpPort {
    HANDLE             hSem;
    IocpPacket        *head;
    IocpPacket        *tail;
    struct IocpPort   *next;
} IocpPort;

typedef struct IocpAssoc {
    HANDLE             hFile;
    IocpPort          *port;
    DWORD              key;
    struct IocpAssoc  *next;
} IocpAssoc;

static IocpPort  *ports  = NULL;
static IocpAssoc *assocs = NULL;
static volatile LONG iocpLock = 0;

static void lock(void)
{
    while(__sync_lock_test_and_set(&iocpLock, 1))
        Sleep(0);
}

static void unlock(void)
{
    __sync_lock_release(&iocpLock);
}

//lock must be held
static IocpPort *findPort(HANDLE h)
{
    IocpPort *p;

    for(p = ports; p; p = p->next)
        if(p->hSem == h)
            return p;
    return NULL;
}

static void enqueue(IocpPort *port, DWORD bytes, DWORD key, LPOVERLAPPED ov, DWORD status)
{
    IocpPacket *pkt = (IocpPacket *)malloc(sizeof(IocpPacket));

    if(!pkt)
        return;
    pkt->bytes  = bytes;
    pkt->key    = key;
    pkt->ov     = ov;
    pkt->status = status;
    pkt->next   = NULL;
    if(port->tail)
        port->tail->next = pkt;
    else
        port->head = pkt;
    port->tail = pkt;
    ReleaseSemaphore(port->hSem, 1, NULL);
}

extern "C" {

HANDLE WIN32API CreateIoCompletionPort(HANDLE FileHandle, HANDLE ExistingCompletionPort,
                                       DWORD CompletionKey, DWORD NumberOfConcurrentThreads)
{
    IocpPort *port;

    if(ExistingCompletionPort == NULL)
    {
        HANDLE hSem = CreateSemaphoreA(NULL, 0, 0x7FFFFFFF, NULL);

        if(hSem == NULL)
            return NULL;
        port = (IocpPort *)calloc(1, sizeof(IocpPort));
        if(!port) {
            CloseHandle(hSem);
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return NULL;
        }
        port->hSem = hSem;
        lock();
        port->next = ports;
        ports = port;
        unlock();
        ExistingCompletionPort = hSem;
    }
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        IocpAssoc *a;

        lock();
        port = findPort(ExistingCompletionPort);
        if(!port) {
            unlock();
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        for(a = assocs; a; a = a->next)
            if(a->hFile == FileHandle)
                break;
        if(a) {
            unlock();                                   //already associated
            SetLastError(ERROR_INVALID_PARAMETER);
            return NULL;
        }
        a = (IocpAssoc *)malloc(sizeof(IocpAssoc));
        if(a) {
            a->hFile = FileHandle;
            a->port  = port;
            a->key   = CompletionKey;
            a->next  = assocs;
            assocs   = a;
        }
        unlock();
        if(!a) {
            SetLastError(ERROR_NOT_ENOUGH_MEMORY);
            return NULL;
        }
    }
    return ExistingCompletionPort;
}

BOOL WIN32API PostQueuedCompletionStatus(HANDLE CompletionPort, DWORD dwNumberOfBytesTransferred,
                                         DWORD dwCompletionKey, LPOVERLAPPED lpOverlapped)
{
    IocpPort *port;

    lock();
    port = findPort(CompletionPort);
    if(port)
        enqueue(port, dwNumberOfBytesTransferred, dwCompletionKey, lpOverlapped, 0);
    unlock();
    if(!port) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }
    return TRUE;
}

BOOL WIN32API GetQueuedCompletionStatus(HANDLE CompletionPort, LPDWORD lpNumberOfBytesTransferred,
                                        LPDWORD lpCompletionKey, LPOVERLAPPED *lpOverlapped,
                                        DWORD dwMilliseconds)
{
    IocpPort   *port;
    IocpPacket *pkt;
    DWORD       wr;

    if(lpOverlapped)
        *lpOverlapped = NULL;

    lock();
    port = findPort(CompletionPort);
    unlock();
    if(!port) {
        SetLastError(ERROR_INVALID_HANDLE);
        return FALSE;
    }

    wr = WaitForSingleObject(CompletionPort, dwMilliseconds);
    if(wr != WAIT_OBJECT_0) {
        SetLastError(wr == WAIT_TIMEOUT ? WAIT_TIMEOUT : ERROR_INVALID_HANDLE);
        return FALSE;
    }

    lock();
    pkt = port->head;
    if(pkt) {
        port->head = pkt->next;
        if(!port->head)
            port->tail = NULL;
    }
    unlock();
    if(!pkt) {                                          //semaphore signalled without a packet: treat as timeout
        SetLastError(WAIT_TIMEOUT);
        return FALSE;
    }
    if(lpNumberOfBytesTransferred) *lpNumberOfBytesTransferred = pkt->bytes;
    if(lpCompletionKey)            *lpCompletionKey            = pkt->key;
    if(lpOverlapped)               *lpOverlapped               = pkt->ov;
    wr = pkt->status;
    free(pkt);
    if(wr != 0) {
        SetLastError(wr);                               //failed I/O: packet delivered with FALSE
        return FALSE;
    }
    return TRUE;
}

/*
 * Called by the file device when an overlapped request on hFile has completed.
 * Returns TRUE when a packet was queued (the handle is associated with a port).
 */
BOOL WIN32API OdinIocpComplete(HANDLE hFile, LPOVERLAPPED ov, DWORD bytes, DWORD status)
{
    IocpAssoc *a;
    BOOL       queued = FALSE;

    if(assocs == NULL)
        return FALSE;
    lock();
    for(a = assocs; a; a = a->next) {
        if(a->hFile == hFile) {
            enqueue(a->port, bytes, a->key, ov, status);
            queued = TRUE;
            break;
        }
    }
    unlock();
    return queued;
}

/* The file handle is closed: forget its port association. */
VOID WIN32API OdinIocpRemove(HANDLE hFile)
{
    IocpAssoc **pp, *a;

    if(assocs == NULL)
        return;
    lock();
    for(pp = &assocs; (a = *pp) != NULL; pp = &a->next) {
        if(a->hFile == hFile) {
            *pp = a->next;
            free(a);
            break;
        }
    }
    unlock();
}

} // extern "C"
