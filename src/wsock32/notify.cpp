/* $Id: notify.cpp,v 1.3 1999-08-16 20:18:39 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Winsock code
 *
 * Copyright 1998 Vince Vielhaber
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <os2win.h>
//#include <wsock32.h>
#include <misc.h>
#include <odinwrap.h>


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef unsigned int    u_int;
typedef u_int           SOCKET;

typedef unsigned long TID;

typedef struct AsyncStatus {
    HWND hwnd;     // owner's hwindow
    u_int msg;     // message to send when event occurs
    ULONG event;    // event that may occur
    SOCKET socket; // the socket
    int status;    // blocking yes/no
    TID threadID;  // Thread ID for async
    int MsgStat;   // has message been sent yet?
    struct AsyncStatus *Next; // pointer to next AsyncStatus in the list
    struct AsyncStatus *Prev; // pointer to previous AsyncStatus in the list
} AsyncStatus;



/*****************************************************************************
 * Macros                                                                    *
 *****************************************************************************/

/*
 * WSAMAKESELECTREPLY is intended for use by the Windows Sockets implementation
 * when constructing the response to WSAAsyncSelect().
 */

//#define OS2WSAMAKESELECTREPLY(event,error)     MAKELONG(event,error)
#ifdef MAKELONG
#undef MAKELONG
#endif
#define MAKELONG(a, b)      ((LONG)(((WORD)(a)) | ((DWORD)((WORD)(b))) << 16))
#define OS2WSAMAKESELECTREPLY(event,error)     MAKELONG(event,error)




int Notify(AsyncStatus *as, int event)
{
  int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: Open32 Notifying %x, %x, %d\n",
         (HWND)as->hwnd,as->msg,(int)as->socket);
#endif

    rc = PostMessageA(as->hwnd,as->msg,as->socket,OS2WSAMAKESELECTREPLY(event,0));

return rc;
}

// WPARAM is UINT, LPARAM is LONG

int NotifyWSA(HWND hw,u_int msg,UINT wp,LONG lp)
{
int rc;

#ifdef DEBUG
    WriteLog("WSOCK32: Open32 WSA-Notifying %x, %x\n",
         (HWND)hw,msg);
#endif

    rc = PostMessageA(hw,msg,(WPARAM)wp,(LPARAM)lp);

return rc;

}



