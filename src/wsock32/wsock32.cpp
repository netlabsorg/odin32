/* $Id: wsock32.cpp,v 1.18 2000-03-13 20:32:24 sandervl Exp $ */

/*
 *
 * Project Odin Software License can be found in LICENSE.TXT
 *
 * Win32 SOCK32 for OS/2
 *
 * Copyright (C) 1999 Patrick Haller <phaller@gmx.net>
 *
 */

/* Remark:
 * 1999/11/21 experimental rewrite using IBM's PMWSock only
 *            -> some structural differences remain! (hostent)
 * 1999/12/01 experimental rewrite works (TELNET)
 *            -> open issue: WSASetLastError / WSAGetLastError
 *               call SetLastError / GetLastError according to docs
 *
 * identical structures:
 * - sockaddr_in
 * - WSADATA
 * - sockaddr
 * - fd_set
 * - timeval
 *
 * incompatible structures:
 * - hostent
 * - netent
 * - servent
 * - protent
 * - linger
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <pmwsock.h>
#include <odin.h>
#include <odinwrap.h>
#include <os2sel.h>
#include <misc.h>
#include <wprocess.h>
#include <heapstring.h>
#include <win32wnd.h>
#include <stdlib.h>

#include "wsock32.h"
#include "relaywin.h"


ODINDEBUGCHANNEL(WSOCK32-WSOCK32)


/*****************************************************************************
 * Local variables                                                           *
 *****************************************************************************/

#define ERROR_SUCCESS 0


static WSOCKTHREADDATA wstdFallthru; // for emergency only

static HWND hwndRelay = NULL; // handle to our relay window


/*****************************************************************************
 * Prototypes                                                                *
 *****************************************************************************/

void __stdcall SetLastError(DWORD dwError);


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


#if 0
/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/


ODINPROCEDURE1(OS2WSASetLastError,
               int,iError)
{
  // according to the docs, WSASetLastError() is just a call-through
  // to SetLastError()
  WSASetLastError(iError);
  SetLastError(iError);
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION0(int,OS2WSAGetLastError)
{
  // according to the docs, WSASetLastError() is just a call-through
  // to SetLastError(). However, what can be implemented here?
  return WSAGetLastError();
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(int,OS2__WSAFDIsSet,SOCKET, s,
                                  fd_set*,fds)
{
  return (__WSAFDIsSet(s,fds));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(SOCKET,OS2accept, SOCKET,           s,
                                struct sockaddr *,addr,
                                int *,            addrlen)
{
  return(accept(s,addr,addrlen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(int,OS2bind,
              SOCKET ,s,
              const struct sockaddr *,addr,
              int, namelen)
{
  return(bind(s,addr,namelen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(int,OS2closesocket,SOCKET, s)
{
  return(closesocket(s));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(int,OS2connect,
              SOCKET, s,
              const struct sockaddr *,name,
              int, namelen)
{
  return(connect(s,name,namelen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(int,OS2ioctlsocket,
              SOCKET,s,
              long, cmd,
              u_long *,argp)
{
  return(ioctlsocket(s,cmd,argp));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(int,OS2getpeername,
              SOCKET, s,
              struct sockaddr *,name,
              int *, namelen)
{
  return(getpeername(s,name,namelen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(int,OS2getsockname,
              SOCKET,s,
              struct sockaddr *,name,
              int *, namelen)
{
  return(getsockname(s,name,namelen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(int,OS2getsockopt,
              SOCKET, s,
              int, level,
              int, optname,
              char *, optval,
              int *,optlen)
{
  return(getsockopt(s,
                    level,
                    optname,
                    optval,
                    optlen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(u_long,OS2htonl,
              u_long,hostlong)
{
  return(htonl(hostlong));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(u_short,OS2htons,
              u_short,hostshort)
{
  return(htons(hostshort));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(unsigned long,OS2inet_addr,
              const char *, cp)
{
  dprintf(("WSOCK32: OS2inet_addr(%s)\n",
           cp));

  return (inet_addr(cp));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(char *,OS2inet_ntoa,
              struct in_addr, in)
{
  return(inet_ntoa(in));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(int,OS2listen,
              SOCKET, s,
              int, backlog)
{
  return(listen(s,backlog));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(u_long,OS2ntohl,
              u_long,netlong)
{
  return(ntohl(netlong));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(u_short,OS2ntohs,
              u_short,netshort)
{
  return(ntohs(netshort));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION4(int,OS2recv,
              SOCKET,s,
              char *,buf,
              int,len,
              int,flags)
{
  return(recv(s,
              buf,
              len,
              flags));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION6(int,OS2recvfrom,
              SOCKET,s,
              char *,buf,
              int,len,
              int,flags,
              struct sockaddr *,from,
              int *,fromlen)
{

  return(recvfrom(s,
                buf,
                len,
                flags,
                from,
                fromlen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(int,OS2select,
              int,nfds,
              fd_set *,readfds,
              fd_set *,writefds,
              fd_set *,exceptfds,
              const struct timeval *,timeout)
{
  return(select(nfds,
                readfds,
                writefds,
                exceptfds,
                timeout));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION4(int,OS2send,
              SOCKET,s,
              const char *,buf,
              int,len,
              int,flags)
{
  return(send(s,
              buf,
              len,
              flags));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION6(int,OS2sendto,
              SOCKET,s,
              const char *,buf,
              int,len,
              int,flags,
              const struct sockaddr *,to,
              int,tolen)
{
  return(sendto(s,
              buf,
              len,
              flags,
              to,
              tolen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(int,OS2setsockopt,
              SOCKET,s,
              int,level,
              int,optname,
              const char *,optval,
              int,optlen)
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

    rc = setsockopt(s,level,optname,(char *)&xx, sizeof(xx));
  }
  else 
  if(level == SOL_SOCKET && (optname == SO_SNDBUF || optname == SO_RCVBUF)) {
	ULONG size;

	//SvL: Limit send & receive buffer length to 64k
	size = min(*(ULONG *)optval, 65000);
	rc = setsockopt(s,level,optname, (char *)&size,optlen);
  }
  else {
    rc = setsockopt(s,level,optname,(char *)optval,optlen);
  }

  if (rc == SOCKET_ERROR)
    //OS2WSASetLastError(iTranslateSockErrToWSock(sock_errno()));
    OS2WSASetLastError(WSAEINVAL);
  else
    OS2WSASetLastError(ERROR_SUCCESS);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(int,OS2shutdown,
              SOCKET,s,
              int,how)
{
  return(shutdown(s,
                  how));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(SOCKET,OS2socket,
              int,af,
              int,type,
              int,protocol)
{
  return(socket(af,
                type,
                protocol));
}


/* Database function prototypes */

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION3(struct Whostent *,OS2gethostbyaddr,
              const char *,addr,
              int,len,
              int,type)
{
  WHOSTENT         *yy;
  struct hostent   *xx;
  PWSOCKTHREADDATA pwstd;

  xx = gethostbyaddr((char *)addr,len,type);
  //PH: we assume PMWSOCK sets WSASetLastError correctly!

  if(xx == NULL)
     return (WHOSTENT *)NULL;

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(struct Whostent *,OS2gethostbyname,
              const char *,name)
{
  WHOSTENT         *yy;
  struct hostent   *xx;
  PWSOCKTHREADDATA pwstd;


  xx = gethostbyname((char *)name);
  //PH: we assume PMWSOCK sets WSASetLastError correctly!

  if(xx == NULL)
    return (WHOSTENT *)NULL;

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(int,OS2gethostname,
              char *,name,
              int,namelen)
{
   //PH: we assume PMWSOCK sets WSASetLastError correctly!
   return(gethostname(name,
                     namelen));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(struct Wservent *,OS2getservbyport,
              int,              port,
              const char *,     proto)
{
  struct servent   *xx;
  PWSOCKTHREADDATA pwstd;

  //PH: we assume PMWSOCK sets WSASetLastError correctly!
  xx = getservbyport(port,(char *)proto);

  if(xx == NULL)
    return (WSERVENT *)NULL;

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(struct Wservent *,OS2getservbyname,
              const char *,     name,
              const char *,     proto)
{
  WSERVENT         *yy;
  struct servent   *xx;
  PWSOCKTHREADDATA pwstd;


  //PH: we assume PMWSOCK sets WSASetLastError correctly!
  xx = getservbyname((char *)name,(char *)proto);

  if(xx == NULL)
    return (WSERVENT *)NULL;

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(struct Wprotoent *,OS2getprotobynumber,
              int,proto)
{
  struct protoent  *xx;
  PWSOCKTHREADDATA pwstd;

  //PH: we assume PMWSOCK sets WSASetLastError correctly!
  xx = getprotobynumber(proto);

  if(xx == NULL)
    return (WPROTOENT *)NULL;

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(struct Wprotoent *,OS2getprotobyname,
              const char *,name)
{
  struct protoent  *xx;
  PWSOCKTHREADDATA pwstd;

  //PH: we assume PMWSOCK sets WSASetLastError correctly!
  xx = getprotobyname((char *)name);

  if(xx == NULL)
    return (WPROTOENT *)NULL;

  // access current thread wsock data block
  pwstd = iQueryWsockThreadData();

  pwstd->wptnt.p_name    = xx->p_name;
  pwstd->wptnt.p_aliases = xx->p_aliases;
  pwstd->wptnt.p_proto   = (short)xx->p_proto;

  return &pwstd->wptnt;
}



/* Microsoft Windows Extension function prototypes */

/*****************************************************************************
 * Name      :
 * Purpose   :
 * Parameters:
 * Variables :
 * Result    :
 * Remark    :
 * Status    : UNTESTED STUB
 *
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION2(int,OS2WSAStartup,
              USHORT,wVersionRequired,
              LPWSADATA,lpWSAData)
{
  return(WSAStartup(wVersionRequired,
                    lpWSAData));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION0(int,OS2WSACleanup)
{
  return(WSACleanup());
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION0(BOOL,OS2WSAIsBlocking)
{
  return WSAIsBlocking();
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION0(int,OS2WSAUnhookBlockingHook)
{
  return WSAUnhookBlockingHook();
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(PFN,OS2WSASetBlockingHook,
              PFN,lpBlockFunc)
{
  return(WSASetBlockingHook(lpBlockFunc));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION0(int,OS2WSACancelBlockingCall)
{
  return(WSACancelBlockingCall());
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION6(LHANDLE,OS2WSAAsyncGetServByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              const char *,proto,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETSERVBYNAME,
                       FALSE,
                       buf);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetServByName(hwndRelay,
                              ulNewID,
                              name,
                              proto,
                              buf,
                              buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION6(LHANDLE,OS2WSAAsyncGetServByPort,
              HWND,hWnd,
              u_int,wMsg,
              int,port,
              const char *,proto,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETSERVBYPORT,
                       FALSE,
                       buf);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetServByPort(hwndRelay,
                             ulNewID,
                             port,
                             proto,
                             buf,
                             buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetProtoByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETPROTOBYNAME,
                       FALSE,
                       buf);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetProtoByName(hwndRelay,
                              ulNewID,
                              name,
                              buf,
                              buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetProtoByNumber,
              HWND,hWnd,
              u_int,wMsg,
              int,number,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETPROTOBYNUMBER,
                       FALSE,
                       buf);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetProtoByNumber(hwndRelay,
                                ulNewID,
                                number,
                                buf,
                                buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION5(LHANDLE,OS2WSAAsyncGetHostByName,
              HWND,hWnd,
              u_int,wMsg,
              const char *,name,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETHOSTBYNAME,
                       FALSE,
                       (PVOID)buf, (PVOID)buflen);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetHostByName(hwndRelay,
                             ulNewID,
                             name,
                             buf,
                             buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION7(LHANDLE,OS2WSAAsyncGetHostByAddr,
              HWND,hWnd,
              u_int,wMsg,
              const char *,addr,
              int,len,
              int,type,
              char *,buf,
              int,buflen)
{
  int   rc;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  // add entry to list, we need to store both our temp buffer and the apps buffer
  ulNewID = RelayAlloc(hWnd,
                       wMsg,
                       ASYNCREQUEST_GETHOSTBYADDR,
                       FALSE,
                       buf);

  // call pmwsock function, will fill our temp buffer
  rc = WSAAsyncGetHostByAddr(hwndRelay,
                             ulNewID,
                             addr,
                             len,
                             type,
                             buf,
                             buflen);

  // if an error occurs, free the allocated relay entry
  if (rc == SOCKET_ERROR)
    RelayFree(ulNewID);

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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION1(int,OS2WSACancelAsyncRequest,
              LHANDLE,hAsyncTaskHandle)
{
  return(WSACancelAsyncRequest(hAsyncTaskHandle));
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
 * Author    : Patrick Haller [Thu, 1999/11/25 23:00]
 *****************************************************************************/

ODINFUNCTION4(int,OS2WSAAsyncSelect,
              SOCKET,s,
              HWND,hWnd,
              u_int,wMsg,
              long,lEvent)
{
  int   rc;
//  int   iError;
  HWND  hwndOS2 = Win32ToOS2Handle(hWnd);
  ULONG ulNewID;

  if (hwndRelay == NULL) // already initialized ?
    hwndRelay = RelayInitialize(hwndOS2);

  /* @@@PH: our source window doesn't seem to have an anchor block.
            Docs suggest we've missed to call WinInitialize on the
            caller thread.

            Cause however is the Open32 handle is (of course!) invalid
            in plain PM Window Manager! -> use DAPWSOCK

            Unfortunately, DAPWSOCK calls WinQueryAnchorBlock(hOpen32), too.
            So, we're stuck until I resolve hWnd to it's valid PM
            counterpart.

            new problem: we've ultimately got to use PostMessageA instead
            anything else. => create invisible msg relay window:
            - hMsg = registerMessage(hWnd, wMsg)
            - call WSAAsyncSelect with object window handle
            - overwrite hWnd relay for "same handles"
   */

  // add event to list or remove any list entry in case of WSAAsyncSelect(hwnd,0,0)
  if ( (wMsg == 0) && (lEvent == 0) )
  {
    // remove entry from list
    RelayFreeByHwnd(hWnd);
  }
  else
    // add entry to list
    ulNewID = RelayAlloc(hWnd,
                         wMsg,
                         ASYNCREQUEST_SELECT,
                         TRUE);

  rc = WSAAsyncSelect(s,
                      hwndRelay,
                      ulNewID,
                      lEvent);

//  iError = WSAGetLastError();
//  dprintf(("res=%d, err=%d\n",
//           rc,
//           iError));
  return (rc);
}
