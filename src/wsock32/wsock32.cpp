/* $Id: wsock32.cpp,v 1.9 1999-10-20 20:41:54 phaller Exp $ */

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


// forwarder prototypes
void _System SetLastError(int iError);
int  _System GetLastError(void);


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

// @@@PH not reentrancy proof
static WHOSTENT whsnt;
static WSERVENT wsvnt;
static WPROTOENT wptnt;


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
  return recv(s,buf,len,flags);
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
  return (SOCKET)socket(af,type,protocol);
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
 * Author    : Patrick Haller [Tue, 1998/06/16 23:00]
 *****************************************************************************/

ODINFUNCTION0(int,OS2WSAGetLastError)
{
  return GetLastError();
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


