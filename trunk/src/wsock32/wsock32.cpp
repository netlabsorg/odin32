/* $Id: wsock32.cpp,v 1.4 1999-08-16 20:18:40 phaller Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 SOCK32 for OS/2
 *
 * 1998/08/25 Vince Vielhaber
 *
 * @(#) wsock32.c                        1.0.0   1998/08/25 VV initial release
 *                      1.0.1   1999/04/27 VV cleanup and implement select.
 *
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/


#define INCL_DOSPROCESS     /* Process and thread values */
#define INCL_DOSFILEMGR     /* for DosRead and DosWrite */
#define INCL_DOSQUEUES      /* for unnamed pipes */
#define INCL_DOSERRORS      /* DOS error values          */
#define INCL_WINMESSAGEMGR  /* Window Message Functions     */
#define INCL_WINMENUS       /* Window Menu Functions        */

/* this is for IBM TCP/IP 5.0 headers as present in the current Warp 4 toolkit */
#define TCPV40HDRS 1

#define VV_BSD_SELECT       /* undefine this if it interferes with other routines */

#ifdef VV_BSD_SELECT
# define BSD_SELECT
#endif

#include <os2wrap.h>                     //Odin32 OS/2 api wrappers
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <ctype.h>
#include <types.h>
#include <odinwrap.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>

#ifdef VV_BSD_SELECT
# include <sys/select.h>
#endif

#include <sys/time.h>
#include <win32type.h>
#include "wsock32.h"
#include "misc.h"


ODINDEBUGCHANNEL(WSOCK32-WSOCK32)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/

#define GETSERVBYNAME    1
#define GETSERVBYPORT    2
#define GETPROTOBYNAME   3
#define GETPROTOBYNUMBER 4
#define GETHOSTBYNAME    5
#define GETHOSTBYADDR    6

#define BLOCKING 0
#define NONBLOCKING 1

#ifdef FD_CLR
#undef FD_CLR
#define FD_CLR(x,y) WFD_CLR(x,y)
#undef FD_SET
#define FD_SET(x,y) WFD_SET(x,y)
#undef FD_ZERO
#define FD_ZERO(x) WFD_ZERO(x)
#undef FD_ISSET
#define FD_ISSET(x,y) WFD_SET(x,y)
#endif


/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

typedef struct WSAStruct
{
  int CallingWhat;
  HWND hw;
  u_int msg;
  char *carg1;
  char *carg2;
  int iarg1;
  int iarg2;
  char *buf;
  int buflen;
} WSAStruct;

typedef struct PipeStruct
{
  AsyncStatus as;
  int MsgLoop;
  HFILE rcv;
  HFILE snd;
} PipeStruct;

typedef struct sockaddr* PSOCKADDR;

//typedef struct _TRANSMIT_FILE_BUFFERS {
//    PVOID Head;
//    DWORD HeadLength;
//    PVOID Tail;
//    DWORD TailLength;
//} TRANSMIT_FILE_BUFFERS, *PTRANSMIT_FILE_BUFFERS, *LPTRANSMIT_FILE_BUFFERS;
//
//BOOL, OS2TransmitFile, //, IN, SOCKET, hSocket, //, IN, HANDLE, hFile, //, IN, DWORD, nNumberOfBytesToWrite, //, IN DWORD nNumberOfBytesPerSend,
//    IN LPOVERLAPPED lpOverlapped,
//    IN LPTRANSMIT_FILE_BUFFERS lpTransmitBuffers,
//    IN DWORD dwReserved)
//{
//    return FALSE;
//}


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

void _System AsyncLoop             (ULONG);
void         CheckThreads          (AsyncStatus *);
void         NotifyApp             (int,AsyncStatus *);
int          Notify                (AsyncStatus *,int);
int          NotifyWSA             (HWND hw,u_int msg,UINT wp,LONG lp);
void _System WSAFunct              (ULONG); // for the wsa database calls
void         SetErrForDatabaseCalls(void);


/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

static WHOSTENT whsnt;
static WSERVENT wsvnt;
static WPROTOENT wptnt;
size_t nSize;
int *pArray;

int MsgSent;
int LocalErrorNumber = 0;

TID tidAsyncLoop = 0;     /* ID of AsyncSelect (AsyncLoop) thread */

PipeStruct PS;

AsyncStatus *TopASY = 0;


/*
typedef struct AsyncStatus {
    HWND hwnd;     // owner's hwindow
    u_int msg;     // message to send when event occurs
    long event;    // event that may occur
    SOCKET socket; // the socket
    int status;    // blocking yes/no
    TID threadID;  // Thread ID for async
    int MsgStat;   // has message been sent yet?
    struct AsyncStatus *Next; // pointer to next AsyncStatus in the list
    struct AsyncStatus *Prev; // pointer to previous AsyncStatus in the list
} AsyncStatus;
*/


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

void _System AsyncLoop(ULONG ASP)
{
  int socks[1],r,w,e,rc,ii;
  AsyncStatus *as;

  as = (AsyncStatus *)ASP;

  r = w = e = 0;
  if(as->event & FD_READ) r = 1;
  if(as->event & FD_WRITE) w = 1;
  if(as->event & FD_OOB) e = 1;

  socks[0] = (int)as->socket;

  if((r+w+e) == 0)
  {
    dprintf(("WSOCK32: Turning off async\n"));

    ii = 0;
    rc = ioctl(socks[0],FIONBIO,(char *)&ii,sizeof(ii));
    as->threadID = 0;
    as->hwnd = 0;
    as->msg = 0;
    as->event = 0;
    as->status = BLOCKING;
    return;
  } // end if
  else
  {
    dprintf(("WSOCK32: Setting up non-blocking sockets\n"));
    ii = 1;
    rc = ioctl(socks[0],FIONBIO,(char *)&ii,sizeof(ii));
    if(rc != 0)
    {
      dprintf(("WSOCK32: ioctl failed trying to non-block.\n"));
      return;
    }
    as->status = NONBLOCKING;
  } // end else

  do
  {
    rc = select(socks[0],(fd_set*)&r,0,0,0); // ioctl may be better for this.
    if(rc > 0)
    {
      rc = ioctl(socks[0],FIONREAD,(char *)&ii,sizeof(ii));
      if(rc == 0 && ii > 0)
      {
        /* data is ready */
        NotifyApp(FD_READ,as);
      }
    }

    if(rc < 0)
    {
      rc = sock_errno();
      /* something ain't right */
      if(rc == 10038)
      { // Connection closed
        NotifyApp(FD_CLOSE,as);
        DosSleep(500);
        return;
      }

      dprintf(("WSOCK32: Select error: %d\n",
               rc));
    } // end if
    DosSleep(50);
  }
  while(1);
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

void CheckThreads(AsyncStatus *as)
{
  AsyncStatus *asy;

  if(as != NULL)
    if(as->threadID != 0) DosKillThread(as->threadID);

  for(asy = TopASY; asy; asy = asy->Next)
    if(asy->threadID != 0)
      DosKillThread(asy->threadID);
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

void NotifyApp(int xx,AsyncStatus *as)
{
  BOOL    fResult;        /* message-posted indicator             */
  unsigned long ii;


//#ifdef DEBUG
//    WriteLog("WSOCK32: Notifying the caller.  rc = %d\n",xx);
//#endif

  if(as->MsgStat == 0)
  {
   fResult = Notify(as,xx);
   dprintf(("WSOCK32: Notify returns: %d\n",
            fResult));
  } // end if

  if(as->MsgStat == 2)
    as->MsgStat = 0;
  else
    as->MsgStat = 1;
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

void _System WSAFunct(ULONG xx)
{
  WSAStruct *wsa;
  WSERVENT *ooo;
  char *yy;
  int ii;
  size_t ss;
  UINT wp;
  LONG lp;
  int id = *_threadid;

  wsa = (WSAStruct *)xx;

  dprintf(("WSOCK32: WSAFunct: xx = %p, hwnd = %p\n",
           xx,
           wsa->hw));

  dprintf(("WSOCK32: WSAFunct info carg1 = %s, carg2 = %s\n",
           wsa->carg1,
           wsa->carg2));

  dprintf(("WSOCK32: WSAFunct info buf = %p, %d\n",
           wsa->buf,
           wsa->buflen));

  switch (wsa->CallingWhat)
  {
    case GETSERVBYNAME:
      yy = (char *)OS2getservbyname(wsa->carg1,wsa->carg2);
      ss = sizeof(WSERVENT);
      break;
    case GETSERVBYPORT:
      yy = (char *)OS2getservbyport(wsa->iarg1,wsa->carg1);
      break;
    case GETPROTOBYNUMBER:
      yy = (char *)OS2getprotobynumber(wsa->iarg1);
      break;
    case GETPROTOBYNAME:
      yy = (char *)OS2getprotobyname(wsa->carg1);
      break;
    case GETHOSTBYNAME:
      yy = (char *)OS2gethostbyname(wsa->carg1);
      break;
    case GETHOSTBYADDR:
      yy = (char *)OS2gethostbyaddr(wsa->carg1,wsa->iarg1,wsa->iarg2);
      break;
    default:
      yy = (char *)NULL;
      OS2WSASetLastError(-5000);
      break;
  } // end switch

#ifdef DEBUG
  if(yy)
  {
    ooo = (WSERVENT *)yy;
    dprintf(("WSOCK32: WSAFunct service name = %s, port = %d\n",
             ooo->s_name,
             (int)ooo->s_port));
  }
#endif

  wp = id;

  if(yy == (char *)NULL)
  {
    dprintf(("WSOCK32: WSAFunct error carg1 = %s, carg2 = %s\n",
             wsa->carg1,
             wsa->carg2));

   ii = OS2WSAGetLastError();
   lp = OS2WSAMAKEASYNCREPLY(0,ii);
  } // end if
  else
  {
    if(wsa->buflen < ss)
      ii = WSAENOBUFS;
    else
      ii = 0;

    lp = OS2WSAMAKEASYNCREPLY(ss,ii);
    if(ii == 0)
      memmove(wsa->buf,yy,ss);
  }

  do
  {
    if(WinQueryAnchorBlock(wsa->hw))
          ii = NotifyWSA(wsa->hw,wsa->msg,wp,lp);
  } while(ii != TRUE);

  free(wsa);
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

void SetErrForDatabaseCalls(void)
{
  switch(h_errno)
  {
    case 1:
      OS2WSASetLastError(11001); // host not found
      break;
    case 2:
      OS2WSASetLastError(11002); // try again later
      break;
    case 3:
      OS2WSASetLastError(11003); // No recovery
      break;
    case 4:
      OS2WSASetLastError(11004); // No address or no data
      break;
    default:
      OS2WSASetLastError(0); // unknown error and should never get here
      dprintf(("WSOCK32: Unknown error condition: %d\n",
               h_errno));
      break;
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

AsyncStatus * FindASY(SOCKET s)
{
  AsyncStatus *as;

  for(as = TopASY; as; as = as->Next)
    if(as->socket == s)
      return as;

  return 0;
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

ODINFUNCTION2(int, OS2__WSAFDIsSet, SOCKET,       fd,
                                    Wfd_set FAR*, set)
{
  int i = set->fd_count;

  while (i--)
    if (set->fd_array[i] == fd)
      return 1;

  return 0;
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

ODINFUNCTION3(SOCKET,OS2accept,SOCKET,    s,
                               PSOCKADDR, addr,
                               int*,      addrlen)
{
  return accept(s,addr,addrlen);
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

ODINFUNCTION3(int,OS2bind,SOCKET,          s,
                          const struct sockaddr *, addr,
                          int,             namelen)
{
  return bind(s,(PSOCKADDR)addr,namelen);
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

ODINFUNCTION1(int,OS2closesocket,SOCKET,s)
{
  AsyncStatus *as;

  as = FindASY(s);
  if(as == NULL)
  {
    LocalErrorNumber = 10038;
    return -1;
  }

  CheckThreads(as);

  if(as->Prev && as->Next)
    as->Prev->Prev = as->Next->Next; // I SURE HOPE THIS IS RIGHT!!!!!!!!

  free(as);

  return soclose((int)s);
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

ODINFUNCTION3(int,OS2connect,SOCKET,          s,
                             const struct sockaddr*, name,
                             int,             namelen)
{
  return connect(s,
                 (PSOCKADDR)name,
                 namelen);
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

ODINFUNCTION3(int,OS2ioctlsocket,SOCKET,  s,
                                 long,    cmd,
                                 u_long*, argp)
{
  return ioctl(s, cmd, (char *)argp, 4);
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

ODINFUNCTION3(int,OS2getpeername,SOCKET,   s,
                                 PSOCKADDR,name,
                                 int*,     namelen)
{
  return getpeername(s,name,namelen);
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

ODINFUNCTION3(int,OS2getsockname,SOCKET,   s,
                                 PSOCKADDR,name,
                                 int*,     namelen)
{
  return getsockname(s,name,namelen);
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

ODINFUNCTION5(int,OS2getsockopt,SOCKET,s,
                                int,   level,
                                int,   optname,
                                char*, optval,
                                int*,  optlen)
{
  return getsockopt(s,level,optname,optval,optlen);
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

ODINFUNCTION1(u_long,OS2htonl,u_long,hostlong)
{
  return htonl(hostlong);
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

ODINFUNCTION1(u_short,OS2htons,u_short,hostshort)
{
  return htons(hostshort);
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

ODINFUNCTION1(u_long,OS2inet_addr,const char*, cp)
{
  return inet_addr((char *)cp);
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

ODINFUNCTION1(char*,OS2inet_ntoa,struct in_addr,in)
{
  return inet_ntoa(in);
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

ODINFUNCTION2(int,OS2listen,SOCKET,s,
                            int,   backlog)
{
  return listen(s,backlog);
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

ODINFUNCTION1(u_long,OS2ntohl,u_long,netlong)
{
  return ntohl(netlong);
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

ODINFUNCTION1(u_short,OS2ntohs,u_short,netshort)
{
  return ntohs(netshort);
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

ODINFUNCTION4(int,OS2recv,SOCKET,s,
                          char*, buf,
                          int,   len,
                          int,   flags)
{
  unsigned long ii;
  int           xx,
                yy;
  char          buff[200];
  AsyncStatus   *as;

  PS.MsgLoop = 0;

  as = FindASY(s);

  if(as != NULL)
    as->MsgStat = 2;

  xx = recv(s,buf,len,flags);

  return xx;
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

ODINFUNCTION6(int,OS2recvfrom,SOCKET,    s,
                              char*,     buf,
                              int,       len,
                              int,       flags,
                              PSOCKADDR, from,
                              int*,      fromlen)
{
  return recvfrom(s,buf,len,flags,from,fromlen);
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

#ifdef VV_BSD_SELECT

ODINFUNCTION5(int,OS2select,int,      nfds,
                            Wfd_set*, readfds,
                            Wfd_set*, writefds,
                            Wfd_set*, exceptfds,
                            const struct Wtimeval*, timeout)
{
  return select(nfds,
                (fd_set *)readfds,
                (fd_set *)writefds,
                (fd_set *)exceptfds,
                (timeval *)timeout);
}

#else
#  error OS/2-style select not implemented!
#endif


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

ODINFUNCTION4(int,OS2send,SOCKET,      s,
                          const char*, buf,
                          int,         len,
                          int,         flags)
{
  return send(s,(char *)buf,len,flags);
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

ODINFUNCTION6(int,OS2sendto,SOCKET,          s,
                            const char*,     buf,
                            int,             len,
                            int,             flags,
                            const struct sockaddr*, to,
                            int,             tolen)
{
  return sendto(s,(char *)buf,len,flags,(PSOCKADDR)to,tolen);
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

ODINFUNCTION5(int,OS2setsockopt,SOCKET,      s,
                                int,         level,
                                int,         optname,
                                const char*, optval,
                                int,         optlen)
{
  struct Wlinger *yy;
  struct linger xx;

  if(level   == SOL_SOCKET &&
     optname == SO_LINGER)
  {
    yy = (struct Wlinger *)optval;
    xx.l_onoff = (int)yy->l_onoff;
    xx.l_linger = (int)yy->l_linger;

    return setsockopt(s,level,optname,(char *)&xx,optlen);
  }
  else
    return setsockopt(s,level,optname,(char *)optval,optlen);
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

ODINFUNCTION2(int,OS2shutdown,SOCKET,s,
                              int,   how)
{
  return shutdown(s,how);
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

ODINFUNCTION3(SOCKET,OS2socket,int,af,
                               int,type,
                               int,protocol)
{
  SOCKET s;
  AsyncStatus *as;

  s = (SOCKET)socket(af,type,protocol);
  if(s > 0)
  {
    as = (AsyncStatus *)malloc(sizeof(AsyncStatus));
    if(as != NULL)
    {
      as->hwnd = (HWND)0;
      as->msg = 0;
      as->event = 0L;
      as->socket = s;
      as->status = BLOCKING;
      as->threadID = 0;
      as->MsgStat = 0;
      as->Next = TopASY;
      as->Prev = NULL;

      if(TopASY)
        TopASY->Prev = as;

      TopASY = as;
    }
    else
    {
      soclose(s);
      return -1;
    }
  }

  return s;
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

ODINFUNCTION3(WHOSTENT*,OS2gethostbyaddr,const char*, addr,
                                         int,         len,
                                         int,         type)
{
  WHOSTENT *yy;
  struct hostent *xx;

  xx = gethostbyaddr((char *)addr,len,type);

  if(xx == NULL)
  {
     SetErrForDatabaseCalls();
     return (WHOSTENT *)NULL;
  }

  whsnt.h_name = xx->h_name;
  whsnt.h_aliases = xx->h_aliases;
  whsnt.h_addrtype = (short)xx->h_addrtype;
  whsnt.h_length = (short)xx->h_length;
  whsnt.h_addr_list = xx->h_addr_list;

  return &whsnt;
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

ODINFUNCTION1(WHOSTENT*,OS2gethostbyname,const char*,name)
{
  WHOSTENT *yy;
  struct hostent *xx;

  xx = gethostbyname((char *)name);
  if(xx == NULL)
  {
    SetErrForDatabaseCalls();
    return (WHOSTENT *)NULL;
  }

  whsnt.h_name = xx->h_name;
  whsnt.h_aliases = xx->h_aliases;
  whsnt.h_addrtype = (short)xx->h_addrtype;
  whsnt.h_length = (short)xx->h_length;
  whsnt.h_addr_list = xx->h_addr_list;

  return &whsnt;
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

ODINFUNCTION2(int,OS2gethostname,char *,name,
                                 int,   namelen)
{
  return gethostname(name,namelen);
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

ODINFUNCTION2(WSERVENT*,OS2getservbyport, int,         port,
                                          const char*, proto)
{
  struct servent *xx;

  xx = getservbyport(port,(char *)proto);

  if(xx == NULL)
  { // this call doesn't generate an error message
    return (WSERVENT *)NULL;
  }

  wsvnt.s_name = xx->s_name;
  wsvnt.s_aliases = xx->s_aliases;
  wsvnt.s_port = (short)xx->s_port;
  wsvnt.s_proto = xx->s_proto;

  return &wsvnt;
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

ODINFUNCTION2(WSERVENT*,OS2getservbyname,const char*,name,
                                         const char*,proto)
{
  WSERVENT *yy;
  struct servent *xx;

  xx = getservbyname((char *)name,(char *)proto);

  if(xx == NULL)
  { // this call doesn't generate an error message
    return (WSERVENT *)NULL;
  }

  wsvnt.s_name = xx->s_name;
  wsvnt.s_aliases = xx->s_aliases;
  wsvnt.s_port = (short)xx->s_port;
  wsvnt.s_proto = xx->s_proto;

  return &wsvnt;
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

ODINFUNCTION1(WPROTOENT*,OS2getprotobynumber,int,proto)
{
  struct protoent *xx;

  xx = getprotobynumber(proto);

  if(xx == NULL)
  {
     // this call doesn't generate an error message
    return (WPROTOENT *)NULL;
  }

  wptnt.p_name = xx->p_name;
  wptnt.p_aliases = xx->p_aliases;
  wptnt.p_proto = (short)xx->p_proto;

  return &wptnt;
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

ODINFUNCTION1(WPROTOENT*,OS2getprotobyname,const char*,name)
{
  struct protoent *xx;

  xx = getprotobyname((char *)name);

  if(xx == NULL)
  { // this call doesn't generate an error message
    return (WPROTOENT *)NULL;
  }

  wptnt.p_name = xx->p_name;
  wptnt.p_aliases = xx->p_aliases;
  wptnt.p_proto = (short)xx->p_proto;

  return &wptnt;
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

ODINFUNCTION2(int,OS2WSAStartup,USHORT,   wVersionRequired,
                                LPWSADATA,lpWsaData)
{
  APIRET rc;
  int ii;

  /* Make sure that the version requested is >= 1.1.   */
  /* The low byte is the major version and the high    */
  /* byte is the minor version.                        */

  if ( LOBYTE( wVersionRequired ) < 1 ||
       ( LOBYTE( wVersionRequired ) == 1 &&
         HIBYTE( wVersionRequired ) < 1 )) {
      return WSAVERNOTSUPPORTED;
  }

  /* Since we only support 1.1, set both wVersion and  */
  /* wHighVersion to 1.1.                              */

  lpWsaData->wVersion = MAKEWORD( 1, 1 );
  lpWsaData->wHighVersion = MAKEWORD( 1, 1 );
  strcpy(lpWsaData->szDescription,"Win32OS2 WSOCK32.DLL Ver. 1.1");
  lpWsaData->iMaxUdpDg = 32767;
  lpWsaData->iMaxSockets = 2048;
  strcpy(lpWsaData->szSystemStatus,"No Status");

  LocalErrorNumber = 0;

  if(sock_init() == 0)
  {
#ifdef DEBUG
    WriteLog("WSOCK32: WSAStartup sock_init returned 0\n");
#endif
      return 0;
  }
  else
    ii = sock_errno();

  return ii;
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

ODINFUNCTION0(int,OS2WSACleanup)
{
  CheckThreads((AsyncStatus *)NULL);

  return 0;
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

ODINPROCEDURE1(OS2WSASetLastError,int,iError)
{
  LocalErrorNumber = iError;
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

ODINFUNCTION0(int,OS2WSAGetLastError)
{
  int ii;

  if(LocalErrorNumber == 0)
  {
    ii = sock_errno(); // WSAGetLastError();
    return ii;
  }
  else
  {
    return LocalErrorNumber;
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

ODINFUNCTION0(BOOL,OS2WSAIsBlocking)
{
  dprintf(("WSOCK32: WSAIsBlocking unimplemented\n"));

  return -5000; //WSAIsBlocking();
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

ODINFUNCTION0(int,OS2WSAUnhookBlockingHook)
{
  dprintf(("WSOCK32: WSAUnhookBlockingHook unimplemented\n"));

  return -5000; //WSAUnhookBlockingHook();
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

ODINFUNCTION1(PROC,OS2WSASetBlockingHook,PROC,lpBlockFund)
{
  dprintf(("WSOCK32: WSASetBlockingHook Unimplemented\n"));
  return (PROC)NULL;
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

ODINFUNCTION0(int,OS2WSACancelBlockingCall)
{
  dprintf(("WSOCK32: WSACancelBlockingCall unimplemented\n"));

  return -5000; //WSACancelBlockingCall();
}


// The following 6 calls need to start a new thread, perform
// the operation, copy ALL the info to the buffer provided then
// notify the sender.

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

ODINFUNCTION6(LHANDLE,OS2WSAAsyncGetServByName,HWND,        hWnd,
                                               u_int,       wMsg,
                                               const char*, name,
                                               const char*, proto,
                                               char*,       buf,
                                               int,         buflen)
{
  WSAStruct *wsa;
  PFNTHREAD   pfnAsyncThread = &WSAFunct; /* Address of thread program   */
  ULONG       ulThreadParm = 100;          /* Parameter to thread routine     */
  APIRET      rc           = NO_ERROR;     /* Return code */
  TID         tid;


  OS2WSASetLastError(WSAEWOULDBLOCK);
  return 0;

  dprintf(("WSOCK32: WSAAsyncGetServByName.  name: %s, proto: %s \n",
           name,
           proto));

  wsa = (WSAStruct *)malloc(sizeof(WSAStruct));
  if(wsa == NULL)
  {
    OS2WSASetLastError(WSAEWOULDBLOCK);
    return 0;
  }

  wsa->CallingWhat = GETSERVBYNAME;
  wsa->hw = hWnd;
  wsa->msg = wMsg;
  wsa->carg1 = strdup(name);
  wsa->carg2 = strdup(proto);
  wsa->buf = buf;
  wsa->buflen = buflen;

  ulThreadParm = (ULONG)wsa;

  rc = DosCreateThread(&tid,           /* Thread ID (returned by function)  */
                       pfnAsyncThread, /* Address of thread program         */
                       ulThreadParm,   /* Parameter passed to ThreadProc    */
                       CREATE_READY |  /* Thread is ready when created      */
                       STACK_SPARSE,   /* Do not pre-commit stack pages     */
                       8192L);         /* Stack size, rounded to page bdy   */
  if (rc != NO_ERROR)
  {
    dprintf(("WSOCK32: DosCreateThread error in WSAAsyncGetServByName: return code = %u\n",
            rc));
    OS2WSASetLastError(rc);
    free(wsa);
    return 0;
  }

  dprintf(("WSOCK32 THREAD: DosCreateThread's tid: %lu, ThreadParm = %p\n",
           (unsigned long)tid,
           ulThreadParm));
  dprintf(("WSOCK32 THREAD: hwnd: %p\n",wsa->hw));

  return (LHANDLE)tid; //WSAAsyncGetServByName(hWnd,wMsg,name,proto,buf,buflen);
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

ODINFUNCTION6(LHANDLE,OS2WSAAsyncGetServByPort,HWND,        hWnd,
                                               u_int,       wMsg,
                                               int,         port,
                                               const char*, proto,
                                               char*,       buf,
                                               int,         buflen)
{
  dprintf(("WSOCK32: WSAAsyncGetServByPort unimplemented\n"));

  return -5000; //WSAAsyncGetServByPort(hWnd,wMsg,port,proto,buf,buflen);
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

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetProtoByName,HWND,        hWnd,
                                                u_int,       wMsg,
                                                const char*, name,
                                                char *,      buf,
                                                int,         buflen)
{
  dprintf(("WSOCK32: WSAAsyncGetProtoByName unimplemented\n"));
  return -5000; //WSAAsyncGetProtoByName(hWnd,wMsg,name,buf,buflen);
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

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetProtoByNumber,HWND,   hWnd,
                                                  u_int,  wMsg,
                                                  int,    number,
                                                  char *, buf,
                                                  int,    buflen)
{
  dprintf(("WSOCK32: WSAAsyncGetProtoByNumber unimplemented\n"));
  return -5000; //WSAAsyncGetProtoByNumber(hWnd,wMsg,number,buf,buflen);
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

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetHostByName,HWND,         hWnd,
                                               u_int,        wMsg,
                                               const char *, name,
                                               char *,       buf,
                                               int,          buflen)
{
  dprintf(("WSOCK32: WSAAsyncGetHostByName unimplemented\n"));
  return -5000; //WSAAsyncGetHostByName(hWnd,wMsg,name,buf,buflen);
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

ODINFUNCTION7(LHANDLE, OS2WSAAsyncGetHostByAddr, HWND,        hWnd,
                                                 u_int,       wMsg,
                                                 const char*, addr,
                                                 int,         len,
                                                 int,         type,
                                                 char*,       buf,
                                                 int,         buflen)
{
  dprintf(("WSOCK32: WSAAsyncGetHostByAddr unimplemented\n"));

  return -5000; //WSAAsyncGetHostByAddr(hWnd,wMsg,addr,len,type,buf,buflen);
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

ODINFUNCTION1(int, OS2WSACancelAsyncRequest,LHANDLE,hAsyncTaskHandle)
{
  TID    tid;
  APIRET rc;

  dprintf(("WSOCK32: WSACancelAsyncRequest unimplemented\n"));

  tid = (LHANDLE)hAsyncTaskHandle;
  if(tid == 0)
    rc = WSAEINVAL;
  else
    rc = DosKillThread(tid);

  switch(rc)
  {
    case 0: // SUCCESS!!
      return 0;
    case 170:
      rc = WSAEINPROGRESS;
      break;
    case 309:
      rc = WSAEINVAL;
      break;
    default:
      rc = -5000;
      break;
  } // end switch

  OS2WSASetLastError(rc);
  return SOCKET_ERROR; // ERROR!
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

ODINFUNCTION4(int, OS2WSAAsyncSelect, SOCKET, s,
                                      HWND,   hWnd,
                                      u_int,  wMsg,
                                      long,   lEvent)
{
  PFNTHREAD   pfnAsyncThread = &AsyncLoop; /* Address of thread program   */
  ULONG       ulThreadParm = 100;          /* Parameter to thread routine     */
  APIRET      rc           = NO_ERROR;     /* Return code                     */
  unsigned long ii;
  AsyncStatus *as;

  as = FindASY(s);
  if(as == NULL)
    return 0;

  CheckThreads(as);

  as->hwnd = hWnd;
  as->msg = wMsg;
  as->event = lEvent;

  ulThreadParm = (ULONG)as;

  rc = DosCreateThread(&(as->threadID),  /* Thread ID (returned by function)  */
                       pfnAsyncThread, /* Address of thread program         */
                       ulThreadParm,   /* Parameter passed to ThreadProc    */
                       CREATE_READY |  /* Thread is ready when created      */
                       STACK_SPARSE,   /* Do not pre-commit stack pages     */
                       8192L);         /* Stack size, rounded to page bdy   */
  if (rc != NO_ERROR)
  {
    dprintf(("WSOCK32: DosCreateThread error: return code = %u\n",
             rc));
    OS2WSASetLastError(rc);
    return 0;
  }

  return 1; //WSAAsyncSelect(s,hWnd,wMsg,lEvent);
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

ODINFUNCTION4(int, OS2WSARecvEx, SOCKET,    s,
                                 char FAR*, buf,
                                 int,       len,
                                 int FAR *,flags)
{
  dprintf(("WSOCK32: WSARecvEx not implemented.\n"));

//    return WSARecvEx(s,buf,len,flags);
  return 0;
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

ODINPROCEDURE2(OS2s_perror, char*, pszMessage,
                            void*, pUnknown)
{
  perror(pszMessage);
}


