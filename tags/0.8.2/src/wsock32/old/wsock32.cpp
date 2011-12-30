/* $Id: wsock32.cpp,v 1.1 1999-12-07 20:19:57 achimha Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 SOCK32 for OS/2
 *
 * 1998/08/25 Vince Vielhaber
 *
 * @(#) wsock32.c       1.0.0   1998/08/25 VV initial release
 *                      1.0.1   1999/04/27 VV cleanup and implement select.
 *
 */

/* Remark:
 * PH 1999/11/10 memory leak as WSOCKTHREADDATA is NOT yet
 *               freed when thread dies!
 * PH 1999/11/10 asyncSelect() still left to proper implementation
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
#include <wprocess.h>
#include <heapstring.h>

#include "wsock32const.h"
#include "wsock32.h"
#include "misc.h"


ODINDEBUGCHANNEL(WSOCK32-WSOCK32)


/*****************************************************************************
 * Defines                                                                   *
 *****************************************************************************/


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

#ifndef ERROR_SUCCESS
#define ERROR_SUCCESS 0
#endif



/*****************************************************************************
 * Structures                                                                *
 *****************************************************************************/

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


/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

static WSOCKTHREADDATA wstdFallthru; // for emergency only


/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    : free memory when thread dies
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

PWSOCKTHREADDATA iQueryWsockThreadData(void)
{
  struct _THDB*     pThreadDB = (struct _THDB*)GetThreadTHDB();
  PWSOCKTHREADDATA pwstd;

  // check for existing pointer
  if (pThreadDB != NULL)
  {
    if (pThreadDB->pWsockData == NULL)
    {
      // allocate on demand + initialize
      pwstd = (PWSOCKTHREADDATA)HEAP_malloc (sizeof(WSOCKTHREADDATA));
      pThreadDB->pWsockData = (LPVOID)pwstd;
    }
    else
      // use already allocated memory
      pwstd = (PWSOCKTHREADDATA)pThreadDB->pWsockData;
  }

  if (pwstd == NULL)
    pwstd = &wstdFallthru; // no memory and not allocated already

  return pwstd;
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

#define CASEERR2(a) case SOC##a: case a: return WSA##a;
#define CASEERR1(a) case SOC##a: return WSA##a;

int iTranslateSockErrToWSock(int iError)
{
  switch(iError)
  {
    CASEERR2(EINTR)
    CASEERR2(EBADF)
    CASEERR2(EACCES)
    CASEERR2(EINVAL)
    CASEERR2(EMFILE)

    CASEERR1(EWOULDBLOCK)
    CASEERR1(EINPROGRESS)
    CASEERR1(EALREADY)
    CASEERR1(ENOTSOCK)
//  CASEERR1(EDESTADRREQ)
    CASEERR1(EMSGSIZE)
    CASEERR1(EPROTOTYPE)
    CASEERR1(ENOPROTOOPT)
    CASEERR1(EPROTONOSUPPORT)
    CASEERR1(ESOCKTNOSUPPORT)
    CASEERR1(EOPNOTSUPP)
    CASEERR1(EPFNOSUPPORT)
    CASEERR1(EAFNOSUPPORT)
    CASEERR1(EADDRINUSE)
    CASEERR1(EADDRNOTAVAIL)
    CASEERR1(ENETDOWN)
    CASEERR1(ENETUNREACH)
    CASEERR1(ENETRESET)
    CASEERR1(ECONNABORTED)
    CASEERR1(ECONNRESET)
    CASEERR1(ENOBUFS)
    CASEERR1(EISCONN)
    CASEERR1(ENOTCONN)
    CASEERR1(ESHUTDOWN)
    CASEERR1(ETOOMANYREFS)
    CASEERR1(ETIMEDOUT)
    CASEERR1(ECONNREFUSED)
    CASEERR1(ELOOP)
    CASEERR1(ENAMETOOLONG)
    CASEERR1(EHOSTDOWN)
    CASEERR1(EHOSTUNREACH)

    CASEERR1(ENOTEMPTY)
//    CASEERR(EPROCLIM)
//    CASEERR(EUSERS)
//    CASEERR(EDQUOT)
//    CASEERR(ESTALE)
//    CASEERR(EREMOTE)
//    CASEERR(EDISCON)


    default:
      dprintf(("WSOCK32: Unknown error condition: %d\n",
               iError));
      return iError;
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
  SOCKET rc = accept(s,addr,addrlen);

  if (rc == INVALID_SOCKET)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(int,OS2bind,SOCKET,          s,
                          const struct sockaddr *, addr,
                          int,             namelen)
{
  int rc = bind(s,(PSOCKADDR)addr,namelen);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);
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

ODINFUNCTION1(int,OS2closesocket,SOCKET,s)
{
  int rc = soclose((int)s);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(int,OS2connect,SOCKET,          s,
                             const struct sockaddr*, name,
                             int,             namelen)
{
  int rc = connect(s,
                   (PSOCKADDR)name,
                   namelen);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(int,OS2ioctlsocket,SOCKET,  s,
                                 long,    cmd,
                                 u_long*, argp)
{
  int rc = ioctl(s, cmd, (char *)argp, 4);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(int,OS2getpeername,SOCKET,   s,
                                 PSOCKADDR,name,
                                 int*,     namelen)
{
  SOCKET rc = getpeername(s,name,namelen);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(int,OS2getsockname,SOCKET,   s,
                                 PSOCKADDR,name,
                                 int*,     namelen)
{
  SOCKET rc = getsockname(s,name,namelen);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION5(int,OS2getsockopt,SOCKET,s,
                                int,   level,
                                int,   optname,
                                char*, optval,
                                int*,  optlen)
{
  int rc = getsockopt(s,level,optname,optval,optlen);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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
  int rc = listen(s,backlog);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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
  int rc = recv(s,buf,len,flags);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION6(int,OS2recvfrom,SOCKET,    s,
                              char*,     buf,
                              int,       len,
                              int,       flags,
                              PSOCKADDR, from,
                              int*,      fromlen)
{
  int rc = recvfrom(s,buf,len,flags,from,fromlen);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

#ifdef VV_BSD_SELECT

ODINFUNCTION5(int,OS2select,int,      nfds,
                            Wfd_set*, readfds,
                            Wfd_set*, writefds,
                            Wfd_set*, exceptfds,
                            const struct Wtimeval*, timeout)
{
  int rc = select(nfds,
                (fd_set *)readfds,
                (fd_set *)writefds,
                (fd_set *)exceptfds,
                (timeval *)timeout);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

  return rc;
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
  int rc = send(s,(char *)buf,len,flags);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION6(int,OS2sendto,SOCKET,          s,
                            const char*,     buf,
                            int,             len,
                            int,             flags,
                            const struct sockaddr*, to,
                            int,             tolen)
{
  int rc = sendto(s,(char *)buf,len,flags,(PSOCKADDR)to,tolen);

  if (rc < 0)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION5(int,OS2setsockopt,SOCKET,      s,
                                int,         level,
                                int,         optname,
                                const char*, optval,
                                int,         optlen)
{
  struct Wlinger *yy;
  struct linger xx;
  int    rc;

  if(level   == SOL_SOCKET &&
     optname == SO_LINGER)
  {
    yy = (struct Wlinger *)optval;
    xx.l_onoff = (int)yy->l_onoff;
    xx.l_linger = (int)yy->l_linger;

    rc = setsockopt(s,level,optname,(char *)&xx,optlen);
  }
  else
    rc = setsockopt(s,level,optname,(char *)optval,optlen);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION2(int,OS2shutdown,SOCKET,s,
                              int,   how)
{
  int rc = shutdown(s,how);

  if (rc == SOCKET_ERROR)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(SOCKET,OS2socket,int,af,
                               int,type,
                               int,protocol)
{
  SOCKET rc = socket(af,type,protocol);

  if (rc == INVALID_SOCKET)
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
  else
    WSASetLastError(ERROR_SUCCESS);

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

ODINFUNCTION3(WHOSTENT*,OS2gethostbyaddr,const char*, addr,
                                         int,         len,
                                         int,         type)
{
  WHOSTENT         *yy;
  struct hostent   *xx;
  PWSOCKTHREADDATA pwstd;

  xx = gethostbyaddr((char *)addr,len,type);

  if(xx == NULL)
  {
     WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
     return (WHOSTENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->whsnt.h_name      = xx->h_name;
  pwstd->whsnt.h_aliases   = xx->h_aliases;
  pwstd->whsnt.h_addrtype  = (short)xx->h_addrtype;
  pwstd->whsnt.h_length    = (short)xx->h_length;
  pwstd->whsnt.h_addr_list = xx->h_addr_list;

  return &pwstd->whsnt;
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
  WHOSTENT         *yy;
  struct hostent   *xx;
  PWSOCKTHREADDATA pwstd;


  xx = gethostbyname((char *)name);
  if(xx == NULL)
  {
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return (WHOSTENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->whsnt.h_name      = xx->h_name;
  pwstd->whsnt.h_aliases   = xx->h_aliases;
  pwstd->whsnt.h_addrtype  = (short)xx->h_addrtype;
  pwstd->whsnt.h_length    = (short)xx->h_length;
  pwstd->whsnt.h_addr_list = xx->h_addr_list;

  return &pwstd->whsnt;
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
  int rc = gethostname(name,namelen);

  if (rc == SOCKET_ERROR)
    WSASetLastError(ERROR_SUCCESS);
  else
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));

  return (rc);
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
  struct servent   *xx;
  PWSOCKTHREADDATA pwstd;

  xx = getservbyport(port,(char *)proto);

  if(xx == NULL)
  { // this call doesn't generate an error message
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return (WSERVENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->wsvnt.s_name    = xx->s_name;
  pwstd->wsvnt.s_aliases = xx->s_aliases;
  pwstd->wsvnt.s_port    = (short)xx->s_port;
  pwstd->wsvnt.s_proto   = xx->s_proto;

  return &pwstd->wsvnt;
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
  WSERVENT         *yy;
  struct servent   *xx;
  PWSOCKTHREADDATA pwstd;


  xx = getservbyname((char *)name,(char *)proto);

  if(xx == NULL)
  { // this call doesn't generate an error message
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return (WSERVENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->wsvnt.s_name    = xx->s_name;
  pwstd->wsvnt.s_aliases = xx->s_aliases;
  pwstd->wsvnt.s_port    = (short)xx->s_port;
  pwstd->wsvnt.s_proto   = xx->s_proto;

  return &pwstd->wsvnt;
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
  struct protoent  *xx;
  PWSOCKTHREADDATA pwstd;

  xx = getprotobynumber(proto);

  if(xx == NULL)
  {
     // this call doesn't generate an error message
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return (WPROTOENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->wptnt.p_name    = xx->p_name;
  pwstd->wptnt.p_aliases = xx->p_aliases;
  pwstd->wptnt.p_proto   = (short)xx->p_proto;

  return &pwstd->wptnt;
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
  struct protoent  *xx;
  PWSOCKTHREADDATA pwstd;

  xx = getprotobyname((char *)name);

  if(xx == NULL)
  { // this call doesn't generate an error message
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return (WPROTOENT *)NULL;
  }
  else
    WSASetLastError(ERROR_SUCCESS);

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->wptnt.p_name    = xx->p_name;
  pwstd->wptnt.p_aliases = xx->p_aliases;
  pwstd->wptnt.p_proto   = (short)xx->p_proto;

  return &pwstd->wptnt;
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

  if(sock_init() == 0)
  {
#ifdef DEBUG
    WriteLog("WSOCK32: WSAStartup sock_init returned 0\n");
#endif
    WSASetLastError(ERROR_SUCCESS);
    return 0;
  }
  else
  {
    WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    return(WSASYSNOTREADY);
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

ODINFUNCTION0(int,OS2WSACleanup)
{
  WSASetLastError(ERROR_SUCCESS);
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

ODINPROCEDURE1(WSASetLastError,int,iError)
{
  PWSOCKTHREADDATA pwstd = iQueryWsockThreadData();
  pwstd->dwLastError = iError;
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

ODINFUNCTION0(int,WSAGetLastError)
{
  PWSOCKTHREADDATA pwstd = iQueryWsockThreadData();
  return(pwstd->dwLastError);
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


