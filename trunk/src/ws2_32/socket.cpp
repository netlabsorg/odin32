/* $Id */
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

#include <odin.h>
#include <winsock2.h>
#include <debugtools.h>

/***********************************************************************
 *      WSACreateEvent()          (WS2_32.???)
 *
 */
WSAEVENT WINAPI WSACreateEvent(void)
{
    /* Create a manual-reset event, with initial state: unsignealed */
    TRACE("WSACreateEvent");
    
    return CreateEventA(NULL, TRUE, FALSE, NULL);    
}

BOOL WINAPI WSASetEvent(WSAEVENT hEvent)
{
    TRACE("WSASetEvent");
    return SetEvent(hEvent);
}

/***********************************************************************
 *      WSACloseEvent()          (WS2_32.???)
 *
 */
BOOL WINAPI WSACloseEvent(WSAEVENT event)
{
    TRACE ("WSACloseEvent event=0x%x\n", event);

    return CloseHandle(event);
}

/***********************************************************************
 *      WSASocketA()          (WS2_32.???)
 *
 */
SOCKET WINAPI WSASocketA(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOA lpProtocolInfo,
                         GROUP g, DWORD dwFlags)
{
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
SOCKET WINAPI WSASocketW(int af, int type, int protocol,
                         LPWSAPROTOCOL_INFOW lpProtocolInfo,
                         GROUP g, DWORD dwFlags)
{
   /* 
      FIXME: The "advanced" parameters of WSASocketA (lpProtocolInfo,
      g, dwFlags) are ignored.
   */
   
   TRACE("WSASocketW af=%d type=%d protocol=%d protocol_info=%p group=%d flags=0x%lx\n", 
         af, type, protocol, lpProtocolInfo, g, dwFlags );

   return ( socket (af, type, protocol) );
}

/***********************************************************************
 *		WSAEnumNetworkEvents
 */
int WINAPI WSAEnumNetworkEvents(SOCKET s, WSAEVENT hEvent, LPWSANETWORKEVENTS lpEvent)
{
    dprintf(("WSAEnumNetworkEvents %x %x %x NOT IMPLEMENTED", s, hEvent, lpEvent));
    SetLastError(WSAEINVAL);
    return SOCKET_ERROR;
}

/***********************************************************************
 *		WSAEventSelect (WS2_32.39)
 */
int WINAPI WSAEventSelect(SOCKET s, WSAEVENT hEvent, LONG lEvent)
{
    TRACE("WS2_32: WSAEventSelect %08x, hEvent %08x, event %08x\n", s, hEvent, (unsigned)lEvent );
#ifdef DEBUG
    // log all event bits that are set
    char tmpbuf[300];
    strcpy(tmpbuf, "");
    if (lEvent & FD_READ)
        strcat(tmpbuf, " FD_READ");
    if (lEvent & FD_WRITE)
        strcat(tmpbuf, " FD_WRITE");
    if (lEvent & FD_OOB)
        strcat(tmpbuf, " FD_OOB");    
    if (lEvent & FD_ACCEPT)
        strcat(tmpbuf, " FD_ACCEPT");
    if (lEvent & FD_CONNECT)
        strcat(tmpbuf, " FD_CONNECT");
    if (lEvent & FD_CLOSE)
        strcat(tmpbuf, " FD_CLOSE");
    if (lEvent & FD_QOS)
        strcat(tmpbuf, " FD_QOS");
    if (lEvent & FD_GROUP_QOS)
        strcat(tmpbuf, " FD_GROUP_QOS");
    if (lEvent & FD_ROUTING_INTERFACE_CHANGE)
        strcat(tmpbuf, " FD_ROUTING_INTERFACE_CHANGE");
    if (lEvent & FD_ADDRESS_LIST_CHANGE)
        strcat(tmpbuf, " FD_ADDRESS_LIST_CHANGE");
    dprintf(("event bits:%s", tmpbuf));
#endif
    SetLastError(WSAEINVAL);
    return SOCKET_ERROR;
}

