/* $Id: socket.cpp,v 1.16 2003-02-24 11:14:30 sandervl Exp $ */
/*
 * based on Windows Sockets 1.1 specs
 * (ftp.microsoft.com:/Advsys/winsock/spec11/WINSOCK.TXT)
 * 
 * (C) 1993,1994,1996,1997 John Brezak, Erik Bos, Alex Korobka.
 *
 * NOTE: If you make any changes to fix a particular app, make sure 
 * they don't break something else like Netscape or telnet and ftp 
 * clients and servers (www.winsite.com got a lot of those).
 *
 * NOTE 2: Many winsock structs such as servent, hostent, protoent, ...
 * are used with 1-byte alignment for Win16 programs and 4-byte alignment
 * for Win32 programs in winsock.h. winsock2.h uses forced 4-byte alignment.
 * So we have non-forced (just as MSDN) ws_XXXXent (winsock.h), 4-byte forced
 * ws_XXXXent32 (winsock2.h) and 1-byte forced ws_XXXXent16 (winsock16.h).
 */


/*****************************************************************************
 * Includes                                                                  *
 *****************************************************************************/

#include <odin.h>
#include <winsock2.h>
#include <debugtools.h>
#include <odinwrap.h>


ODINDEBUGCHANNEL(WS2_32-SOCKET)


/***********************************************************************
 *      WSACreateEvent()          (WS2_32.???)
 *
 */
ODINFUNCTION0(WSAEVENT, WSACreateEvent)
{
    /* Create a manual-reset event, with initial state: unsignealed */
    
    return CreateEventA(NULL, TRUE, FALSE, NULL);    
}

ODINFUNCTION1(BOOL, WSASetEvent,
              WSAEVENT, hEvent)
{
    return SetEvent(hEvent);
}

/***********************************************************************
 *      WSACloseEvent()          (WS2_32.???)
 *
 */
ODINFUNCTION1(BOOL, WSACloseEvent,
              WSAEVENT, hEvent)
{
    return CloseHandle(hEvent);
}

/***********************************************************************
 *      WSASocketA()          (WS2_32.???)
 *
 */
ODINFUNCTION6(SOCKET, WSASocketA,
              int, af,
              int, type,
              int, protocol,
              LPWSAPROTOCOL_INFOA, lpProtocolInfo,
              GROUP, g,
              DWORD, dwFlags)
{
  dprintf(("WSASocketA incorrectly implemented"));
   /* 
      FIXME: The "advanced" parameters of WSASocketA (lpProtocolInfo,
      g, dwFlags) are ignored.
   */
   
   TRACE("WSASocketA af=%d type=%d protocol=%d protocol_info=%p group=%d flags=0x%lx\n", 
         af, type, protocol, lpProtocolInfo, g, dwFlags );

   return ( socket (af, type, protocol) );
}

/***********************************************************************
 *      WSASocketA()          (WS2_32.???)
 *
 */
ODINFUNCTION6(SOCKET, WSASocketW,
              int, af,
              int, type,
              int, protocol,
              LPWSAPROTOCOL_INFOW, lpProtocolInfo,
              GROUP, g,
              DWORD, dwFlags)
{
  dprintf(("WSASocketW incorrectly implemented"));
   /* 
      FIXME: The "advanced" parameters of WSASocketA (lpProtocolInfo,
      g, dwFlags) are ignored.
   */
   
   TRACE("WSASocketW af=%d type=%d protocol=%d protocol_info=%p group=%d flags=0x%lx\n", 
         af, type, protocol, lpProtocolInfo, g, dwFlags );

   return ( socket (af, type, protocol) );
}
/***********************************************************************
 *              WSAAccept                        (WS2_32.26)
 */
SOCKET WINAPI WSAAccept( SOCKET s, struct sockaddr *addr, LPINT addrlen,
               LPCONDITIONPROC lpfnCondition, DWORD dwCallbackData)
{

       int ret = 0, size = 0;
       WSABUF CallerId, CallerData, CalleeId, CalleeData;
       /*        QOS SQOS, GQOS; */
       GROUP g;
       SOCKET cs;
       SOCKADDR src_addr, dst_addr;

       TRACE("Socket  %u, sockaddr %p, addrlen %p, fnCondition %p, dwCallbackData %ld\n",
               s, addr, addrlen, lpfnCondition, dwCallbackData);

       
       size = sizeof(src_addr);
       cs = accept(s, &src_addr, &size);
       
       if (cs == SOCKET_ERROR) return SOCKET_ERROR;

       CallerId.buf = (char *)&src_addr;
       CallerId.len = sizeof(src_addr);  

       CallerData.buf = NULL;
       CallerData.len = (ULONG)NULL;   

       getsockname(cs, &dst_addr, &size);

       CalleeId.buf = (char *)&dst_addr;
       CalleeId.len = sizeof(dst_addr);  
       
       ret = (*lpfnCondition)(&CallerId, &CallerData, NULL, NULL,
                       &CalleeId, &CalleeData, &g, dwCallbackData); 

       switch (ret)
       {
               case CF_ACCEPT:
                       if (addr && addrlen)
                               addr = (struct sockaddr *)memcpy(addr, &src_addr, (*addrlen > size) ?  size : *addrlen );
                       return cs;
               case CF_DEFER:
                       WSASetLastError(WSATRY_AGAIN);
                       return SOCKET_ERROR;
               case CF_REJECT:
                       closesocket(cs);
                       WSASetLastError(WSAECONNREFUSED);
                       return SOCKET_ERROR;
               default:
                       FIXME("Unknown return type from Condition function\n");
                       WSASetLastError(WSAENOTSOCK);
                       return SOCKET_ERROR;
               }

       WSASetLastError(WSAENOTSOCK);
       return SOCKET_ERROR;
}
//******************************************************************************
//******************************************************************************
/***********************************************************************
 *		WSASendTo		(WS2_32.74)
 */
INT WINAPI WSASendTo( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                      LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
                      const struct sockaddr *to, int tolen,
                      LPWSAOVERLAPPED lpOverlapped,
                      LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    WSASetLastError(WSAENOTSOCK);
    return SOCKET_ERROR;
}
/***********************************************************************
 *		WSASend			(WS2_32.72)
 */
INT WINAPI WSASend( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                    LPDWORD lpNumberOfBytesSent, DWORD dwFlags,
                    LPWSAOVERLAPPED lpOverlapped,
                    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )
{
    DWORD dwBytesSent, tmpret;
    DWORD ret = NO_ERROR;

    dprintf(("WSASend %d %x %d %x %x %x %x", s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags, lpOverlapped, lpCompletionRoutine));

    if(lpBuffers == NULL) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }
    if(lpNumberOfBytesSent == NULL) {
        lpNumberOfBytesSent = &dwBytesSent;
    }
    if(!lpOverlapped && !lpCompletionRoutine) 
    {
        *lpNumberOfBytesSent = 0;

        for(int i=0;i<dwBufferCount;i++) {
            tmpret = send(s, lpBuffers[i].buf, lpBuffers[i].len,  dwFlags);
            if(tmpret != SOCKET_ERROR) {
                *lpNumberOfBytesSent += tmpret;
            }
            else {
                ret = SOCKET_ERROR;
                break;
            }
        }
        if(*lpNumberOfBytesSent) {
            WSASetLastError(NO_ERROR);
            ret = NO_ERROR;
        }
        return ret;
    }
    return WSASendTo ( s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, dwFlags,
                       NULL, 0, lpOverlapped, lpCompletionRoutine );
}
/***********************************************************************
 *              WSARecvFrom             (WS2_32.69)
 */
INT WINAPI WSARecvFrom( SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
                        LPDWORD lpNumberOfBytesRecvd, LPDWORD lpFlags, struct sockaddr *lpFrom,
                        LPINT lpFromlen, LPWSAOVERLAPPED lpOverlapped,
                        LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine )

{
    WSASetLastError(WSAENOTSOCK);
    return SOCKET_ERROR;
}
/***********************************************************************
 *		WSARecv			(WS2_32.67)
 */
int WINAPI WSARecv (SOCKET s, LPWSABUF lpBuffers, DWORD dwBufferCount,
		    LPDWORD lpNumberOfBytesReceived, LPDWORD lpFlags,
		    LPWSAOVERLAPPED lpOverlapped,
		    LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine)
{
    DWORD dwBytesReceived, tmpret, flags;
    DWORD ret = NO_ERROR;

    dprintf(("WSARecv %d %x %d %x %x %x %x", s, lpBuffers, dwBufferCount, lpNumberOfBytesReceived, lpFlags, lpOverlapped, lpCompletionRoutine));
    if(lpBuffers == NULL) {
        WSASetLastError(WSAEINVAL);
        return SOCKET_ERROR;
    }
    if(lpNumberOfBytesReceived == NULL) {
        lpNumberOfBytesReceived = &dwBytesReceived;
    }
    if(lpFlags == NULL) {
        lpFlags = &flags;
        flags = 0;
    }
    if(!lpOverlapped && !lpCompletionRoutine) 
    {
        *lpNumberOfBytesReceived = 0;

        for(int i=0;i<dwBufferCount;i++) {
            tmpret = recv(s, lpBuffers[i].buf, lpBuffers[i].len,  *lpFlags);
            if(tmpret != SOCKET_ERROR) {
                *lpNumberOfBytesReceived += tmpret;
            }
            else {
                ret = SOCKET_ERROR;
                break;
            }
        }
        if(*lpNumberOfBytesReceived) {
            WSASetLastError(NO_ERROR);
            ret = NO_ERROR;
        }
        dprintf(("WSARecv returned %d (read %x)", ret, *lpNumberOfBytesReceived));
        *lpFlags = 0; //what to do with this?
        return ret;
    }
    return WSARecvFrom (s, lpBuffers, dwBufferCount, lpNumberOfBytesReceived, lpFlags,
                        NULL, NULL, lpOverlapped, lpCompletionRoutine);
}

/***********************************************************************
 *              WSAAddressToStringA                      (WS2_32.27)
 *
 *  See WSAAddressToStringW
 */
INT WINAPI WSAAddressToStringA( LPSOCKADDR sockaddr, DWORD len,
                                LPWSAPROTOCOL_INFOA info, LPSTR string,
                                LPDWORD lenstr )
{
    INT size;
    CHAR buffer[22]; /* 12 digits + 3 dots + ':' + 5 digits + '\0' */
    CHAR *p;

    TRACE( "(%p, %d, %p, %p, %p)\n", sockaddr, len, info, string, lenstr );

    if (!sockaddr || len < sizeof(SOCKADDR_IN)) return SOCKET_ERROR;
    if (!string || !lenstr) return SOCKET_ERROR;

    /* sin_family is guaranteed to be the first u_short */
    if (((SOCKADDR_IN *)sockaddr)->sin_family != AF_INET) return SOCKET_ERROR;

    sprintf( buffer, "%u.%u.%u.%u:%u",
             (unsigned int)(ntohl( ((SOCKADDR_IN *)sockaddr)->sin_addr.s_addr ) >> 24 & 0xff),
             (unsigned int)(ntohl( ((SOCKADDR_IN *)sockaddr)->sin_addr.s_addr ) >> 16 & 0xff),
             (unsigned int)(ntohl( ((SOCKADDR_IN *)sockaddr)->sin_addr.s_addr ) >> 8 & 0xff),
             (unsigned int)(ntohl( ((SOCKADDR_IN *)sockaddr)->sin_addr.s_addr ) & 0xff),
             ntohs( ((SOCKADDR_IN *)sockaddr)->sin_port ) );

    p = strchr( buffer, ':' );
    if (!((SOCKADDR_IN *)sockaddr)->sin_port) *p = 0;

    size = strlen( buffer );

    if (*lenstr <  size)
    {
        *lenstr = size;
        WSASetLastError(WSAEFAULT);
        return SOCKET_ERROR;
    }

    strcpy( string, buffer );
    return 0;
}

/***********************************************************************
 *              WSALookupServiceBeginA                       (WS2_32.59)
 */
INT WINAPI WSALookupServiceBeginA( LPWSAQUERYSETA lpqsRestrictions,
                                   DWORD dwControlFlags,
                                   LPHANDLE lphLookup)
{
    FIXME("(%p 0x%08lx %p) Stub!\n", lpqsRestrictions, dwControlFlags,
            lphLookup);
    WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
    return SOCKET_ERROR;
}

/***********************************************************************
 *              WSALookupServiceBeginW                       (WS2_32.60)
 */
INT WINAPI WSALookupServiceBeginW( LPWSAQUERYSETW lpqsRestrictions,
                                   DWORD dwControlFlags,
                                   LPHANDLE lphLookup)
{
    FIXME("(%p 0x%08lx %p) Stub!\n", lpqsRestrictions, dwControlFlags,
            lphLookup);
    WSASetLastError(WSA_NOT_ENOUGH_MEMORY);
    return SOCKET_ERROR;
}
