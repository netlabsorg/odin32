/* $Id: socket.cpp,v 1.10 2001-10-10 19:08:08 phaller Exp $ */
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
